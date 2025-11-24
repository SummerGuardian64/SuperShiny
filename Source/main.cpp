#include "ssge/Program.h" // Super Shiny Game Engine
#include "SuperShiny/SuperShiny.h" // Super Shiny game

int main(int argc, char** argv)
{
	// Create Super Shiny implementation object
	auto game = SuperShiny::makeGame();
	// Start program, pass IGame and argc, and return what it returns.
	return ssge::Program::run(game, argc, argv);
}
