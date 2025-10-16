#include "Program.h"
#include "Engine.h"
#include "PassKey.h"
#include <memory>

using namespace ssge;

int ssge::Program::run(int argc, char* argv[])
{
    static bool alreadyRunning = false;
    if (alreadyRunning)return -1;

    auto engine = std::make_unique<Engine>(PassKey<Program>());
    if (engine->init())
        engine->mainLoop();
    else
        return -1;
}