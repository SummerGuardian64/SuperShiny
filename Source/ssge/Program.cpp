#include "Program.h"
#include "IGame.h"
#include "Engine.h"
#include "PassKey.h"
#include <memory>

using namespace ssge;

int ssge::Program::run(IGame& game, int argc, char* argv[])
{
    // Prevent running the program twice
    static bool alreadyRunning = false;
    if (alreadyRunning)return -1;

    // Create engine with the game
    auto engine = std::make_unique<Engine>(PassKey<Program>(), game);

    // Initialize the engine
    if (engine->init(PassKey<Program>()))
    { // Run the Engine's main loop now
        engine->mainLoop(PassKey<Program>());
        return 0; // Return success code
    }
    else
    { // Engine init failed
        return -1; // Return failure code
    }
}