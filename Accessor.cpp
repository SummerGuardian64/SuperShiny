#include "Accessor.h"
#include "Scene.h"
#include "Engine.h"
#include "SceneManager.h"
#include "InputManager.h"
#include "Level.h"
#include "SplashScreen.h"
#include "GameWorld.h"
#include "EntityManager.h"
#include "MenuManager.h"
#include "TitleScreen.h"

using namespace ssge;

void EngineAccess::finish()
{
	actual->finish();
}

void EngineAccess::wrapUp()
{
	actual->wrapUp();
}

void ScenesAccess::changeScene(SceneClassID sceneClassID)
{
	std::unique_ptr<Scene> newScene;
	switch (sceneClassID)
	{
	case SceneClassID::SplashScreen:
		newScene = std::make_unique<SplashScreen>();
		break;
	case SceneClassID::TitleScreen:
		newScene = std::make_unique<TitleScreen>();
		break;
	case SceneClassID::GameWorld:
		newScene = std::make_unique<GameWorld>();
		break;
	}
	actual->changeScene(std::move(newScene));
}

void ssge::ScenesAccess::goToLevel(int wantedLevel)
{
	actual->changeScene(std::make_unique<GameWorld>(wantedLevel));
}

void ScenesAccess::pause()
{
	actual->pause();
}

void ScenesAccess::unpause()
{
	actual->unpause();
}

bool ScenesAccess::isPaused() const
{
	return actual->isPaused();
}

void ScenesAccess::setPause(bool pause)
{
	return actual->setPause(pause);
}

bool ssge::ScenesAccess::isFadeFinished() const
{
	return actual->isFadeFinished();
}

bool InputsAccess::isPressed(int buttonIndex)
{
	return actual->getPad().isPressed(buttonIndex);
}

bool InputsAccess::isJustPressed(int buttonIndex)
{
	return actual->getPad().isJustPressed(buttonIndex);
}

bool InputsAccess::isJustReleased(int buttonIndex)
{
	return actual->getPad().isJustReleased(buttonIndex);
}

uint32_t InputsAccess::getCurrentButtonsForPlayer(int playerId)
{
	if (playerId == 0)
	{
		return actual->getPad().getPressedButtons();
	}
	else return 0;
	//TODO: Support for more players!
}

SceneClassID CurrentSceneAccess::getSceneClassID() const
{
	return currentScene->getSceneClassID();
}

bool LevelAccess::valid() const
{
	return actual->getTilesetMeta().isValid();
}

bool LevelAccess::rectOverlapsSolid(const SDL_FRect& r) const
{
	//FIXME: UNIMPLEMENTED!
	return false;
}

void LevelAccess::rectToTileSpan(const SDL_FRect& r, int& col0, int& col1, int& row0, int& row1) const
{
	return actual->rectToTileSpan(r, col0, col1, row0, row1);
}

Level::BlockQuery LevelAccess::queryTile(int col, int row) const
{
	return actual->queryTile(col, row);
}

Level::SweepHit LevelAccess::sweepHorizontal(const SDL_FRect& rect, float dx) const
{
	return actual->sweepHorizontal(rect, dx);
}

Level::SweepHit LevelAccess::sweepVertical(const SDL_FRect& rect, float dy) const
{
	return actual->sweepVertical(rect, dy);
}

bool LevelAccess::rectInWater(const SDL_FRect& r) const
{
	return actual->rectInWater(r);
}

EntityReference ssge::EntitiesAccess::addEntity(EntityClassID entityClassID)
{
	return actual->addEntity(entityClassID);
}

bool ssge::MenusAccess::isOpen() const
{
	return actual->isOpen();
}

void ssge::MenusAccess::openMainMenu()
{
	actual->pushMenu(0);
}

void ssge::MenusAccess::openPauseMenu()
{
	actual->pushMenu(2);
}

void ssge::MenusAccess::close()
{
	actual->close();
}
