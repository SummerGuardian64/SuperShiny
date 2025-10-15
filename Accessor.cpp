#include "Accessor.h"
#include "Scene.h"
#include "Engine.h"
#include "SceneManager.h"
#include "InputManager.h"
#include "Level.h"

using namespace ssge;

void EngineAccess::finish()
{
	actual->finish();
}

void EngineAccess::wrapUp()
{
	actual->wrapUp();
}

void ScenesAccess::changeScene(SceneClassID newScene)
{
	//FIXME: UNIMPLEMENTED!
}

void ScenesAccess::pause(bool pause)
{
	actual->setPause(pause);
}

void ScenesAccess::unpause()
{
	actual->unpause();
}

bool ScenesAccess::isPaused()
{
	return actual->isPaused();
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
