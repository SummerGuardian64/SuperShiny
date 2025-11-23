#pragma once
#include "InputBinding.h"
#include "InputPad.h"
#include "PassKey.h"
#include <list>
#include "IniFile.h"

namespace ssge
{
	class Engine;

	class InputManager // Manages Inputs
	{
	public:
		InputManager(PassKey<Engine> pk);
		InputManager(const InputManager& toCopy) = delete;
		InputManager(InputManager&& toMove) = delete;

		void init(PassKey<Engine> pk);
	private:
		bool handleHardwareChange(const SDL_Event& e);
		bool handleListeningForBinding(const SDL_Event& e);
		bool handleBindings(const SDL_Event& e, InputBinding* chosenBindings);
		void mouseWheelFix(InputBinding* chosenBindings);
	public:
		void handle(SDL_Event e);
		void latch();

	private:
		struct Joypad
		{
			SDL_GameController* ctrl = nullptr;
			SDL_Joystick* joy = nullptr;
			SDL_JoystickID instance = -1;
		};

		static const int MAX_JOYPADS = 8;
		Joypad joypadSlots[MAX_JOYPADS];

		void prepareJoypadSlots(); // Call only on start
	public:
		void onControllerAdded(int deviceIndex);      // SDL_CONTROLLERDEVICEADDED
		void onControllerRemoved(SDL_JoystickID id);  // SDL_CONTROLLERDEVICEREMOVED
		void onJoystickAdded(int deviceIndex);        // SDL_JOYDEVICEADDED
		void onJoystickRemoved(SDL_JoystickID id);    // SDL_JOYDEVICEREMOVED

		int getJoypadSlotByInstance(SDL_JoystickID id) const; // returns slot or -1
		Joypad* getJoypadByInstance(SDL_JoystickID id);
		int getFreeJoypadSlot() const;                    // returns slot or -1

	private:
		static const int MAX_BINDINGS = 32;
		InputBinding bindings[MAX_BINDINGS];
		InputBinding fallbackBindings[MAX_BINDINGS]; // For the con

		uint32_t directInputs = 0;
		InputPad pad;

		int listeningFor = -1;
		InputBinding lastBinding;
	public:
		const InputPad& getPad() const;
		bool isListeningForBinding() const;
		void listenForBinding(int bindingIndex);
		void stopListeningForBinding();
		InputBinding getBinding(int bindingIndex) const;
		InputBinding* fetchBinding(int bindingIndex);
		InputBinding* fetchFallbackBinding(int bindingIndex);
		std::string getBindingString(int bindingIndex) const;
		int getMaxBindings() const;

		void loadFromIniFile(IniFile& iniFile);
		void saveToIniFile(IniFile& iniFile);
	};
}