// main.cpp (game side)
#include "ssge/Program.h"
#include "SuperShiny/SuperShiny.h"

int main(int argc, char** argv)
{
	auto game = SuperShiny::makeGame();
	return ssge::Program::run(game, argc, argv);
}
