#pragma once
#include "SDL.h"
#include "PassKey.h"

namespace ssge
{
	class SceneManager;
	class WindowManager;
	class InputManager;
	class Scene;
	class Program;
	class DrawContext;

	class Engine // The game engine core
	{
	private:
		Program& program;
	public:
		Engine(ssge::PassKey<ssge::Program> pk, Program& program);
		Engine(const Engine& toCopy) = delete;
		Engine(Engine&& toMove) = delete;
		~Engine();

	public:
		// Initializes the engine
		bool init();
		// Loads all necessary resources without which the Program cannot work.
		// Returns true on success, false on failure.
		bool loadInitialResources(SDL_Renderer* renderer);
		// Prepares the initial state for the first main loop iteration.
		// Returns true on success, false on failure.
		bool prepareInitialState();
		// Handles event
		void handle(SDL_Event e);
		// Updates the engine
		bool update(double deltaTime);
		// Lets the engine draw
		void render(DrawContext context);
		// Shut down the engine
		void shutdown();

		ssge::SceneManager* getSceneManager();
		ssge::InputManager* getInputManager();
		ssge::WindowManager* getWindowManager();

		// Engine control from higher levels

		void finish();

		void wrapUp();
	private:
		// Manages scenes
		SceneManager* scenes;
		// Manages inputs
		InputManager* inputs;
		// Manages resources
		//ResourceManager resources;

		bool wannaFinish;
		bool wannaWrapUp;
	};
}