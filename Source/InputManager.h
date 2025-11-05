#pragma once
#include "InputBinding.h"
#include "InputPad.h"
#include "PassKey.h"

namespace ssge
{
	class Engine;

	class InputManager // Manages Inputs
	{
	public:
		InputManager(PassKey<Engine> pk);
		InputManager(const InputManager& toCopy) = delete;
		InputManager(InputManager&& toMove) = delete;

		void handle(SDL_Event e);
		void latch();

		InputBinding bindings[32];
		uint32_t directInputs = 0;
		InputPad pad;

	public:
		const InputPad& getPad();
	};
}