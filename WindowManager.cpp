#include "WindowManager.h"

ssge::WindowManager::WindowManager()
{
    window = nullptr;
    windowSurface = nullptr;
    renderer = nullptr;
}

ssge::WindowManager::~WindowManager()
{
    shutdown();
}

const char* ssge::WindowManager::init(const char* title, int width, int height)
{
    // Don't re-create the window!
    if (window)
        return "Window already created!";

    // Create window
    window = SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        width, height,
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN
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

SDL_Window* ssge::WindowManager::getWindow() const
{
    return window;
}

SDL_Surface* ssge::WindowManager::getWindowSurface() const
{
    return windowSurface;
}

SDL_Renderer* ssge::WindowManager::getRenderer() const
{
    return renderer;
}

SDL_Rect ssge::WindowManager::makeBestFitScale()
{
    int virtualWidth = ssge::Game::VIRTUAL_WIDTH;
    int virtualHeight = ssge::Game::VIRTUAL_HEIGHT;
    
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

void ssge::WindowManager::updateWindow()
{
    SDL_UpdateWindowSurface(window);
}

void ssge::WindowManager::shutdown()
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
