#include "Engine.h"
#include "SceneManager.h"
#include "InputManager.h"
#include <memory>
#include "StepContext.h"
#include "DrawContext.h"
#include "Game.h"
#include <SDL.h>
#include "PassKey.h"
#include "MenuManager.h"
#include "WindowManager.h"
#include "Accessor.h"
#include <SDL_ttf.h>
#include "MenuRenderer.h"
#include "GameWorld.h"
#include "../SuperShiny/TitleScreen.h"

using namespace ssge;

Engine::Engine(PassKey<Program> pk, IGame& game) : game(game)
{
	window = new WindowManager(PassKey<Engine>());
	scenes = new SceneManager(PassKey<Engine>());
	inputs = new InputManager(PassKey<Engine>());
	menus = new MenuManager(PassKey<Engine>(), *this);
}

Engine::~Engine()
{
	shutdown();
}

bool Engine::init(PassKey<Program> pk)
{
	std::cout << "initSDL" << std::endl;
	bool success = true;

	// Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTS | SDL_INIT_AUDIO |
		SDL_INIT_JOYSTICK | SDL_INIT_HAPTIC) != 0)
	{
		std::cout << "SDL_Init error: ", SDL_GetError();
		success = false;
	}

	// Initialize SDL_image for PNG support
	int imgFlags = IMG_INIT_PNG;
	if (!(IMG_Init(imgFlags) & imgFlags))
	{
		std::cout << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError();
		return false;
	}

	// Initialize program window
	if (auto error = window->init(Game::APPLICATION_TITLE, 1280, 720))
	{
		std::cout << error << std::endl;
		return false;
	}

	if (TTF_Init() != 0)
	{
		std::cout << TTF_GetError() << std::endl;
	}

	//TODO: Better error handling
	loadInitialResources(window->getRenderer());

	if (!prepareInitialState()) return false;
	
	return true;
}

bool Engine::loadInitialResources(SDL_Renderer* renderer)
{
	Game::init(renderer);

	// load fonts once (SDL_ttf already initialized by Program)
	menuTitle = TTF_OpenFont("Fonts/VCR_OSD_MONO.ttf", 28);
	menuItem = TTF_OpenFont("Fonts/VCR_OSD_MONO.ttf", 22);

	// register your menus here (or in a helper)
	const int mainId = menus->registerMenu(Menu{
		"SUPER SHINY",
		{
			{"Play",    true,true,false, MenuCommand{MenuCommandType::NewGame}},
			{"Levels",  true,true,false, MenuCommand::PushMenu(/*level id*/1)},
			{"Options", true,false,false, MenuCommand::PushMenu(/*opts id*/2)},
			{"Exit",    true,true,false, MenuCommand{MenuCommandType::ExitProgram}}
		}
		});
	const int levelId = menus->registerMenu(Menu{
		"Select Level",
		{
			{"Radical Sunshine",  true,true,false, MenuCommand::GoToLevel(1)},
			{"Extreme Moonlight", true,true,false, MenuCommand::GoToLevel(2)},
			{"Back",              true,true,false, MenuCommand::Pop()}
		}
		});
	const int pauseId = menus->registerMenu(Menu{
		"Paused",
		{
			{"Continue", true,true,false,MenuCommand::Unpause()},
			{"Back to Main Menu", true,true,false,MenuCommand::PushMenu(3)}
		}
		});
	const int areYouSureId = menus->registerMenu(Menu{
		"Are you sure?",
		{
			{"Yes", true,true,false,MenuCommand::GoToTitleScreen()},
			{"No", true,true,false,MenuCommand::Pop()}
		}
		});

	return true;
}

bool Engine::prepareInitialState()
{
	//TODO: Initialize first scene
	auto stepContext = StepContext(PassKey<Engine>(),
		0,
		EngineAccess(this),
		GameAccess(game),
		ScenesAccess(scenes, game),
		InputsAccess(inputs),
		DrawingAccess(window->getRenderer()),
		MenusAccess(menus));
	game.init(stepContext);

	//scenes->changeScene("SplashScreen");
	inputs->bindings[0].bindToKey(SDL_Scancode::SDL_SCANCODE_UP);
	inputs->bindings[1].bindToKey(SDL_Scancode::SDL_SCANCODE_DOWN);
	inputs->bindings[2].bindToKey(SDL_Scancode::SDL_SCANCODE_LEFT);
	inputs->bindings[3].bindToKey(SDL_Scancode::SDL_SCANCODE_RIGHT);
	inputs->bindings[4].bindToKey(SDL_Scancode::SDL_SCANCODE_X);
	inputs->bindings[5].bindToKey(SDL_Scancode::SDL_SCANCODE_Z);
	inputs->bindings[6].bindToKey(SDL_Scancode::SDL_SCANCODE_C);
	inputs->bindings[7].bindToKey(SDL_Scancode::SDL_SCANCODE_ESCAPE);

	return true;
}

bool Engine::mainLoop(PassKey<Program> pk)
{
	SDL_Renderer* renderer = window->getRenderer();

	int virtualWidth = Game::VIRTUAL_WIDTH;
	int virtualHeight = Game::VIRTUAL_HEIGHT;

	// Create a render target texture for the virtual screen.
	SDL_Texture* gameScreen =
		SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
			virtualWidth, virtualHeight);
	if (!gameScreen) {
		std::cout << "SDL_CreateTexture (gameScreen) error: " << SDL_GetError();
		return false;
	}

	bool done = false;
	double fps = 60.0f;
	double frameDuration = 1.0f / fps;
	Uint32 lastTicks = SDL_GetTicks();
	while (!done)
	{
		handleEvents();

		Uint32 currentTicks = SDL_GetTicks();
		float deltaTime = (currentTicks - lastTicks) / 1000.0f;
		if (deltaTime >= frameDuration)
		{
			lastTicks -= frameDuration;

			// Update the engine and see if it's done (wants to quit)
			done |= !update(frameDuration);

			// Render the game onto the virtual gameScreen texture.
			SDL_SetRenderTarget(renderer, gameScreen);
			render(DrawContext(renderer));
			SDL_SetRenderTarget(renderer, NULL);

			// Clear the window (black background for letterboxing).
			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
			SDL_RenderClear(renderer);

			// Render the gameScreen texture scaled to best fit the window.
			SDL_Rect fitRect = window->makeBestFitScale();
			SDL_RenderCopy(renderer, gameScreen, NULL, &fitRect);

			SDL_RenderPresent(renderer);
		}
	}
	SDL_DestroyTexture(gameScreen);

	return false;
}

void Engine::handleEvents()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			wrapUp();
			break;
		case SDL_EventType::SDL_KEYDOWN:
		case SDL_EventType::SDL_KEYUP:
		case SDL_EventType::SDL_JOYBUTTONDOWN:
		case SDL_EventType::SDL_JOYBUTTONUP:
		case SDL_EventType::SDL_JOYAXISMOTION:
		case SDL_EventType::SDL_JOYBALLMOTION:
		case SDL_EventType::SDL_JOYHATMOTION:
		case SDL_EventType::SDL_JOYDEVICEADDED:
		case SDL_EventType::SDL_JOYDEVICEREMOVED:
			inputs->handle(event);
			break;
		default:
			break;
		}
	}
}

bool Engine::update(double deltaTime)
{
	// Stop updating the engine if it's finished
	if (wannaFinish)
		return false;

	// Start wrapping things up. This is a gentle quit request
	if (wannaWrapUp)
		scenes->wrapUp();

	// Latch all inputs the way the NES does it
	inputs->latch();
	
	// Step the scenes via SceneManager
	StepContext stepContext(
		PassKey<Engine>(),
		deltaTime,
		EngineAccess(this),
		GameAccess(game),
		ScenesAccess(scenes, game),
		InputsAccess(inputs),
		DrawingAccess(window->getRenderer()),
		MenusAccess(menus)
	);

	scenes->step(stepContext);

	// map inputs to menu intents (replace with your real buttons)
	const bool openPause = inputs->pad.isJustPressed(7);   // e.g. Esc/Start
	const bool up = inputs->pad.isJustPressed(0);
	const bool down = inputs->pad.isJustPressed(1);
	const bool left = inputs->pad.isJustPressed(2);
	const bool right = inputs->pad.isJustPressed(3);
	const bool accept = inputs->pad.isJustPressed(4);
	const bool back = inputs->pad.isJustPressed(5);

	menus->handleInput(up, down, left, right, accept, back);

	//menus->step(stepContext);

	// Let this be the final update if we're finished
	return !wannaFinish;
}

void Engine::render(DrawContext context)
{
	scenes->draw(context);
	
	if (menus->isOpen())
	{
		// Draw menu over scene (HUD layer)
		// Either give MenuManager a renderer or build a MenuRenderer here:
		MenuRenderer mr(context.getRenderer(), menuTitle, menuItem, {/*colors/sizes*/ });
		if (const Menu* m = menus->current()) {
			mr.draw(*m, { 0, 80 }, context.getBounds().w);
		}
	}
}

void Engine::shutdown()
{
	std::cout << "shutdown()" << std::endl;

	if (window)
	{
		delete window;
		window = nullptr;
	}
	if (scenes)
	{
		delete scenes;
		scenes = nullptr;
	}
	if (inputs)
	{
		delete inputs;
		inputs = nullptr;
	}

	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

void Engine::finish()
{
	wannaFinish = true;
}

void Engine::wrapUp()
{
	wannaWrapUp = true;
}

void Engine::onMenuCommand(const MenuCommand& cmd)
{
	switch (cmd.type) {
	case MenuCommandType::NewGame:
		//FIXME: Decouple me!!!
		scenes->changeScene(std::make_unique<GameWorld>(1));
		menus->close(); // or pop stack if you prefer
		break;
	case MenuCommandType::GoToLevel:
		//FIXME: Decouple me!!!
		scenes->changeScene(std::make_unique<GameWorld>(cmd.param));
		menus->close();
		break;
	case MenuCommandType::Unpause:
		menus->close();
		scenes->unpause();
		break;
	case MenuCommandType::GoToTitleScreen:
		menus->close();
		scenes->changeScene(std::make_unique<TitleScreen>());
		break;
	case MenuCommandType::Pop:
		menus->popMenu();
		break;
	case MenuCommandType::PushMenu:
		menus->pushMenu(cmd.param);
		break;
	case MenuCommandType::ExitProgram:
		this->wrapUp(); // your graceful shutdown
		break;
	case MenuCommandType::Option:
		// apply +/- adjustment to config (volume, controls) deterministically
		// e.g. config.volume = clamp(config.volume + cmd.param, 0, 10);
		break;
	default: break;
	}
}
