#pragma once
#include "SuperShiny.h"

namespace ssge
{
	class InputManager // Manages Inputs
	{
		friend class Program;
	private:
		InputManager();
		InputManager(const InputManager& toCopy) = delete;
		InputManager(InputManager&& toMove) = delete;
		~InputManager();

		void handleEvents();
		void latch();
	};
}