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

void SuperShiny::init(ssge::StepContext& context)
{
	SDL_Renderer* renderer = context.drawing.getRenderer();
	sprites.load("Shiny", renderer);
	sprites.load("Orb", renderer);
	context.scenes.changeScene("SplashScreen");
}

void SuperShiny::step(ssge::StepContext& context)
{
}

void SuperShiny::draw(ssge::DrawContext& context)
{
}

void SuperShiny::cleanUp(ssge::PassKey<ssge::Engine> pk)
{
	sprites.unload("Shiny");
	sprites.unload("Orb");
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

	// TODO: Port this:
	if (context.currentScene.getSceneClassID()=="TitleScreen") // FIXME: HARDCODED!
	{ // If this is the main menu, ask the player do they wanna exit program
	  
		//Breakenzi style:
		//previousMenus.push(currentMenu);
	    //setMenu(Breakenzi::ConfirmExitProgram);

		// TODO: Implement the menus first
		//cmdEx.smallCmd = ssge::MenuCommand::GOTO_MENU;
		//cmdEx.targetMenu = nullptr; // TODO: IMPLEMENT ME!
		// 
		// Until it's implemented, we stick to this
		cmdEx.smallCmd = ssge::MenuCommand::EXIT_PROGRAM;
	}
	else
	{ // Otherwise just close the menu
	    //closeMenu = true;
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
	else return nullptr;
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
