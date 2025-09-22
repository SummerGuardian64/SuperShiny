#pragma once
#include "SuperShiny.h"

namespace ssge
{
	class Engine // The game engine core
	{
		friend class Program;
	private:
		Program& program;

		Engine(Program& program);
		Engine(const Engine& toCopy) = delete;
		Engine(Engine&& toMove) = delete;
		~Engine();

	public:
		// Initializes the engine
		bool init();
		// Loads all necessary resources without which the Program cannot work.
		// Returns true on success, false on failure.
		bool loadInitialResources();
		// Prepares the initial state for the first main loop iteration.
		// Returns true on success, false on failure.
		bool prepareInitialState();
		// Handles event
		void handle(SDL_Event e);
		// Updates the engine
		bool update(double deltaTime);
		// Lets the engine draw
		void render(SDL_Renderer* renderer);
		// Shut down the engine
		void shutdown();

		ssge::SceneManager* getSceneManager();
		ssge::InputManager* getInputManager();

	private:
		// Manages scenes
		SceneManager* scenes;
		// Manages inputs
		InputManager* inputs;
		// Manages resources
		//ResourceManager resources;
	};
}