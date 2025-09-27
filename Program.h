#pragma once
#include "SuperShiny.h"
#include "SDL.h"

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
		// Starts the engine.
		// Returns true on success, false on failure.
		bool startEngine();
		// Runs an iteration of the main loop.
		// Returns true if the loop should be continued. Otherwise false.
		bool mainLoop();
		// Cleans up all resources.
		// Returns true on success, false on failure.
		bool cleanup();
		// Shuts down the Program
		// Returns true on success, false on failure.
		bool shutdown();

		// Program systems
	public:
		// Manages the program window
		WindowManager* window;
		// The game engine core
		Engine* engine;
		
	};
}