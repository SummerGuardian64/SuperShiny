#pragma once

namespace ssge
{
	class IGame;
	class Engine;

	class Program // Super Shiny Game Engine Program class
	{
	public:
		// Call this only in the main function of the whole C++ project, pass the arguments and return the return value.
		static int run(IGame& game, int argc, char* argv[]);

		Program() = delete;
		Program(const Program& toCopy) = delete;
		Program(Program&& toMove) = delete;
	};
}