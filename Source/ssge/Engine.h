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

	class Engine // Super Shiny Game Engine core class
	{
		// CLARIFICATIONS: Memory management
		// All pointers are managed by the engine
		// Null-checks are not performed where it is absolutely assumed that
		// the engine has been successfully initialized!
		// The initialization should fail if any of these Managers could not
		// have been initialized for whatever reason.
		// Likewise, the shutdown() function makes sure all Engine objects are
		// deleted in proper order.
		// This is why such strict barebones pointer approach is absolutely
		// necessary.

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
		//ResourceManager resources; // TODO: After major CRTP overhaul (v0.2)
		// Manages menus
		MenuManager* menus;

		// Fonts for menus
		// TODO: Font management during major CRTP overhaul (v0.2)
		TTF_Font* menuFont = nullptr;

		// Tells the Engine it's time to shut down
		// Set by finish()
		// TODO: Mailboxes/events
		bool wannaFinish = false;
		// Tells the Engine to gracefully shut down (e.g. fade out)
		// Set by wrapUp()
		// TODO: Mailboxes/events
		bool wannaWrapUp = false;
	public:
		// Only Program is allowed to create Engine,
		// and it must bring the concrete implementation of the game
		Engine(PassKey<Program> pk, IGame& game);
		Engine(const Engine& toCopy) = delete; // No Engine copies!
		Engine(Engine&& toMove) = delete; // Engine stays put!
		~Engine(); // Program destroys Engine. Dtor calls shutdown() basically

	public:
		// Initializes the engine
		// Only Program is allowed to call this!
		bool init(PassKey<Program> pk);
	private:
		// Loads all necessary resources without which the Engine cannot work.
		// Returns true on success, false on failure.
		bool loadInitialResources(SDL_Renderer* renderer);
		// Prepares a proper context for game implementation initialization.
		// Returns true on success, false on failure.
		bool initGame();
	public:
		// Runs an iteration of the main loop.
		// Returns true if the loop should be continued. Otherwise false.
		// Only Program is allowed to call this!
		bool mainLoop(PassKey<Program> pk);
	private:
		// Handles event
		void handleEvents();
		// Ticks the engine. This is where step functions are called.
		bool tick(double deltaTime);
		// Lets the engine draw
		void render(DrawContext context);
		// Shut down the engine
		void shutdown();


		///////////////////////////////////////
		// Engine control from higher levels //
		///////////////////////////////////////
	public:
		// Tells the Engine it's time to shut down 
		void finish();
		// Tells the Engine to gracefully shut down (e.g. fade out)
		void wrapUp();
		// Tells whether the Engine is performing a graceful shutdown
		bool isWrappingUp() const;
	};
}