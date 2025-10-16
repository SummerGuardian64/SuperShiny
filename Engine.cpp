#include "Engine.h"
#include "SceneManager.h"
#include "InputManager.h"
#include <memory>
#include "StepContext.h"
#include "DrawContext.h"
#include "Game.h"
#include "SDL.h"
#include "PassKey.h"
#include "WindowManager.h"
#include "Accessor.h"
#include "SplashScreen.h"

using namespace ssge;

Engine::Engine(PassKey<Program> pk)
{
	window = new WindowManager(PassKey<Engine>());
	scenes = new SceneManager(PassKey<Engine>());
	inputs = new InputManager(PassKey<Engine>());
}

Engine::~Engine()
{
	shutdown();
}

bool Engine::init()
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

	//TODO: Better error handling
	loadInitialResources(window->getRenderer());

	if (!prepareInitialState()) return false;
	
	return true;
}

bool Engine::loadInitialResources(SDL_Renderer* renderer)
{
	Game::init(renderer);
	return true;
}

bool Engine::prepareInitialState()
{
	//TODO: Initialize first scene
	//scenes->changeScene(std::make_unique<GameWorld>());
	scenes->changeScene(std::make_unique<SplashScreen>());
	inputs->bindings[0].bindToKey(SDL_Scancode::SDL_SCANCODE_UP);
	inputs->bindings[1].bindToKey(SDL_Scancode::SDL_SCANCODE_DOWN);
	inputs->bindings[2].bindToKey(SDL_Scancode::SDL_SCANCODE_LEFT);
	inputs->bindings[3].bindToKey(SDL_Scancode::SDL_SCANCODE_RIGHT);
	inputs->bindings[4].bindToKey(SDL_Scancode::SDL_SCANCODE_RETURN);

	return true;
}

bool Engine::mainLoop()
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
		ScenesAccess(scenes),
		InputsAccess(inputs),
		DrawingAccess(getWindowManager()->getRenderer())
	);

	scenes->step(stepContext);

	// Let this be the final update if we're finished
	return !wannaFinish;
}

void Engine::render(DrawContext context)
{
	scenes->draw(context);
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

	IMG_Quit();
	SDL_Quit();
}

SceneManager* Engine::getSceneManager()
{
	return scenes;
}

InputManager* Engine::getInputManager()
{
	return inputs;
}

WindowManager* Engine::getWindowManager()
{
	return window;
}

void Engine::finish()
{
	wannaFinish = true;
}

void Engine::wrapUp()
{
	wannaWrapUp = true;
}
