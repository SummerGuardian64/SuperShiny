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

		static const int MAX_BINDINGS = 32;
		InputBinding bindings[MAX_BINDINGS];
		uint32_t directInputs = 0;
		InputPad pad;

		int listeningFor = -1;
		InputBinding lastBinding;
	public:
		const InputPad& getPad();
		bool isListeningForBinding() const;
		void listenForBinding(int bindingIndex);
		void stopListeningForBinding();
		InputBinding getBinding(int bindingIndex) const;
		std::string getBindingString(int bindingIndex) const;
		int getMaxBindings() const;
	};
}