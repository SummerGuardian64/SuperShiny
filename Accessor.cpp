#include "Accessor.h"
#include "Scene.h"
#include "Engine.h"
#include "SceneManager.h"
#include "InputManager.h"
#include "Level.h"
#include "SplashScreen.h"
#include "GameWorld.h"

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
		//newScene = std::make_unique<TitleScreen>();
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

bool InputsAccess::isPressed(int buttonIndex)
{
	return actual->getPad().checkPressed(1 << buttonIndex);
}

bool InputsAccess::isJustPressed(int buttonIndex)
{
	return actual->getPad().checkJustPressed(1 << buttonIndex);
}

bool InputsAccess::isJustReleased(int buttonIndex)
{
	return actual->getPad().checkJustReleased(1 << buttonIndex);
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

//auto LevelAccess::sweepAABB(const SDL_FRect& box, SDL_FPoint d) const
//{
//	//FIXME: UNIMPLEMENTED!
//
//}
