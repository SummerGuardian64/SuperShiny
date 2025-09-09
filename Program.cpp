#include "Program.h"
#include <iostream>
#include "SDL.h"

using namespace ssge;

int ssge::Program::run(int argc, char* argv[])
{
    static bool alreadyRunning = false;
    if (alreadyRunning)return -1;
    Program theProgram;
    return theProgram._run(argc, argv);
}

ssge::Program::Program()
{
    window = new WindowManager();
    scenes = new SceneManager();
    inputs = new InputManager();
    exitRequested = false;
}

ssge::Program::~Program()
{
    delete window;
    delete scenes;
    delete inputs;
}

int ssge::Program::_run(int argc, char* argv[])
{
    int exitCode = 0;

    if (init()) {
        if (loadInitialResources()) {
            if (prepareInitialState()) {
                while (mainLoop()) {
                    SDL_Delay(16); // TODO: Replace with proper timing system!
                }
            }
            else { exitCode = -1; }
        }
        else { exitCode = -1; }
    }
    else { exitCode = -1; }

    if (!cleanup()) exitCode = -1;
    if (!shutdown()) exitCode = -1;

    return exitCode;
}

bool ssge::Program::init()
{
    std::cout << "initSDL" << std::endl;
    bool success = true;

    // Initialize program window
    if (window->init(getApplicationTitle(), 1280, 720) != nullptr)
        success = false;

    return success;
}

bool ssge::Program::loadInitialResources()
{
    std::cout << "loadInitialResources()" << std::endl;
    return true;
}

bool ssge::Program::prepareInitialState()
{
    std::cout << "prepareInitialState()" << std::endl;
    return true;
}

bool ssge::Program::mainLoop()
{
    std::cout << "mainLoop()" << std::endl;
    handleEvents();
    latchInputs();
    stepScenes();
    drawScenes();
    return !wasExitRequested();
}

bool ssge::Program::cleanup()
{
    std::cout << "cleanup()" << std::endl;
    return true;
}

bool ssge::Program::shutdown()
{
    std::cout << "shutdown()" << std::endl;

    // Shut down the program window
    window->shutdown();
    return true;
}

void ssge::Program::handleEvents()
{
    std::cout << "handleEvents()" << std::endl;

    SDL_Event e;
    if (SDL_PollEvent(&e))
    {
        switch (e.type)
        {
        case SDL_KEYDOWN:
            if (e.key.keysym.sym == SDLK_ESCAPE)
            {
                requestExit();
            }
            break;
        default:
            break;
        }
    }
}

void ssge::Program::latchInputs()
{
    std::cout << "latchInputs()" << std::endl;
}

void ssge::Program::stepScenes()
{
    std::cout << "stepScenes()" << std::endl;
    auto stepContext = StepContext(*this);
    scenes->step(stepContext);
}

void ssge::Program::drawScenes()
{
    std::cout << "drawScenes()" << std::endl;
    auto drawContext = DrawContext(*this);
    scenes->draw(drawContext);
    window->updateWindow();
}

bool ssge::Program::wasExitRequested() const
{
    return exitRequested;
}

void ssge::Program::requestExit()
{
    exitRequested = true;
}

const char ssge::Program::APPLICATION_TITLE[] = "Your game name here";

const char* ssge::Program::getApplicationTitle()
{
    return APPLICATION_TITLE;
}
