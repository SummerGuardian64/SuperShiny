#include "WindowManager.h"

ssge::WindowManager::WindowManager()
{
    window = nullptr;
    windowSurface = nullptr;
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

    // Obtain window surface
    windowSurface = SDL_GetWindowSurface(window);

    // See if window surface was obtained
    if (!windowSurface)
    {
        // Destroy the window if the surface couldn't be obtained
        SDL_DestroyWindow(window);
        window = nullptr;

        // Return error message
        return SDL_GetError();
    }

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

void ssge::WindowManager::updateWindow()
{
    SDL_UpdateWindowSurface(window);
}

void ssge::WindowManager::shutdown()
{
    if (window)
    {
        SDL_DestroyWindow(window);
        window = nullptr;
    }
}
