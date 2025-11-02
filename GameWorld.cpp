#include "GameWorld.h"
#include <string>
#include "DrawContext.h"
#include "Game.h"
#include "PassKey.h"
#include "Accessor.h"
#include <algorithm>

using namespace ssge;

bool ssge::GameWorld::initLevel(SceneStepContext& context)
{
    if (!level)
    {
        std::string tilesetPath;
        std::string error;
        std::string path = "Levels/level" + std::to_string(wantedLevel) + ".ini";
        auto loader = Level::Loader(PassKey<GameWorld>());

        auto lvl = loader.loadLevel(path.c_str());

        if (!lvl)
        {
            std::cout << "There were errors during loading level from " << path << std::endl << loader.getErrorLog();
        }
        else
        {/*
            if (!tilesetPath.empty())
            {
                lvl->setTileset(SdlTexture(tilesetPath.c_str(), context.drawing.getRenderer()));
            }*/
            lvl->loadTileset(context.drawing.getRenderer());
            level = std::move(lvl);
        }
    }

    // Return true if the level is initialized successfully.
    // Return false if there's a problem
    return (level != nullptr);
}

GameWorld::GameWorld()
{
    backgroundColor = SDL_Color{ 0,0,255,255 };
    setConfines(SDL_FRect{ 0,0,1280,720 });
    gameplayOver = false;
    wantedLevel = 0;
}

GameWorld::GameWorld(int wantedLevel)
{
    backgroundColor = SDL_Color{ 0,0,255,255 };
    setConfines(SDL_FRect{ 0,0,1280,720 });
    gameplayOver = false;

    this->wantedLevel = wantedLevel;
}

Scene& ssge::GameWorld::getAsScene()
{
    return *this;
}

bool GameWorld::isGameplayOver() const
{
	return false;
}

void GameWorld::finishGameplay()
{
    gameplayOver = true;
}

SDL_FRect GameWorld::getConstConfines() const
{
	return confines;
}

SDL_FRect GameWorld::getConfines()
{
	return confines;
}

void GameWorld::setConfines(SDL_FRect confines)
{
	this->confines = confines;
}

SceneClassID GameWorld::getSceneClassID() const
{
	return SceneClassID::GameWorld;
}

void GameWorld::init(SceneStepContext& context)
{
    bool levelSuccessful = initLevel(context);

    if (!levelSuccessful)
    {
        // ERROR! Get out of there!
        // TODO! Report the error
        context.scenes.pause();
        context.scenes.changeScene(SceneClassID::TitleScreen);
    }
    else
    {
        auto shiny = entities.addEntity(EntityClassID::Shiny);

        entityToScrollTo = shiny;
    }

    //std::string error;
    //level = LevelLoader::loadLevel(game.progress.getLevel(), error);
    //if (level == NULL)
    //{
    //    //TODO: Error handling
    //    // Go to some error scene and pass it the error string
    //    std::cerr << error;
    //    game.gotoTitleScreen();
    //    return;
    //}
    //level->initPowerupTypes(powerupTypes);
    //level->loadResources();
    //game.backgroundMusic.loadSong(level->backgroundMusicPath, true, true);

    //// Spawn the paddle
    //addEntity(std::make_shared<Paddle>(*this));

    //// Spawn a new ball
    //spawnNewBall();

    //// Give the player a hint
    //hints.playerEnteredNewLevel();
}

void GameWorld::step(SceneStepContext& context)
{
    //// Step all entities
    GameWorldStepContext gameWorldStepContext(
        ssge::PassKey<GameWorld>(),
        context.deltaTime,
        context.engine,
        context.scenes,
        context.inputs,
        context.drawing,
        context.currentScene,
        GameWorldAccess(this),
        LevelAccess(level.get())
    );

    entities.step(gameWorldStepContext);

    if (auto e = entityToScrollTo.get())
    {
        scrollTarget = e->position;
    }

    if (context.inputs.isJustPressed(7))
    {
        context.scenes.pause();
        context.menus.openPauseMenu();
    }

    //// TODO: game victory/loss criteria check

    //// If the gameplay isn't over yet, process the game
    //if (!isGameplayOver())
    //{
    //    // If all balls have been destroyed
    //    if (countAllEntities(EntityClassID::Ball) == 0)
    //    {
    //        // That means that player lost a life
    //        if (!game.progress.loseLife())
    //        { // If there's remaining lives
    //            // Give the player another ball
    //            spawnNewBall();

    //            // Update the hint
    //            hints.playerLostALife();
    //        }
    //        else
    //        { // If there's no more lives
    //            // Tell the player they lost
    //            hints.playerLostAllLives();

    //            // Halt the gameplaay
    //            finishGameplay();

    //            // Play game over
    //            game.backgroundMusic.loadGameOverSong(true, false);

    //            // Open the game over screen
    //            game.menus.openGameOver();
    //        }
    //    }
    //}
    // Here would be the hints
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
    SDL_RenderFillRect(renderer, &context.getBounds());

    // Default scroll offset is at half of the screen size
    SDL_Rect screenSize = context.getBounds();
    SDL_FRect halfScreen{ 0, 0, screenSize.w / 2,screenSize.h / 2 };
    SDL_FPoint centerPoint{ halfScreen.w,halfScreen.h };

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
            centerPoint.x = std::clamp(scrollTarget.x, halfScreen.w, levelSize.w - halfScreen.w);

        if (levelSize.h < screenSize.h)
            centerPoint.y = levelSize.h / 2;
        else
            centerPoint.y = std::clamp(scrollTarget.y, halfScreen.h, levelSize.h - halfScreen.h);

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
    /*sf::Font& font = game.getFont();
    Text::fastDraw(renderTarget, font, sf::String("LIVES: ") + std::to_string(game.progress.getLives()), 50, 50, 24, TextAlign::Left);
    Text::fastDraw(renderTarget, font, sf::String("SCORE: ") + std::to_string(game.progress.getScore()), 1280.f / 2 - 50.f / 2, 50, 24, TextAlign::Center);
    Text::fastDraw(renderTarget, font, sf::String("LEVEL: ") + std::to_string(game.progress.getLevel()), 1280 - 50, 50, 24, TextAlign::Right);

    Text::fastDraw(renderTarget, font, sf::String(hints.makeHintString()), 1280.f / 2, 720.f / 3 * 2, 24, TextAlign::Center);*/
}
