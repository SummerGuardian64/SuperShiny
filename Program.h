#pragma once
#include "SuperShiny.h"

namespace ssge
{
	class Program // Super Shiny Game Engine Program class
	{
	public:
		// Call this only in the main function of the whole C++ project, pass the arguments and return the return value.
		static int run(int argc, char* argv[]);
	private:
		// The Program object may only be created within the static run function
		Program();
		// The Program object may not be copied
		Program(const Program& toCopy) = delete;
		// The Program object may not be moved
		Program(Program&& toMove) = delete;
		// Do not call the destructor! The Program object will get deleted as it goes out of scope of the static run function
		~Program();

		// The effective main function
		int _run(int argc, char* argv[]);


		// Program flow
	private:
		// Initializes the program's managers.
		// Returns true on success, false on failure.
		bool init();
		// Loads all necessary resources without which the Program cannot work.
		// Returns true on success, false on failure.
		bool loadInitialResources();
		// Prepares the initial state for the first main loop iteration.
		// Returns true on success, false on failure.
		bool prepareInitialState();
		// Runs an iteration of the main loop.
		// Returns true if the loop should be continued. Otherwise false.
		bool mainLoop();
		// Cleans up all resources.
		// Returns true on success, false on failure.
		bool cleanup();
		// Shuts down the Program
		// Returns true on success, false on failure.
		bool shutdown();


		// Engine flow
	private:
		// Handles input events
		void handleEvents();
		// Latches inputs
		void latchInputs();
		// Steps through all scenes via SceneManager
		void stepScenes();
		// Draws all scenes via SceneManager
		void drawScenes();


		// Engine systems
	private:
		// Manages the program window
		WindowManager* window;
		// Manages scenes
		SceneManager* scenes;
		// Manages inputs
		InputManager* inputs;
		// Manages resources
		//ResourceManager resources;


		// Program control
	private:
		// Has the user requested an exit from the main loop?
		bool exitRequested;
	public:
		// Returns whether exit from the main loop was requested
		bool wasExitRequested() const;
		// Request exit from the main loop
		void requestExit();


		// Application-specific details
	private:
		static const char APPLICATION_TITLE[];
	public:
		const char* getApplicationTitle();
	};
}