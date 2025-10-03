#pragma once
#include "InputBinding.h"
#include "InputPad.h"

namespace ssge
{
	class Engine;

	class InputManager // Manages Inputs
	{
		friend class Engine;
		InputManager();
		InputManager(const InputManager& toCopy) = delete;
		InputManager(InputManager&& toMove) = delete;
		~InputManager();

		void handle(SDL_Event e);
		void latch();

		InputBinding bindings[32];
		uint32_t directInputs;
		InputPad pad;

	public:
		const InputPad& getPad();
	};
}