#include "Accessor.h"
#include "Scene.h"
#include "Engine.h"
#include "SceneManager.h"
#include "InputManager.h"
#include "Level.h"
#include "GameWorld.h"
#include "EntityManager.h"
#include "MenuManager.h"

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
	switch (sceneClassID)
	{
	case SceneClassID::SplashScreen:
		changeScene("SplashScreen");
		break;
	case SceneClassID::TitleScreen:
		changeScene("TitleScreen");
		break;
	case SceneClassID::GameWorld:
		changeScene("GameWorld");
		break;
	}
}

void ssge::ScenesAccess::changeScene(std::string newSceneId)
{
	if (!actual)return; // Null-safety

	actual->changeScene(gameScenes.createScene(PassKey<ScenesAccess>(), newSceneId));
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

void ssge::ScenesAccess::restart()
{
	auto currentSceneClassID = actual->getCurrentSceneClassID();
	if (currentSceneClassID == SceneClassID::GameWorld)
	{
		GameWorld* gameWorldScene = dynamic_cast<GameWorld*>(actual->getCurrentScene());
		goToLevel(gameWorldScene->getWantedLevel());
	}
	else
	{
		changeScene(currentSceneClassID);
	}
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

void LevelAccess::rectToBlockSpan(const SDL_FRect& r, int& col0, int& col1, int& row0, int& row1) const
{
	return actual->rectToBlockSpan(r, col0, col1, row0, row1);
}

Level::Block* ssge::LevelAccess::getBlockAt(Level::Block::Coords coords)
{
	return actual->getBlockAt(coords);
}

const Level::Block* ssge::LevelAccess::getConstBlockAt(Level::Block::Coords coords) const
{
	return actual->getBlockAt(coords);
}

SDL_Rect ssge::LevelAccess::calculateLevelSize() const
{
	return actual->calculateLevelSize();
}

Level::BlockQuery LevelAccess::queryBlock(int col, int row) const
{
	return actual->queryBlock(col, row);
}

Level::BlockQuery LevelAccess::queryBlock(SDL_FPoint positionInLevel) const
{
	return actual->queryBlock(positionInLevel);
}

std::vector<Level::BlockQuery> ssge::LevelAccess::queryBlocksUnderCollider(SDL_FRect collider) const
{
	return actual->queryBlocksUnderCollider(collider);
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

EntityReference ssge::EntitiesAccess::addEntity(std::string entityID)
{
	return actual->addEntity(gameEntities.createEntity(PassKey<EntitiesAccess>(), entityID));
}

const ssge::Sprite::Definition* ssge::SpritesAccess::fetchDefinition(std::string sprdefId)
{
	return actual.fetchDefinition(sprdefId);
}

std::unique_ptr<ssge::Sprite> ssge::SpritesAccess::create(std::string sprdefId)
{
	auto sprdef = fetchDefinition(sprdefId);
	if (sprdef)
		return std::make_unique<ssge::Sprite>(*sprdef);
	else return nullptr;
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

void ssge::GameWorldAccess::reportHeroDeadth()
{
	currentGameWorld->reportHeroDeadth();
}
