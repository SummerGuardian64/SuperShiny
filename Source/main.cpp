// main.cpp (game side)
#include "ssge/Program.h"
//#include "ssge/IGameFlow.h"
//#include "Game/Flow.h" // your concrete Flow

int main(int argc, char** argv)
{
	//Game::Flow flow;        // your policy brain; owns menus, game rules
	return ssge::Program::run(/*flow,*/ argc, argv);
}
