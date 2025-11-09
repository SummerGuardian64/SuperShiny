#pragma once
#include "SDL.h"
#include "PassKey.h"
#include <SDL_ttf.h>

namespace ssge
{
	class Program;
	class IGame;
	class WindowManager;
	class AudioManager;
	class SceneManager;
	class InputManager;
	class MenuManager;
	class Scene;
	class DrawContext;

	class Engine // The game engine core
	{
		// Interface to the game's implementation
		IGame& game;
		// Manages the window
		WindowManager* window;
		// Manages audio
		AudioManager* audio;
		// Manages scenes
		SceneManager* scenes;
		// Manages inputs
		InputManager* inputs;
		// Manages resources
		//ResourceManager resources;
		// Manages menus
		MenuManager* menus;

		// fonts for menus (init once)
		TTF_Font* menuFont = nullptr;

		bool wannaFinish = false;
		bool wannaWrapUp = false;
	public:
		Engine(PassKey<Program> pk, IGame& game);
		Engine(const Engine& toCopy) = delete;
		Engine(Engine&& toMove) = delete;
		~Engine();

	public:
		// Initializes the engine
		bool init(PassKey<Program> pk);
	private:
		// Loads all necessary resources without which the Program cannot work.
		// Returns true on success, false on failure.
		bool loadInitialResources(SDL_Renderer* renderer);
		// Prepares the initial state for the first main loop iteration.
		// Returns true on success, false on failure.
		bool prepareInitialState();
	public:
		// Runs an iteration of the main loop.
		// Returns true if the loop should be continued. Otherwise false.
		bool mainLoop(PassKey<Program> pk);
	private:
		// Handles event
		void handleEvents();
		// Updates the engine
		bool update(double deltaTime);
		// Lets the engine draw
		void render(DrawContext context);
		// Shut down the engine
		void shutdown();

	public:
		// Engine control from higher levels
		void finish();
		void wrapUp();
		bool isWrappingUp() const;
	};
}