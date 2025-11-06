#include "IGame.h"
#include "Program.h"
#include "Engine.h"
#include "PassKey.h"
#include <memory>

using namespace ssge;

int ssge::Program::run(IGame& game, int argc, char* argv[])
{
    static bool alreadyRunning = false;
    if (alreadyRunning)return -1;

    auto engine = std::make_unique<Engine>(PassKey<Program>(), game);
    if (engine->init(PassKey<Program>()))
    {
        engine->mainLoop(PassKey<Program>());
        return 0;
    }
    else
    {
        return -1;
    }
}