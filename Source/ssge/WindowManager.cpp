#include "WindowManager.h"
#include "PassKey.h"
#include <cmath>
#include <iostream>

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

#if defined(_WIN32)
    printf("_WIN32_WINNT=0x%04X\n", _WIN32_WINNT);
#if _WIN32_WINNT <= 0x0502
    // Nearest neighbour for scaling
    // Windows XP may be weak otherwise
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "direct3d");
    std::cout << "Windows XP is the best!" << std::endl;
#else
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");
#endif
#endif

    // Create renderer for the window
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (!renderer)
    {
        return SDL_GetError();
    }

    // Make the window stretchable in a letterbox style
    SDL_RenderSetLogicalSize(renderer, virtualWidth, virtualHeight);
    // Set fractional scale
    SDL_RenderSetIntegerScale(renderer, SDL_FALSE);
    // Allow semi-transparency
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    // Debug the renderer
    SDL_RendererInfo info{};
    SDL_GetRendererInfo(renderer, &info);
    std::cout << "Renderer: " << (info.name ? info.name : "unknown")
        << " | flags: 0x" << std::hex << info.flags << std::dec << "\n";


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

int ssge::WindowManager::getVirtualWidth() const
{
    return virtualWidth;
}

int ssge::WindowManager::getVirtualHeight() const
{
    return virtualHeight;
}

bool ssge::WindowManager::isUpscaleIntegral() const
{
    return integralUpscale;
}

void ssge::WindowManager::setIntegralUpscale(bool integralUpscale)
{
    // If things are not the way we're setting them
    if (this->integralUpscale != integralUpscale)
    { // Then we set them, duh
        SDL_RenderSetIntegerScale(renderer, integralUpscale ? SDL_TRUE : SDL_FALSE);
        this->integralUpscale = integralUpscale;

        if (integralUpscale)
        { // Prevent shrinking the window below 1x!
            SDL_SetWindowMinimumSize(window, virtualWidth, virtualHeight);
        }
        else
        {
            SDL_SetWindowMinimumSize(window, 1, 1);
        }
    }
}

void ssge::WindowManager::setBorderedFullScreen(bool borderedFullScreen)
{
    // If things are not the way we're setting them
    if (this->borderedFullScreen != borderedFullScreen)
    { // Then we set them, duh
        if (borderedFullScreen)
        { // Bordered Fullscreen
            SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
        }
        else
        { // Windowed
            SDL_SetWindowFullscreen(window, 0);
            SDL_SetWindowBordered(window, SDL_TRUE);
            SDL_RestoreWindow(window);
        }
        // Remember the current setting
        this->borderedFullScreen = borderedFullScreen;
    }
}

SDL_Rect WindowManager::makeBestFitScale() const
{
    int windowWidth;
    int windowHeight;
    SDL_GetWindowSize(getWindow(), &windowWidth, &windowHeight);

    float scaleX = (float)windowWidth / virtualWidth;
    float scaleY = (float)windowHeight / virtualHeight;
    float scale = (scaleX < scaleY) ? scaleX : scaleY;

    if (integralUpscale)
    {
        // Force integer scale, but never below 1
        int intScale = (int)std::floor(scale + 1e-6f);
        if (intScale < 1) intScale = 1;
        scale = (float)intScale;
    }

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
