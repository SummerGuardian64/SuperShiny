#pragma once
#include "SDL.h"

namespace ssge
{
	class WindowManager // Manages the program window
	{
		friend class Program;
	private:
		WindowManager();
		WindowManager(const WindowManager& toCopy) = delete;
		WindowManager(WindowManager&& toMove) = delete;
		~WindowManager();

		SDL_Window* window;
		SDL_Surface* windowSurface;
	public:
		// Initializes the program window with title and size.
		// Returns error string or nullptr if no error.
		const char* init(const char* title, int width, int height);
		// Gets the window
		SDL_Window* getWindow() const;
		// Gets the window surface
		SDL_Surface* getWindowSurface() const;
		// Updates the window
		void updateWindow();
		// Shuts down the window
		void shutdown();
	};
}