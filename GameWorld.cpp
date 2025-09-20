#include "GameWorld.h"
#include <string>

using namespace ssge;

GameWorld::GameWorld()
{
    backgroundColor = SDL_Color{ 0,0,255,255 };
    setConfines(SDL_FRect{ 0,0,1280,720 });
    gameplayOver = false;
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
    // TODO: entities.step(...)
    
    // TODO: game victory/loss criteria check

    // Here would be the hints
}

void GameWorld::draw(SceneDrawContext& context)
{
    // Draw background color
    //renderTarget.clear(backgroundColor);

    // Draw level
    //if (auto realLevel = level.get())
    //{
    //    realLevel->draw(renderTarget);
    //}

    //// Draw all entities
    //for (auto& entityPtr : entities)
    //{
    //    // Don't draw an entity immediately! Give it time to initialize!
    //    if (entityPtr->getLifespan().asSeconds() > 0)
    //    {
    //        entityPtr->draw(renderTarget);
    //    }
    //}

    drawHUD(context);
}

void GameWorld::drawHUD(SceneDrawContext& context) const
{
    /*sf::Font& font = game.getFont();
    Text::fastDraw(renderTarget, font, sf::String("LIVES: ") + std::to_string(game.progress.getLives()), 50, 50, 24, TextAlign::Left);
    Text::fastDraw(renderTarget, font, sf::String("SCORE: ") + std::to_string(game.progress.getScore()), 1280.f / 2 - 50.f / 2, 50, 24, TextAlign::Center);
    Text::fastDraw(renderTarget, font, sf::String("LEVEL: ") + std::to_string(game.progress.getLevel()), 1280 - 50, 50, 24, TextAlign::Right);

    Text::fastDraw(renderTarget, font, sf::String(hints.makeHintString()), 1280.f / 2, 720.f / 3 * 2, 24, TextAlign::Center);*/
}
