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

EngineAccessRestrained ssge::EngineAccess::restrainAccess() const
{
	return EngineAccessRestrained(actual);
}

void EngineAccess::finish()
{
	if (actual) actual->finish();
}

void EngineAccess::wrapUp()
{
	if (actual) actual->wrapUp();
}

bool EngineAccess::isWrappingUp() const
{
	if (!actual)return false;

	return actual->isWrappingUp();
}

void ScenesAccess::changeScene(std::string newSceneId)
{
	if (actual)
		actual->changeScene(
			gameScenes.createScene(PassKey<ScenesAccess>(), newSceneId)
		);
}

std::string ScenesAccess::getCurrentSceneClassID() const
{
	if (!actual)return "";

	return actual->getCurrentSceneClassID();
}

void ScenesAccess::goToLevel(int wantedLevel)
{
	if (actual) actual->changeScene(std::make_unique<GameWorld>(wantedLevel));
}

void ScenesAccess::goToMainMenu()
{
	std::string mainMenuSceneClassID = gameScenes.getMainMenuSceneClassID();
	if(!mainMenuSceneClassID.empty())
		changeScene(mainMenuSceneClassID);
}

void ScenesAccess::pause()
{
	if (actual) actual->pause();
}

void ScenesAccess::unpause()
{
	if (actual) actual->unpause();
}

bool ScenesAccess::isPaused() const
{
	if (!actual)return false;

	return actual->isPaused();
}

uint8_t ScenesAccess::getFadeVal() const
{
	if (!actual) return 0;

	return actual->getFadeVal();
}

void ScenesAccess::setPause(bool pause)
{
	if (actual) return actual->setPause(pause);
}

bool ScenesAccess::isFadeFinished() const
{
	if (!actual)return false;

	return actual->isFadeFinished();
}

void ScenesAccess::restart()
{
	if (!actual) return;

	auto currentSceneClassID = actual->getCurrentSceneClassID();
	if (currentSceneClassID == "GameWorld")
	{
		// We need to get the wanted level if we want to restart the level

		GameWorld* gameWorldScene =
			dynamic_cast<GameWorld*>(actual->getCurrentScene());

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
	if (actual)
		actual->setIntegralUpscale(integralUpscale);
}

void WindowAccess::setBorderedFullScreen(bool borderedFullScreen)
{
	if (actual)
		actual->setBorderedFullScreen(borderedFullScreen);
}

SDL_Rect WindowAccess::makeBestFitScale() const
{
	if (!actual) return SDL_Rect{ 0,0,0,0 };

	return actual->makeBestFitScale();
}

// AudioAccess

void AudioAccess::setMasterVolume(int v)
{
	if (actual)
		actual->setMasterVolume(v);
}

void AudioAccess::setMusicVolume(int v)
{
	if (actual)
		actual->setMusicVolume(v);
}

void AudioAccess::setSfxVolume(int v)
{
	if (actual)
		actual->setSfxVolume(v);
}

int AudioAccess::getMasterVolume() const
{
	if (!actual)return 0;

	return actual->getMasterVolume();
}

int AudioAccess::getMusicVolume() const
{
	if (!actual)return 0;

	return actual->getMusicVolume();
}

int AudioAccess::getSfxVolume() const
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
	if (actual)
		actual->stopMusic();
}

void AudioAccess::pauseMusic()
{
	if (actual)
		actual->pauseMusic();
}

void AudioAccess::resumeMusic()
{
	if (actual)
		actual->resumeMusic();
}

bool AudioAccess::loadSfx(const std::string& path)
{
	if (!actual)return false;

	return actual->loadSfx(path);
}

int AudioAccess::playSfx(const std::string& path, int loops)
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

bool InputsAccess::isPressed(InputSet buttonIndex)
{
	if (!actual) return false;

	return actual->getPad().isPressed(buttonIndex);
}

bool InputsAccess::isJustPressed(InputSet buttonIndex)
{
	if (!actual) return false;

	return actual->getPad().isJustPressed(buttonIndex);
}

bool InputsAccess::isJustReleased(InputSet buttonIndex)
{
	if (!actual) return false;

	return actual->getPad().isJustReleased(buttonIndex);
}

uint32_t InputsAccess::getCurrentButtonsForPlayer(int playerId)
{
	if (!actual) return 0;

	if (playerId == 0)
	{
		return actual->getPad().getPressedButtons();
	}
	else return 0;
	//TODO: Support for more players!
}

bool InputsAccessConfigurable::isListeningForBinding() const
{
	if (!actual)return false;

	return actual->isListeningForBinding();
}

InputsAccess InputsAccessConfigurable::accessDowngrade()
{
	return InputsAccess(actual);
}

void InputsAccessConfigurable::listenForBinding(int bindingIndex)
{
	if (actual)
		actual->listenForBinding(bindingIndex);
}

void InputsAccessConfigurable::stopListeningForBinding()
{
	if (actual)
		actual->stopListeningForBinding();
}

InputBinding InputsAccess::getBinding(int bindingIndex) const
{
	if (!actual) return InputBinding();

	return actual->getBinding(bindingIndex);
}

std::string InputsAccess::getBindingString(int bindingIndex) const
{
	if (!actual)return "(err: InputsAccess->actual==nullptr)";

	return actual->getBindingString(bindingIndex);
}

int InputsAccess::getMaxBindings() const
{
	if (!actual)return 0;

	return actual->getMaxBindings();
}

InputBinding* InputsAccessConfigurable::fetchBinding(int bindingIndex)
{
	if (!actual)return nullptr;

	return actual->fetchBinding(bindingIndex);
}

InputBinding* InputsAccessConfigurable::fetchFallbackBinding(int bindingIndex)
{
	if (!actual)return nullptr;

	return actual->fetchFallbackBinding(bindingIndex);
}

void InputsAccessConfigurable::loadFromIniFile(IniFile& iniFile)
{
	if (actual)
		actual->loadFromIniFile(iniFile);
}

void InputsAccessConfigurable::saveToIniFile(IniFile& iniFile)
{
	if (actual)
		actual->saveToIniFile(iniFile);
}

// CurrentSceneAccess

std::string CurrentSceneAccess::getSceneClassID() const
{
	if (!currentScene)return "";

	return currentScene->getSceneClassID();
}

// LevelAccess

bool LevelAccess::valid() const
{
	if (!actual)return false;

	return actual->getTilesetMeta().isValid();
}

void LevelAccess::rectToBlockSpan(
	const SDL_FRect& r,
	int& col0, int& col1,
	int& row0, int& row1) const
{
	if(actual)
		actual->rectToBlockSpan(r, col0, col1, row0, row1);
}

Level::Block* LevelAccess::getBlockAt(Level::Block::Coords coords)
{
	if (!actual)return nullptr;

	return actual->getBlockAt(coords);
}

const Level::Block* LevelAccess::getConstBlockAt(
	Level::Block::Coords coords) const
{
	if (!actual)return nullptr;

	return actual->getBlockAt(coords);
}

Level::Block* LevelAccess::getBlockAt(SDL_FPoint point)
{
	if (!actual)return nullptr;

	return actual->getBlockAt(point);
}

const Level::Block* LevelAccess::getConstBlockAt(SDL_FPoint point) const
{
	if (!actual)return nullptr;

	return actual->getConstBlockAt(point);
}

SDL_Rect LevelAccess::calculateLevelSize() const
{
	if (!actual)return SDL_Rect{ 0,0,0,0 };

	return actual->calculateLevelSize();
}

Level::BlockQuery LevelAccess::queryBlock(int col, int row) const
{
	if (!actual)return Level::BlockQuery();

	return actual->queryBlock(col, row);
}

Level::BlockQuery LevelAccess::queryBlock(SDL_FPoint positionInLevel) const
{
	if (!actual)return Level::BlockQuery();

	return actual->queryBlock(positionInLevel);
}

std::vector<Level::BlockQuery> LevelAccess::queryBlocksUnderCollider(
	SDL_FRect collider) const
{
	if (!actual)return std::vector<Level::BlockQuery>();

	return actual->queryBlocksUnderCollider(collider);
}

Level::SweepHit LevelAccess::sweepHorizontal(
	const SDL_FRect& rect,
	float dx) const
{
	if (!actual)return Level::SweepHit();

	return actual->sweepHorizontal(rect, dx);
}

Level::SweepHit LevelAccess::sweepVertical(
	const SDL_FRect& rect,
	float dy) const
{
	if (!actual)return Level::SweepHit();

	return actual->sweepVertical(rect, dy);
}

bool LevelAccess::rectInWater(const SDL_FRect& r) const
{
	if (!actual)return false;

	return actual->rectInWater(r);
}

// EntitiesAccess

EntityReference EntitiesAccess::addEntity(std::string entityID)
{
	if (!actual) return EntityReference();

	return actual->addEntity(
		gameEntities.createEntity(PassKey<EntitiesAccess>(), entityID)
	);
}

// SpritesAccess

const Sprite::Definition* SpritesAccess::fetchDefinition(std::string sprdefId)
{
	return actual.fetchDefinition(sprdefId);
}

std::unique_ptr<Sprite> SpritesAccess::create(std::string sprdefId)
{
	auto sprdef = fetchDefinition(sprdefId);
	if (sprdef)
		return std::make_unique<Sprite>(*sprdef);
	else return nullptr;
}

// MenusAccess

bool MenusAccess::isOpen() const
{
	if (!actual)return false;

	return actual->isOpen();
}

void MenusAccess::setMenu(MenuHeader& menuHeader)
{
	setMenu(&menuHeader);
}

void MenusAccess::setMenu(MenuHeader* menuHeader)
{
	if (actual)
		actual->setMenu(menuHeader);
}

void MenusAccess::abruptMenu(MenuHeader& menuHeader)
{
	abruptMenu(&menuHeader);
}

void MenusAccess::abruptMenu(MenuHeader* menuHeader)
{
	if (!actual) return;

	auto currentMenu = actual->currentMenu;

	// Is our abrupt menu currently displaying
	if (currentMenu != menuHeader)
	{ // If it isn't...
		// Is there another menu?
		if (currentMenu == nullptr) // No?
		{ // Then we set ours
			actual->setMenu(menuHeader);
		}
		else
		{ // Then we make ours into a submenu
			actual->subMenu(menuHeader);
		}
	}
}

void MenusAccess::close()
{
	actual->close();
}

// GameWorldAccess

void GameWorldAccess::reportHeroDeadth()
{
	if (currentGameWorld)
		currentGameWorld->reportHeroDeadth();
}

MenuCommandEx GameAccess::onHavingBackedOutOfMenus(PassKey<MenuManager> pk, MenuContext& context)
{
	return actual.onHavingBackedOutOfMenus(PassKey<GameAccess>(), context);
}

void GameAccess::saveSettings()
{
	actual.saveSettings();
}

void GameAccess::declareVictory()
{
    actual.declareVictory();
}
