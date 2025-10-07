#include "Program.h"
#include <iostream>
#include "SDL.h"
#include "SDL_image.h"
#include "WindowManager.h"
#include "Game.h"
#include "Factory.h"
#include "Engine.h"
#include "DrawContext.h"

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

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTS | SDL_INIT_AUDIO |
        SDL_INIT_JOYSTICK | SDL_INIT_HAPTIC) != 0)
    {
        std::cout << "SDL_Init error: ", SDL_GetError();
        success = false;
    }

    // Initialize SDL_image for PNG support
    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags))
    {
        std::cout << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError();
        return false;
    }

    // Initialize program window
    else if (auto error = window->init(ssge::Game::APPLICATION_TITLE, 1280, 720))
    {
        std::cout << error << std::endl;
        success = false;
    }

    //TODO: Better error handling
    engine->loadInitialResources(window->getRenderer());

    return success;
}

bool ssge::Program::startEngine()
{
    engine = Factory::ForProgram::engine(*this);
    return engine->init();
}

bool ssge::Program::mainLoop()
{
    SDL_Renderer* renderer = window->getRenderer();

    int virtualWidth = Game::VIRTUAL_WIDTH;
    int virtualHeight = Game::VIRTUAL_HEIGHT;

    // Create a render target texture for the virtual screen.
    SDL_Texture* gameScreen =
        SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
            virtualWidth, virtualHeight);
    if (!gameScreen) {
        std::cout << "SDL_CreateTexture (gameScreen) error: " << SDL_GetError();
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
                engine->wrapUp();
                break;
            default:
                engine->handle(event);
                break;
            }
        }

        Uint32 currentTicks = SDL_GetTicks();
        float deltaTime = (currentTicks - lastTicks) / 1000.0f;
        if (deltaTime >= 1.0f / 60.0f)
        {
            lastTicks = currentTicks;

            // Update the engine and see if it's done (wants to quit)
            done |= !engine->update(deltaTime);

            // Delay to simulate ~60 FPS.
            //SDL_Delay(16);

            // Render the game onto the virtual gameScreen texture.
            SDL_SetRenderTarget(renderer, gameScreen);
            engine->render(DrawContext(renderer));
            SDL_SetRenderTarget(renderer, NULL);

            // Clear the window (black background for letterboxing).
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);

            // Render the gameScreen texture scaled to best fit the window.
            SDL_Rect fitRect = window->makeBestFitScale();
            SDL_RenderCopy(renderer, gameScreen, NULL, &fitRect);

            SDL_RenderPresent(renderer);
        }
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

    if (engine)
        engine->shutdown();

    // Shut down the program window
    window->shutdown();

    IMG_Quit();
    SDL_Quit();
    return true;
}
