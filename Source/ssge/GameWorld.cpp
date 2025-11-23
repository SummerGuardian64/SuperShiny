#include "GameWorld.h"
#include <string>
#include "DrawContext.h"
#include "PassKey.h"
#include "Accessor.h"
#include <algorithm>

using namespace ssge;

bool GameWorld::initLevel(SceneStepContext& context)
{
    if (!level)
    {
        std::string tilesetPath;
        std::string error;
        std::string path = "Levels/level" + std::to_string(wantedLevel) + ".ini";

        auto lvl = levelLoader.loadLevel(path.c_str());

        if (!lvl)
        {
            std::cout << "There were errors during loading level from " << path << std::endl << levelLoader.getErrorLog();
        }
        else
        {
            lvl->loadTextures(context.drawing.getRenderer());
            auto musicPath = lvl->getMusicPath();
            if (!musicPath.empty())
            {
                context.audio.playMusicIfNotPlaying(musicPath);
            }
            level = std::move(lvl);
        }
    }

    // Return true if the level is initialized successfully.
    // Return false if there's a problem
    return (level != nullptr);
}

GameWorld::GameWorld() :
    levelLoader(PassKey<GameWorld>()),
    entities(PassKey<GameWorld>())
{
    backgroundColor = SDL_Color{ 0,0,255,255 };
    setConfines(SDL_FRect{ 0,0,1280,720 });
    gameplayOver = false;
    wantedLevel = 0;
}

GameWorld::GameWorld(int wantedLevel) :
    levelLoader(PassKey<GameWorld>()),
    entities(PassKey<GameWorld>())
{
    backgroundColor = SDL_Color{ 0,0,255,255 };
    setConfines(SDL_FRect{ 0,0,1280,720 });
    gameplayOver = false;

    this->wantedLevel = wantedLevel;
}

GameWorld* GameWorld::tryCast(Scene* scene)
{
    if (scene->getSceneClassID() != "GameWorld")
        return dynamic_cast<GameWorld*>(scene);
    else return nullptr;
}

Scene& GameWorld::getAsScene()
{
    return *this;
}

void GameWorld::reportHeroDeadth()
{
    heroDied = true;
}

bool GameWorld::isGameplayOver() const
{
	return false;
}

void GameWorld::finishGameplay()
{
    gameplayOver = true;
}

int GameWorld::getWantedLevel() const
{
    return wantedLevel;
}

SDL_FRect GameWorld::getConstConfines() const
{
	return confines;
}

SDL_FRect GameWorld::getConfines() const
{
	return confines;
}

void GameWorld::setConfines(SDL_FRect confines)
{
	this->confines = confines;
}

std::string GameWorld::getSceneClassID() const
{
    return "GameWorld";
}

void GameWorld::init(SceneStepContext& context)
{
    bool levelSuccessful = initLevel(context);

    if (!levelSuccessful)
    {
        // ERROR! Get out of there!
        std::cout << levelLoader.getErrorLog() << std::endl;
        // TODO: Better error handling!
        context.scenes.pause();
        context.scenes.goToMainMenu();
    }
    else
    {
        auto& spawnList = levelLoader.getSpawnList();

        int currentEntityIndex = 0;
        for (const auto& spawnEntry : spawnList)
        {
            // FIXME: BIG BODGE!
            EntitiesAccess bodge(&entities, context.game);
            EntityReference entity = bodge.addEntity(spawnEntry.what);
            if (!entity)
                continue;

            entity->position = spawnEntry.where;

            if (levelLoader.getHeroIndex() == currentEntityIndex)
            {
                heroEntity = entity;
            }

            currentEntityIndex++;
        }
    }
}

void GameWorld::step(SceneStepContext& context)
{
    //// Step all entities
    GameWorldStepContext gameWorldStepContext(
        PassKey<GameWorld>(),
        context.deltaTime,
        context.engine,
        context.game,
        context.scenes,
        context.inputs,
        context.drawing,
        context.currentScene,
        GameWorldAccess(this),
        LevelAccess(level.get())
    );

    // TODO: Better deadth handling
    // See if heroEntity doesn't exist, then tell IGame about it
    // But that requires a fully fledged super duper ultra hyper mega event system that I don't have time for
    if (heroDied)
    {
        context.scenes.restart();
    }

    entities.step(gameWorldStepContext);

    // TODO: Decouple heroEntity from entityToScrollTo
    if (auto e = heroEntity.get())
    {
        scrollTarget = e->position;
    }

    // Warp check

    if (auto hero = heroEntity.get())
    {
        auto warpQuery = level->queryBlock(hero->position);
        if (warpQuery.coll == Level::Block::Collision::NextSection)
        {
            if (level->nextSection != -1)
            {
                context.scenes.goToLevel(level->nextSection);
            }
        }
        if (warpQuery.coll == Level::Block::Collision::Victory)
        {
            context.game.declareVictory();
        }
    }
}

void GameWorld::draw(DrawContext& context)
{
    SDL_Renderer* renderer = context.getRenderer();

    // Draw background color
    SDL_SetRenderDrawColor(
        renderer,
        backgroundColor.r,
        backgroundColor.g,
        backgroundColor.b,
        backgroundColor.a
    );
    SDL_Rect bounds = context.getBounds();
    SDL_RenderFillRect(renderer, &bounds);

    // Default scroll offset is at half of the screen size
    SDL_Rect screenSize = context.getBounds();
    SDL_Rect halfScreen{ 0, 0, screenSize.w / 2,screenSize.h / 2 };
    SDL_Point centerPoint{ halfScreen.w,halfScreen.h };

    // If level exists, base scrolling off of it and draw it.
    if (level)
    {
        SDL_Rect levelSize = level->calculateLevelSize();

        // Make sure the center point doesn't nag scrollingOffset out of the screen.
        // Exceptionally, if the level is shorter than the screen by any axis,
        // set that axis of the scrollingOffset to half of the level's size by that axis.

        if (levelSize.w < screenSize.w)
            centerPoint.x = levelSize.w / 2;
        else
            centerPoint.x = std::clamp((int)scrollTarget.x, halfScreen.w, levelSize.w - halfScreen.w);

        if (levelSize.h < screenSize.h)
            centerPoint.y = levelSize.h / 2;
        else
            centerPoint.y = std::clamp((int)scrollTarget.y, halfScreen.h, levelSize.h - halfScreen.h);

        // View offset (top-left in world)
        SDL_Point viewOffset{
            int(centerPoint.x - halfScreen.w + 0.5f),
            int(centerPoint.y - halfScreen.h + 0.5f)
        };

        // Derive the current draw context for scrolling with the given offset
        DrawContext scrolledContext = context.deriveForScrolling(viewOffset);

        // Draw the level
        level->draw(scrolledContext);

        // Draw all entities
        entities.draw(scrolledContext);
    }
    else
    {
        // Draw entities without level-based scrolling

        // View offset (top-left in world)
        SDL_Point viewOffset{
            int(centerPoint.x - halfScreen.w + 0.5f),
            int(centerPoint.y - halfScreen.h + 0.5f)
        };

        // Derive the current draw context for scrolling with the given offset
        DrawContext scrolledContext = context.deriveForScrolling(viewOffset);

        // Draw all entities
        entities.draw(scrolledContext);
    }

    // Draw HUD without scrolling
    drawHUD(context);
}

void GameWorld::drawHUD(DrawContext& context) const
{
}
