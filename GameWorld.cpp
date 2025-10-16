#include "GameWorld.h"
#include <string>
#include "DrawContext.h"
#include "Game.h"
#include "PassKey.h"
#include "Accessor.h"

using namespace ssge;

void ssge::GameWorld::initLevel(SceneStepContext& context)
{
    if (!level)
    {
        std::size_t sz = 0;
        const unsigned char* bytes = ssge::Game::Levels::getBytes(wantedLevel, sz);
        if (bytes && sz > 0)
        {
            std::string tilesetPath;
            std::string error;
            auto lvl = ssge::Level::loadFromBytes(bytes, sz, &tilesetPath, &error);
            if (!lvl)
            {
                std::cout << error << std::endl;
            }
            else
            {
                // If you want, load the tileset here using tilesetPath:
                if (!tilesetPath.empty())
                {
                    lvl->setTileset(SdlTexture(tilesetPath.c_str(), context.drawing.getRenderer()));
                }
                level = std::move(lvl);
            }
        }
    }
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
    initLevel(context);

    entities.addEntity(EntityClassID::Shiny);

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
        GameWorldAccess(this)
    );


    entities.step(gameWorldStepContext);

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

    // Draw level
    if (level)
    {
        level->draw(context);
    }

    // Draw all entities
    entities.draw(context);

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
