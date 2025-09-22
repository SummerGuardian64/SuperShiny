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
    engine = nullptr;
    window = new WindowManager();
    exitRequested = false;
}

ssge::Program::~Program()
{
    delete window;
}

int ssge::Program::_run(int argc, char* argv[])
{
    int exitCode = 0;

    if (init())
    {
        if (startEngine())
        {
            while (mainLoop()) { ; }
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
    if (auto error = window->init(ssge::Game::APPLICATION_TITLE, 1280, 720))
    {
        std::cout << error << std::endl;
        success = false;
    }

    return success;
}

bool ssge::Program::startEngine()
{
    engine = new Engine(*this);
    return engine->init();
}

bool ssge::Program::mainLoop()
{
    SDL_Renderer* renderer = window->getRenderer();

    // Create a render target texture for the virtual screen.
    SDL_Texture* gameScreen =
        SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
            Game::VIRTUAL_WIDTH, Game::VIRTUAL_HEIGHT);
    if (!gameScreen) {
        //errorString.Create("SDL_CreateTexture (gameScreen) error: ", SDL_GetError());
        return false;
    }

    bool done = false;
    Uint32 lastTicks = SDL_GetTicks();
    while (!done)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
                done = true;
                break;
            default:
                engine->handle(event);
                break;
            }
        }

        Uint32 currentTicks = SDL_GetTicks();
        float deltaTime = (currentTicks - lastTicks) / 1000.0f;
        lastTicks = currentTicks;

        engine->update(deltaTime);

        // Delay to simulate ~50 FPS.
        SDL_Delay(20);

        // Render the game onto the virtual gameScreen texture.
        SDL_SetRenderTarget(renderer, gameScreen);
        engine->render(renderer);
        SDL_SetRenderTarget(renderer, NULL);

        // Clear the window (black background for letterboxing).
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Render the gameScreen texture scaled to best fit the window.
        SDL_Rect fitRect = window->makeBestFitScale();
        SDL_RenderCopy(renderer, gameScreen, NULL, &fitRect);

        SDL_RenderPresent(renderer);
    }
    SDL_DestroyTexture(gameScreen);

    return false;
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

    if (engine)
        engine->shutdown();

    SDL_Quit();
    return true;
}

bool ssge::Program::wasExitRequested() const
{
    return exitRequested;
}

void ssge::Program::requestExit()
{
    exitRequested = true;
}
