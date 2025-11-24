#include "Engine.h"
#include <SDL.h>
#include <SDL_ttf.h>
#include <memory>
#include "PassKey.h"
#include "SceneManager.h"
#include "WindowManager.h"
#include "AudioManager.h"
#include "InputManager.h"
#include "MenuSystem.h"
#include "MenuContext.h"
#include "StepContext.h"
#include "Accessor.h"
#include "DrawContext.h"
#include "GameWorld.h"

using namespace ssge;

Engine::Engine(PassKey<Program> pk, IGame& game) : game(game)
{
	window = new WindowManager(PassKey<Engine>());
	audio = new AudioManager(PassKey<Engine>());
	scenes = new SceneManager(PassKey<Engine>());
	inputs = new InputManager(PassKey<Engine>());
	menus = new MenuManager(PassKey<Engine>());
}

Engine::~Engine()
{
	shutdown();
}

bool Engine::init(PassKey<Program> pk)
{
	std::cout << "initSDL" << std::endl;
	bool success = true;
	
	// Handy tweak so joysticks don't fail at the con
	// TODO: May need to be parametrized in the future if it causes problems
	SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS, "1");

	do // Gotophobia
	{
		// Initialize SDL
		if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTS |
			SDL_INIT_AUDIO | SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER |
			SDL_INIT_HAPTIC) != 0)
		{
			std::cout << "SDL_Init error: ", SDL_GetError();
			success = false;
			break;
		}

		// Initialize input stuff via InputManager
		inputs->init(PassKey<Engine>());

		// Initialize SDL_image for PNG support
		int imgFlags = IMG_INIT_PNG;
		if (!(IMG_Init(imgFlags) & imgFlags))
		{
			std::cout << "IMG_Init error: " << IMG_GetError();
			success = false;
			break;
		}

		// Initialize program window using game's parameters
		if (auto error = window->init(
			game.getApplicationTitle(),
			game.getVirtualWidth(),
			game.getVirtualHeight())
			)
		{
			std::cout << "WindowManager init error: " << error << std::endl;
			success = false;
			break;
		}

		// Initialize font
		if (TTF_Init() != 0)
		{
			std::cout << "TTF_Init error: " << TTF_GetError() << std::endl;
			success = false;
			break;
		}

		// Initialize audio
		if (!audio->init(PassKey<Engine>()))
		{
			std::cout << "AudioManager init failed. Continuing without sound..."
				<< std::endl;
		}

		// Load initial resources
		if (!loadInitialResources(window->getRenderer()))
		{
			std::cout << "Loading initial resources failed." << std::endl;
			success = false;
			break;
		}

		// Initialize the actual game
		if (!initGame())
		{
			std::cout << "IGame init failed" << std::endl;
			success = false;
			break;
		}
	} while (0);
	
	return success;
}

bool Engine::loadInitialResources(SDL_Renderer* renderer)
{
	bool success = true;

	// load fonts once (SDL_ttf already initialized by Program)
	menuFont = TTF_OpenFont("Fonts/VCR_OSD_MONO.ttf", 28);
	if (menuFont == nullptr)
	{
		std::cout << TTF_GetError() << std::endl;
		success = false;
	}

	return success;
}

bool Engine::initGame()
{
	auto stepContext = StepContext(PassKey<Engine>(),
		0, // Zero delta-time
		EngineAccess(this),
		GameAccess(game),
		WindowAccess(window),
		AudioAccess(audio),
		ScenesAccess(scenes, game),
		InputsAccessConfigurable(inputs),
		DrawingAccess(window->getRenderer()),
		MenusAccess(menus));

	return game.init(stepContext);
}

bool Engine::mainLoop(PassKey<Program> pk)
{
	SDL_Renderer* renderer = window->getRenderer();

	const int virtualWidth = game.getVirtualWidth();
	const int virtualHeight = game.getVirtualHeight();

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
	SDL_RenderSetLogicalSize(renderer, virtualWidth, virtualHeight);
	SDL_RenderSetIntegerScale(renderer, SDL_FALSE);

	// Fixed timestep
	const double fps = 60.0;
	const Uint32 deltaTimeMS = (Uint32)(1000.0 / fps + 0.5); // 16 or 17 ms
	const Uint32 MAX_STEPS = 5;                        // frameskip cap
	const double deltaTime = (double)deltaTimeMS / 1000.0f;

	Uint32 prevTicks = SDL_GetTicks();
	Uint32 accumulatorMS = 0;

	bool done = false;

	// TODO: For Emscripten:
	//       Place the surrounding code into something like beforeMainLoop
	//       and afterMainLoop.
	while (!done)
	{
		handleEvents();

		// Update time passed
		Uint32 now = SDL_GetTicks();
		Uint32 frameMS = now - prevTicks;
		prevTicks = now;

		// Clamp huge stalls (alt-tab, breakpoint, etc.)
		if (frameMS > 250) frameMS = 250;

		// Accumulate time
		accumulatorMS += frameMS;

		// Prevent spiral of deadth by limiting frameskip to MAX_STEPS
		unsigned steps = 0;
		while (accumulatorMS >= deltaTimeMS && steps < MAX_STEPS)
		{
			// Additional responsiveness
			handleEvents();
			// Update the engine and see if it's done (wants to quit)
			done |= !tick(deltaTime);
			accumulatorMS -= deltaTimeMS;
			steps++;
		}

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);
		render(DrawContext(renderer, virtualWidth, virtualHeight));
		SDL_RenderPresent(renderer);

		// Cooperative yield (keeps XP/old drivers happy)
		if (accumulatorMS < frameMS)
		{
			SDL_Delay(1);
		}
	}

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
			// Game implementation handles quit requests!
			game.queryQuit();
			break;
		// Keyboard events
		case SDL_EventType::SDL_KEYDOWN:
		case SDL_EventType::SDL_KEYUP:
		// Mouse events
		case SDL_EventType::SDL_MOUSEBUTTONDOWN:
		case SDL_EventType::SDL_MOUSEBUTTONUP:
		case SDL_EventType::SDL_MOUSEWHEEL:
		// Joystick events
		case SDL_EventType::SDL_JOYBUTTONDOWN:
		case SDL_EventType::SDL_JOYBUTTONUP:
		case SDL_EventType::SDL_JOYAXISMOTION:
		case SDL_EventType::SDL_JOYBALLMOTION:
		case SDL_EventType::SDL_JOYHATMOTION:
		case SDL_EventType::SDL_JOYDEVICEADDED:
		case SDL_EventType::SDL_JOYDEVICEREMOVED:
		// GameController events
		case SDL_EventType::SDL_CONTROLLERDEVICEADDED:
		case SDL_EventType::SDL_CONTROLLERDEVICEREMOVED:
		case SDL_EventType::SDL_CONTROLLERBUTTONDOWN:
		case SDL_EventType::SDL_CONTROLLERBUTTONUP:
		case SDL_EventType::SDL_CONTROLLERAXISMOTION:
			// InputManager handles all of these
			inputs->handle(event);
			break;
		default:
			break;
		}

		// Game implementation handles things too!

		if (event.type == SDL_EventType::SDL_JOYDEVICEREMOVED
			|| event.type == SDL_EventType::SDL_CONTROLLERDEVICEREMOVED)
		{
			// Game implementation handles joypad unplugging
			game.joypadGotUnplugged();
		}
	}
}

bool Engine::tick(double deltaTime)
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
		WindowAccess(window),
		AudioAccess(audio),
		ScenesAccess(scenes, game),
		InputsAccessConfigurable(inputs),
		DrawingAccess(window->getRenderer()),
		MenusAccess(menus)
	);

	scenes->step(stepContext);

	// Step the game implementation
	game.step(stepContext);

	// Step the menu system via MenuManager

	{ // Prepare MenuContext
		Scene* currentScene = scenes->getCurrentScene();
		GameWorld* gameWorld = GameWorld::tryCast(currentScene);
		Level* level = nullptr;
		if (gameWorld)
		{
			level = gameWorld->level.get();
		}
		MenuContext menuContext(
			PassKey<Engine>(),
			EngineAccess(this),
			GameAccess(game),
			WindowAccess(window),
			ScenesAccess(scenes, game),
			InputsAccessConfigurable(inputs),
			DrawingAccess(window->getRenderer()),
			MenusAccess(menus),
			CurrentSceneAccess(currentScene),
			GameWorldAccess(gameWorld),
			LevelAccess(level)
		);

		menus->step(menuContext);
	}

	// Let this be the final tick if we're finished
	return !wannaFinish;
}

void Engine::render(DrawContext context)
{
	scenes->draw(context);
	
	auto menuDrawingCtx = context.deriveWithFont(menuFont);
	menus->draw(menuDrawingCtx);
}

void Engine::shutdown()
{
	std::cout << "shutdown()" << std::endl;

	// Save settings
	StepContext context(
		PassKey<Engine>(),
		0,
		EngineAccess(this),
		GameAccess(game),
		WindowAccess(window),
		AudioAccess(audio),
		ScenesAccess(scenes, game),
		InputsAccessConfigurable(inputs),
		DrawingAccess(window->getRenderer()),
		MenusAccess(menus)
	);
	game.saveSettings(context);

	// Let the game implementation clean itself up
	game.cleanUp(PassKey<Engine>());

	if (menuFont)
	{ // Close menu font
		TTF_CloseFont(menuFont);
		menuFont = nullptr;
	}

	if (inputs)
	{ // Delete InputManager
		delete inputs;
		inputs = nullptr;
	}

	if (audio)
	{ // Delete AudioManager
		delete audio;
		audio = nullptr;
	}

	if (scenes)
	{ // Delete SceneManager
		delete scenes;
		scenes = nullptr;
	}

	if (window)
	{ // Delete WindowManager
		delete window;
		window = nullptr;
	}

	// Quit SDL_ttf
	TTF_Quit();

	// Quit SDL_image
	IMG_Quit();

	// Quit SDL2
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

bool Engine::isWrappingUp() const
{
	return wannaWrapUp;
}