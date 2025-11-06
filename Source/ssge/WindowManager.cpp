#include "WindowManager.h"
#include "PassKey.h"

using namespace ssge;

WindowManager::WindowManager(PassKey<Engine> pk)
{
    window = nullptr;
    windowSurface = nullptr;
    renderer = nullptr;
}

WindowManager::~WindowManager()
{
    shutdown();
}

const char* WindowManager::init(const char* title, int width, int height)
{
    // Don't re-create the window!
    if (window)
        return "Window already created!";

    virtualWidth = width;
    virtualHeight = height;

    // Create window
    window = SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        width, height,
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
    );

    // See if window was created
    if (!window)
    {
        // Return error message
        return SDL_GetError();
    }

    // Create renderer for the window
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (!renderer)
    {
        return SDL_GetError();
    }

    // Allow semi-transparency
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    // No error
    return nullptr;
}

SDL_Window* WindowManager::getWindow() const
{
    return window;
}

SDL_Surface* WindowManager::getWindowSurface() const
{
    return windowSurface;
}

SDL_Renderer* WindowManager::getRenderer() const
{
    return renderer;
}

SDL_Rect WindowManager::makeBestFitScale()
{
    int windowWidth;
    int windowHeight;
    SDL_GetWindowSize(getWindow(), &windowWidth, &windowHeight);

    float scaleX = (float)windowWidth / virtualWidth;
    float scaleY = (float)windowHeight / virtualHeight;
    float scale = (scaleX < scaleY) ? scaleX : scaleY;

    int dstW = (int)(virtualWidth * scale);
    int dstH = (int)(virtualHeight * scale);
    int dstX = (windowWidth - dstW) / 2;
    int dstY = (windowHeight - dstH) / 2;
    return SDL_Rect{dstX, dstY, dstW, dstH};
}

void WindowManager::updateWindow()
{
    SDL_UpdateWindowSurface(window);
}

void WindowManager::shutdown()
{
    if (renderer)
    {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }
    if (window)
    {
        SDL_DestroyWindow(window);
        window = nullptr;
    }
}
