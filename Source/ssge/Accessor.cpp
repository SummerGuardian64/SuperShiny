#include "Accessor.h"
#include "Engine.h"
#include "WindowManager.h"
#include "AudioManager.h"
#include "SceneManager.h"
#include "InputManager.h"
#include "Scene.h"
#include "Level.h"
#include "GameWorld.h"
#include "EntityManager.h"
#include "MenuSystem.h"

using namespace ssge;

void EngineAccess::finish()
{
	if (!actual) return;

	actual->finish();
}

void EngineAccess::wrapUp()
{
	if (!actual) return;

	actual->wrapUp();
}

bool ssge::EngineAccess::isWrappingUp() const
{
	if (!actual)return false;

	return actual->isWrappingUp();
}

void ssge::ScenesAccess::changeScene(std::string newSceneId)
{
	if (!actual)return; // Null-safety

	actual->changeScene(gameScenes.createScene(PassKey<ScenesAccess>(), newSceneId));
}

std::string ssge::ScenesAccess::getCurrentSceneClassID() const
{
	if (!actual)return "";

	return actual->getCurrentSceneClassID();
}

void ssge::ScenesAccess::goToLevel(int wantedLevel)
{
	actual->changeScene(std::make_unique<GameWorld>(wantedLevel));
}

void ssge::ScenesAccess::goToMainMenu()
{
	std::string mainMenuSceneClassID = gameScenes.getMainMenuSceneClassID();
	if(!mainMenuSceneClassID.empty())
		changeScene(mainMenuSceneClassID);
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
	if (currentSceneClassID == "GameWorld")
	{
		GameWorld* gameWorldScene = dynamic_cast<GameWorld*>(actual->getCurrentScene());
		goToLevel(gameWorldScene->getWantedLevel());
	}
	else
	{
		changeScene(currentSceneClassID);
	}
}

// WindowAccess

SDL_Window* WindowAccess::getWindow() const
{
	if (!actual) return nullptr;

	return actual->getWindow();
}

SDL_Surface* WindowAccess::getWindowSurface() const
{
	if (!actual) return nullptr;

	return actual->getWindowSurface();
}

SDL_Renderer* WindowAccess::getRenderer() const
{
	if (!actual) return nullptr;

	return actual->getRenderer();
}

int WindowAccess::getVirtualWidth() const
{
	if (!actual) return 0;

	return actual->getVirtualWidth();
}

int WindowAccess::getVirtualHeight() const
{
	if (!actual) return 0;

	return actual->getVirtualHeight();
}

bool WindowAccess::isUpscaleIntegral() const
{
	if (!actual)return false;

	return actual->isUpscaleIntegral();
}

void WindowAccess::setIntegralUpscale(bool integralUpscale)
{
	if (actual) actual->setIntegralUpscale(integralUpscale);
}

void WindowAccess::setBorderedFullScreen(bool borderedFullScreen)
{
	if (actual) actual->setBorderedFullScreen(borderedFullScreen);
}

SDL_Rect WindowAccess::makeBestFitScale() const
{
	if (!actual) return SDL_Rect{ 0,0,0,0 };

	return actual->makeBestFitScale();
}

// AudioAccess

void AudioAccess::setMasterVolume(int v)
{
	if (!actual)return;
	actual->setMasterVolume(v);
}

void AudioAccess::setMusicVolume(int v)
{
	if (!actual)return;
	actual->setMusicVolume(v);
}

void AudioAccess::setSfxVolume(int v)
{
	if (!actual)return;
	actual->setSfxVolume(v);
}

int  AudioAccess::getMasterVolume() const
{
	if (!actual)return 0;
	return actual->getMasterVolume();
}

int  AudioAccess::getMusicVolume() const
{
	if (!actual)return 0;
	return actual->getMusicVolume();
}

int  AudioAccess::getSfxVolume() const
{
	if (!actual)return 0;
	return actual->getSfxVolume();
}

bool AudioAccess::playMusic(const std::string& path, int loops)
{
	if (!actual)return false;
	return actual->playMusic(path, loops);
}

void AudioAccess::stopMusic()
{
	if (!actual)return;
	actual->stopMusic();
}

void AudioAccess::pauseMusic()
{
	if (!actual)return;
	actual->pauseMusic();
}

void AudioAccess::resumeMusic()
{
	if (!actual)return;
	actual->resumeMusic();
}

bool AudioAccess::loadSfx(const std::string& path)
{
	if (!actual)return false;
	return actual->loadSfx(path);
}

int  AudioAccess::playSfx(const std::string& path, int loops)
{
	if (!actual)return 0;
	return actual->playSfx(path, loops);
}

bool AudioAccess::isMusicPlaying() const
{
	if (!actual)return false;
	return actual->isMusicPlaying();
}

bool AudioAccess::isMusicPlaying(const std::string& path) const
{
	if (!actual)return false;
	return actual->isMusicPlaying(path);
}

bool AudioAccess::playMusicIfNotPlaying(const std::string& path, int loops)
{
	if (!actual)return false;
	return actual->playMusicIfNotPlaying(path, loops);
}

// InputsAccess

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

bool InputsAccess::isListeningForBinding() const
{
	if (!actual)return false;
	return actual->isListeningForBinding();
}

void InputsAccess::listenForBinding(int bindingIndex)
{
	if (!actual)return;
	else actual->listenForBinding(bindingIndex);
}

void InputsAccess::stopListeningForBinding()
{
	if (!actual)return;
	else actual->stopListeningForBinding();
}

InputBinding InputsAccess::getBinding(int bindingIndex) const
{
	if (!actual) return InputBinding();
	else return actual->getBinding(bindingIndex);
}

std::string ssge::InputsAccess::getBindingString(int bindingIndex) const
{
	if (!actual)return "(err: InputsAccess->actual==nullptr)";
	else return actual->getBindingString(bindingIndex);
}

int InputsAccess::getMaxBindings() const
{
	if (!actual)return 0;
	else return actual->getMaxBindings();
}

bool InputsAccess::loadFromIniFile(IniFile& iniFile)
{
	if (!actual)return false;
	else return actual->loadFromIniFile(iniFile);
}

bool InputsAccess::saveToIniFile(IniFile& iniFile)
{
	if (!actual)return false;
	else return actual->saveToIniFile(iniFile);
}

std::string CurrentSceneAccess::getSceneClassID() const
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
	if (!actual)return false;

	return actual->isOpen();
}

void ssge::MenusAccess::setMenu(MenuHeader& menuHeader)
{
	setMenu(&menuHeader);
}

void ssge::MenusAccess::setMenu(MenuHeader* menuHeader)
{
	if (!actual) return;

	actual->setMenu(menuHeader);
}

void ssge::MenusAccess::openMainMenu()
{
	//TODO: Wire up IGameMenus
}

void ssge::MenusAccess::openPauseMenu()
{
	//actual->pushMenu(2);
}

void ssge::MenusAccess::close()
{
	actual->close();
}

void ssge::GameWorldAccess::reportHeroDeadth()
{
	if(currentGameWorld) currentGameWorld->reportHeroDeadth();
}

MenuCommandEx ssge::GameAccess::onHavingBackedOutOfMenus(PassKey<MenuManager> pk, MenuContext& context)
{
	return actual.onHavingBackedOutOfMenus(PassKey<GameAccess>(), context);
}

void GameAccess::saveSettings()
{
	actual.saveSettings();
}
