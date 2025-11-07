#pragma once
#include "SDL.h"
#include "PassKey.h"

namespace ssge
{
	class Engine;

	class WindowManager // Manages the program window
	{
		SDL_Window* window = nullptr;
		SDL_Surface* windowSurface = nullptr;
		SDL_Renderer* renderer = nullptr;
		int virtualWidth = 0;
		int virtualHeight = 0;
		bool integralUpscale = false;
		bool borderedFullScreen = false;
	public:
		WindowManager(PassKey<Engine> pk);
		WindowManager(const WindowManager& toCopy) = delete;
		WindowManager(WindowManager&& toMove) = delete;
		~WindowManager();

		// Initializes the program window with title and size.
		// Returns error string or nullptr if no error.
		const char* init(const char* title, int width, int height);
		// Gets the window
		SDL_Window* getWindow() const;
		// Gets the window surface
		SDL_Surface* getWindowSurface() const;
		// Gets the renderer for the window
		SDL_Renderer* getRenderer() const;
		// Gets virtual width
		int getVirtualWidth() const;
		// Gets virtual height
		int getVirtualHeight() const;
		// Returns true if upscale is integral, false if fractional
		bool isUpscaleIntegral() const;
		// Sets integral upscale, providing false sets fractional upscale
		void setIntegralUpscale(bool integralUpscale = true);
		// Sets bordererd fullscreen or windowed mode
		void setBorderedFullScreen(bool borderedFullScreen = false);
		// Makes a SDL_Rect with best scaling
		SDL_Rect makeBestFitScale() const;
		// Updates the window
		void updateWindow();
		// Shuts down the window
		void shutdown();
	};
}