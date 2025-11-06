#pragma once
#include "SDL.h"
#include "PassKey.h"

namespace ssge
{
	class Engine;

	class WindowManager // Manages the program window
	{
		SDL_Window* window;
		SDL_Surface* windowSurface;
		SDL_Renderer* renderer;
		int virtualWidth;
		int virtualHeight;
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
		// Makes a SDL_Rect with best scaling
		SDL_Rect makeBestFitScale();
		// Updates the window
		void updateWindow();
		// Shuts down the window
		void shutdown();
	};
}