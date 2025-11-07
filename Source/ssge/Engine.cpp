#include "Engine.h"
#include "SceneManager.h"
#include "InputManager.h"
#include <memory>
#include "StepContext.h"
#include "DrawContext.h"
#include <SDL.h>
#include "PassKey.h"
#include "MenuSystem.h"
#include "WindowManager.h"
#include "Accessor.h"
#include <SDL_ttf.h>
#include "GameWorld.h"
#include "MenuContext.h"

using namespace ssge;

Engine::Engine(PassKey<Program> pk, IGame& game) : game(game)
{
	window = new WindowManager(PassKey<Engine>());
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
	if (auto error = window->init(game.getApplicationTitle(), game.getVirtualWidth(), game.getVirtualHeight()))
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
	//Game::init(renderer);

	// load fonts once (SDL_ttf already initialized by Program)
	menuFont = TTF_OpenFont("Fonts/VCR_OSD_MONO.ttf", 28);
	if (menuFont == nullptr)
	{
		std::cout << TTF_GetError() << std::endl;
	}

	return true;
}

bool Engine::prepareInitialState()
{
	//TODO: Initialize first scene
	auto stepContext = StepContext(PassKey<Engine>(),
		0,
		EngineAccess(this),
		GameAccess(game),
		WindowAccess(window),
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

	const int virtualWidth = game.getVirtualWidth();
	const int virtualHeight = game.getVirtualHeight();

	// Create a render target texture for the virtual screen.
	// TODO: If render targets aren’t supported on some old GPUs, consider the
	// "logical size" approach shown below. For now we keep your target.
	SDL_Texture* gameScreen =
		SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
			virtualWidth, virtualHeight);

	if (!gameScreen)
	{
		std::cout << "SDL_CreateTexture (gameScreen) error: " << SDL_GetError();
		return false;
	}

	// Fixed timestep
	const double fps = 60.0;
	const Uint32 deltaTimeMS = (Uint32)(1000.0 / fps + 0.5); // 16 or 17 ms
	const Uint32 MAX_STEPS = 5;                        // frameskip cap

	Uint32 prevTicks = SDL_GetTicks();
	Uint32 accumulatorMS = 0;

	bool done = false;

	// TODO: After the con, let this be MainLoop to please Emscripten.
	//       Place the surrounding code into something like beforeMainLoop and afterMainLoop.
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
			const double dt = (double)deltaTimeMS / 1000.0f;

			// Update the engine and see if it's done (wants to quit)
			done |= !update(dt);
			accumulatorMS -= deltaTimeMS;
			steps++;
		}

		// Render the game onto the virtual gameScreen texture.
		SDL_SetRenderTarget(renderer, gameScreen);
		render(DrawContext(renderer));
		SDL_SetRenderTarget(renderer, nullptr);

		// Clear the window (black background for letterboxing).
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		// Render the gameScreen texture scaled to best fit the window.
		SDL_Rect fitRect = window->makeBestFitScale();
		SDL_RenderCopy(renderer, gameScreen, nullptr, &fitRect);

		SDL_RenderPresent(renderer);

		// Cooperative yield (keeps XP/old drivers happy)
		SDL_Delay(0);
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
		WindowAccess(window),
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

	//menus->handleInput(up, down, left, right, accept, back);

	MenuContext menuContext(
		PassKey<Engine>(),
		EngineAccess(this),
		GameAccess(game),
		WindowAccess(window),
		ScenesAccess(scenes, game),
		InputsAccess(inputs),
		DrawingAccess(window->getRenderer()),
		MenusAccess(menus),
		CurrentSceneAccess(nullptr),
		GameWorldAccess(nullptr),
		LevelAccess(nullptr)
	);

	game.step(stepContext);

	menus->step(menuContext);

	// Let this be the final update if we're finished
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

	game.cleanUp(PassKey<Engine>());

	if (menuFont)
	{
		TTF_CloseFont(menuFont);
		menuFont = nullptr;
	}

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

bool Engine::isWrappingUp() const
{
	return wannaWrapUp;
}