#include "SuperShiny.h"
#include "../ssge/Scene.h"
#include "../ssge/StepContext.h"
#include "../ssge/Accessor.h"
#include "../ssge/MenuSystem.h"
#include "../ssge/MenuContext.h"
#include <memory>
#include <vector>
#include <string_view>
#include "SDL.h"

// GAMEDEV: Please include headers of your scenes
#include "SplashScreen.h"
#include "TitleScreen.h"

// GAMEDEV: Please include headers of your entities
#include "Shiny.h"
#include "Orb.h"

using namespace ssge;

SuperShiny SuperShiny::makeGame()
{
	static bool alreadyCreated = false;
	if (alreadyCreated) throw;
	alreadyCreated = true;
	return SuperShiny();
}

SuperShiny::SuperShiny() :
	scenes(ssge::PassKey<SuperShiny>()),
	entities(ssge::PassKey<SuperShiny>()),
	sprites(ssge::PassKey<SuperShiny>())
{
}

void SuperShiny::syncSettings(ssge::StepContext& context) const
{
	context.window.setIntegralUpscale(config.integralUpscale);
	context.window.setBorderedFullScreen(config.fullScreen);
	context.audio.setMasterVolume(config.masterVolume);
	context.audio.setMusicVolume(config.musicVolume);
	context.audio.setSfxVolume(config.musicVolume);
}

void SuperShiny::init(ssge::StepContext& context)
{
	SDL_Renderer* renderer = context.drawing.getRenderer();
	sprites.load("Shiny", renderer);
	sprites.load("Orb", renderer);
	context.scenes.changeScene("SplashScreen");

	// Load settings

	IniFile configIni;

	if (configIni.loadIni("config.ini"))
	{ // Only parse if we have something loaded
		config.load(configIni);
		context.inputs.loadFromIniFile(configIni);
	}

	menus.init(config);
}

void SuperShiny::step(ssge::StepContext& context)
{
	std::string currentScene = context.scenes.getCurrentSceneClassID();

	if (currentScene == scenes.getMainMenuSceneClassID())
	{
		context.audio.playMusicIfNotPlaying("Music/ShinyTheme.ogg");

		if (!context.menus.isOpen()
			&& context.scenes.isFadeFinished()
			&& !context.engine.isWrappingUp())
		{
			context.menus.setMenu(menus.mainMenu);
		}
	}
	else if (currentScene == "GameWorld")
	{
		bool isPaused = context.scenes.isPaused();
		bool fading = !context.scenes.isFadeFinished();
		bool gameplayRunning = !isPaused && !fading;
		bool menuOpen = context.menus.isOpen();

		// If gameplay is running
		if (gameplayRunning)
		{
			// Offer pause button
			if (context.inputs.isJustPressed(7))
			{
				context.scenes.pause(); // Pause the scene
				context.menus.setMenu(menus.pauseMenu); // Set pause menu
			}
		}
		else if (!menuOpen) // If gameplay is not running, but we closed the menu
		{
			// Unpause!
			context.scenes.unpause();
		}
		else if (fading) // If menu is somehow open during fading
		{
			// Close it!!!
			context.menus.close();
		}
	}

	syncSettings(context);
}

void SuperShiny::draw(ssge::DrawContext& context)
{
}

void SuperShiny::cleanUp(ssge::PassKey<ssge::Engine> pk)
{
	sprites.unload("Shiny");
	sprites.unload("Orb");
}

bool SuperShiny::saveSettings(ssge::StepContext& context)
{
	// Save settings

	IniFile configIni;

	config.save(configIni);
	context.inputs.saveToIniFile(configIni);

	if (!configIni.saveIni("config.ini"))
	{
		std::cout << configIni.getErrorLog() << std::endl;
		return false;
	}
	else return true;
}

const char* SuperShiny::getApplicationTitle()
{
	return "Super Shiny";
}

int SuperShiny::getVirtualWidth()
{
	return 1280;
}

int SuperShiny::getVirtualHeight()
{
	return 720;
}

ssge::MenuCommandEx SuperShiny::onHavingBackedOutOfMenus(ssge::PassKey<ssge::GameAccess> pk, ssge::MenuContext& context)
{
	ssge::MenuCommandEx cmdEx;

	std::string currentScene = context.currentScene.getSceneClassID();

	if (currentScene==scenes.getMainMenuSceneClassID())
	{ // If this is the main menu, ask the player do they wanna exit program
		cmdEx.smallCmd = ssge::MenuCommand::SUB_MENU;
		cmdEx.targetMenu = &menus.confirmExitProgram;
	}
	else
	{ // Otherwise just close the menu
		cmdEx.smallCmd = ssge::MenuCommand::CLOSE_MENU;
	}

	return cmdEx;
}

ssge::IGameScenes& SuperShiny::getScenes(ssge::PassKey<ssge::ScenesAccess> pk)
{
	return scenes;
}

ssge::IGameEntities& SuperShiny::getEntities(ssge::PassKey<ssge::EntitiesAccess> pk)
{
	return entities;
}

ssge::IGameSprites& SuperShiny::getSprites()
{
	return sprites;
}

std::unique_ptr<ssge::Scene> SuperShiny::Scenes::createScene(ssge::PassKey<ssge::ScenesAccess> pk, std::string id)
{
	if (id == "SplashScreen")
		return splashScreen();

	else if (id == "TitleScreen")
		return titleScreen();

	else return nullptr;
}

std::string SuperShiny::Scenes::getMainMenuSceneClassID() const
{
	return "TitleScreen";
}

SuperShiny::Scenes::Scenes(ssge::PassKey<SuperShiny> pk) {}

std::unique_ptr<SplashScreen> SuperShiny::Scenes::splashScreen()
{
	return std::make_unique<SplashScreen>();
}

std::unique_ptr<TitleScreen> SuperShiny::Scenes::titleScreen()
{
	return std::make_unique<TitleScreen>();
}

std::shared_ptr<Shiny> SuperShiny::Entities::shiny()
{
	return std::make_shared<Shiny>();
}

std::shared_ptr<Orb> SuperShiny::Entities::orb()
{
	return std::make_shared<Orb>();
}

SuperShiny::Entities::Entities(ssge::PassKey<SuperShiny> pk) {}

std::shared_ptr<ssge::Entity> SuperShiny::Entities::createEntity(ssge::PassKey<ssge::EntitiesAccess> pk, std::string entityId)
{
	if (entityId == "Shiny")
		return shiny();
	
	else if (entityId == "Orb")
		return orb();

	return nullptr;
}

SuperShiny::Sprites::Sprites(ssge::PassKey<SuperShiny> pk)
{
	using Image = ssge::Sprite::Image;
	using Sequence = ssge::Sprite::Animation::Sequence;

	sprdefShiny.spritesheetPath = "Sprites/Shiny.png";

	for (int y = 0; y < 1024 - 102; y += 102)
	{
		for (int x = 0; x <= 1024 - 77; x += 77)
		{
			sprdefShiny.images.push_back(Image(x, y, 77, 102, 41, 96));
		}
	}

	{ // Stopped
		auto& seq = sprdefShiny.addSequence(20, 20, 0);
		seq.imageIndexes.push_back(0);
	}
	{ // Walking
		auto& seq = sprdefShiny.addSequence(20, 10, 0);
		for (int i = 1; i <= 14; i++)
		{
			seq.imageIndexes.push_back(i);
		}
	}
	{ // Jumping
		sprdefShiny.images[15] = Image(154, 102, 57, 92, 30, 98);
		auto& seq = sprdefShiny.addSequence(20, 20, 0);
		seq.imageIndexes.push_back(15);
	}
	{ // Falling
		sprdefShiny.images[16] = Image(212, 102, 48, 104, 25, 104);
		auto& seq = sprdefShiny.addSequence(20, 20, 0);
		seq.imageIndexes.push_back(16);
	}
	{ // Deadth
		sprdefShiny.images[17] = Image(260, 102, 65, 96, 33, 96);
		auto& seq = sprdefShiny.addSequence(20, 20, 0);
		seq.imageIndexes.push_back(17);
	}

	sprdefOrb.spritesheetPath = "Sprites/Orb.png";

	sprdefOrb.images.push_back(Image(0, 0, 64, 64, 32, 32));
	{
		auto& seq = sprdefOrb.addSequence(20, 20, 0);
		seq.imageIndexes.push_back(0);
	}
}

bool SuperShiny::Sprites::load(std::string sprdefId, SDL_Renderer* renderer)
{
	ssge::Sprite::Definition* sprdef = fetchDefinitionNonConst(sprdefId);
	if (sprdef)
		return sprdef->load(renderer);
	else return false;
}

void SuperShiny::Sprites::unload(std::string sprdefId)
{
	ssge::Sprite::Definition* sprdef = fetchDefinitionNonConst(sprdefId);
	if (sprdef)
		sprdef->unload();
}

const ssge::Sprite::Definition* SuperShiny::Sprites::fetchDefinition(const std::string& sprdefId)
{
	return fetchDefinitionNonConst(sprdefId);
}

ssge::Sprite::Definition* SuperShiny::Sprites::fetchDefinitionNonConst(std::string sprdefId)
{
	if (sprdefId == "Shiny")
		return &sprdefShiny;

	if (sprdefId == "Orb")
		return &sprdefOrb;

	return nullptr;
}

void SuperShiny::Menus::init(SuperShiny::Config& config)
{
	// Compose other menus
	optionsMenu.setTitle("Options");
	confirmRestart.setTitle("Caution!");
	confirmExitGame.setTitle("Warning!");
	levelSelect.setTitle("Level Select");
	friendlyReminderReset.setTitle("Friendly reminder");
	friendlyReminderExit.setTitle("Friendly reminder");

	// Create the exit menu header
	confirmExitProgram.setTitle("Exit?");

	// Create the high score menu header
	highScoreMenu.setTitle("High Score");

	// Compose the main menu
	mainMenu.setTitle("Super Shiny");
	mainMenu.newItem_NewGame("Start Game");
	mainMenu.newItem("Load Game")->visible = false;
	mainMenu.newItem_SubMenu("Level Select", &levelSelect)->visible = false;
	mainMenu.newItem("Super Duper Secret Menu")->visible = false;
	mainMenu.newItem_SubMenu("High Score", &highScoreMenu, (MenuFunction)refreshHighScoreMenu);
	mainMenu.newItem_SubMenu("Options", &optionsMenu);
	mainMenu.newItem_SubMenu("Exit", &confirmExitProgram);
	mainMenu.newLabel("Created by Summer Guardian 64");
	mainMenu.newLabel("Thank you for playing!");

	optionsMenu.newItem_SubMenu("Display Settings", &displaySettingsMenu);
	{
		displaySettingsMenu.setTitle("Display Settings");
		displaySettingsMenu.newItem_IntSetting("Resolution Upscale:", &config.resolutionScaleConfig, 1, 4)->visible = false;
		displaySettingsMenu.newItem_BoolSetting("Display Mode:", &config.fullScreen, "Borderless Fullscreen", "Windowed");
		displaySettingsMenu.newItem_BoolSetting("Stretching:", &config.integralUpscale, "Integral", "Fractional");
		displaySettingsMenu.newItem_GoBack("Save & Back"); // TODO: Config saving
	}
	optionsMenu.newItem_SubMenu("Input Configuration", &inputConfigMenu);
	{
		inputConfigMenu.setTitle("Input Configuration");
		inputConfigMenu.newItem_InputBinding("Up", 0);
		inputConfigMenu.newItem_InputBinding("Down", 1);
		inputConfigMenu.newItem_InputBinding("Left", 2);
		inputConfigMenu.newItem_InputBinding("Right", 3);
		inputConfigMenu.newItem_InputBinding("Jump", 4);
		inputConfigMenu.newItem_InputBinding("Run/Shoot/Grab", 5);
		inputConfigMenu.newItem_InputBinding("Dash/Pounce", 6);
		inputConfigMenu.newItem_InputBinding("Pause Game", 7);
		inputConfigMenu.newItem_InputBinding("Menu Accept", 8);
		inputConfigMenu.newItem_InputBinding("Menu Back", 9);
		inputConfigMenu.newItem_GoBack("Save & Back"); // TODO: Config saving
	}
	optionsMenu.newItem_SubMenu("Volume Control", &volumeControlMenu);
	{
		volumeControlMenu.setTitle("Volume Control");
		volumeControlMenu.newItem_IntSetting("Master:", &config.masterVolume, 0, 100);
		volumeControlMenu.newItem_IntSetting("Music:", &config.musicVolume, 0, 100);
		volumeControlMenu.newItem_IntSetting("Sounds:", &config.sfxVolume, 0, 100);
		volumeControlMenu.newItem_GoBack("Save & Back"); // TODO: Config saving
	}
	optionsMenu.newItem_GoBack("Save & Back"); // TODO: Config saving

	confirmRestart.newLabel("Restarting a level will discard unsaved score!");
	confirmRestart.newLabel("Are you sure you want to restart this level?");
	confirmRestart.newItem_RestartLevel("Yes");
	confirmRestart.newItem_GoBack("No");

	confirmExitGame.newLabel("Exiting the game will discard unsaved score!");
	confirmExitGame.newLabel("Are you sure you want to return to the main menu?");
	confirmExitGame.newItem_MainMenu("Yes");
	confirmExitGame.newItem_GoBack("No");

	confirmExitProgram.newLabel("Are you sure you want to exit Super Shiny?");
	confirmExitProgram.newItem_ExitProgram("Yes");
	confirmExitProgram.newItem_GoBack("No");

	levelSelect.newItem_IntSetting("Level", &levelSelectorInt, 0, 10, MenuCommand::GOTO_LEVEL);
	levelSelect.newItem("Start Game")->command = MenuCommand::GOTO_LEVEL;
	levelSelect.newItem_GoBack("Back");

	// Compose the pause menu
	pauseMenu.setTitle("Pause");
	pauseMenu.newItem_CloseMenu("Resume");
	pauseMenu.newItem_SubMenu("Restart Level", &confirmRestart);
	pauseMenu.newItem_SubMenu("High Score", &highScoreMenu, (MenuFunction)&refreshHighScoreMenu)->visible = false;
	pauseMenu.newItem_SubMenu("Options", &optionsMenu);
	pauseMenu.newItem_SubMenu("Go To Main Menu", &confirmExitGame);

	// Compose the game over menu
	gameOverMenu.setTitle("Game Over");
	gameOverMenu.newItem_SubMenu("High Score", &highScoreMenu, (MenuFunction)&refreshHighScoreMenu);
	gameOverMenu.newItem_RestartLevel("Try Again");
	gameOverMenu.newItem_NewGame("Start Over");
	gameOverMenu.newItem_SubMenu("Options", &optionsMenu);
	gameOverMenu.newItem_SubMenu("Go To Main Menu", &confirmExitGame);

	// Compose the level completion menu
	levelCompleteMenu.setTitle("Level Complete!");
	levelCompleteMenu.newItem_NextLevel("Next Level");
	levelCompleteMenu.newItem_SubMenu("High Score", &highScoreMenu, (MenuFunction)&refreshHighScoreMenu);
	levelCompleteMenu.newItem_SubMenu("Options", &optionsMenu);
	levelCompleteMenu.newItem_SubMenu("Go To Main Menu", &confirmExitGame);

	// Compose the victory menu
	victoryMenu.setTitle("Congratulations!");
	victoryMenu.newLabel("You have completed the last level of this demo!");
	victoryMenu.newLabel("Thank you for playing!");
	victoryMenu.newLabel("Don't forget to enter your high score!");
	victoryMenu.newItem_SubMenu("Enter High Score", &highScoreMenu, (MenuFunction)&refreshHighScoreMenu);
	victoryMenu.newItem_SubMenu("Start Over", &friendlyReminderReset);
	victoryMenu.newItem_SubMenu("Go To Main Menu", &friendlyReminderExit);

	friendlyReminderReset.newLabel("You should save your high score before restarting");
	friendlyReminderReset.newItem_NewGame("It's alright. I wanna start over");
	friendlyReminderReset.newItem_GoBack("Oh nose! Take me back!");

	friendlyReminderExit.newLabel("You should save your high score before exiting ;)");
	friendlyReminderExit.newItem_MainMenu("It's alright. Exit, please");
	friendlyReminderExit.newItem_GoBack("Uh oh, you're right. Go back!");
}

void SuperShiny::Menus::refreshHighScoreMenu(int direction)
{
}

bool SuperShiny::Config::load(ssge::IniFile& iniFile)
{
	masterVolume = iniFile.getInt("Config", "MasterVolume", 100);
	sfxVolume = iniFile.getInt("Config", "SfxVolume", 100);
	musicVolume = iniFile.getInt("Config", "MusicVolume", 100);
	resolutionScaleConfig = iniFile.getInt("Config", "ResolutionScaleConfig", 1);
	fullScreen = iniFile.getBool("Config", "FullScreen", false);
	integralUpscale = iniFile.getBool("Config", "IntegralUpscale", false);
	return true;
}

bool SuperShiny::Config::save(ssge::IniFile& iniFile) const
{
	iniFile.setInt("Config", "MasterVolume", masterVolume);
	iniFile.setInt("Config", "SfxVolume", sfxVolume);
	iniFile.setInt("Config", "MusicVolume", musicVolume);
	iniFile.setInt("Config", "ResolutionScaleConfig", resolutionScaleConfig);
	iniFile.setBool("Config", "FullScreen", fullScreen);
	iniFile.setBool("Config", "IntegralUpscale", integralUpscale);
	return false;
}
