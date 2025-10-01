#pragma once
#include "SDL.h"

namespace ssge
{
	class InputBinding
	{
	public:
		enum class DeviceType
		{
			Disconnected,
			Keyboard,
			MouseButton,
			MouseWheel,
			JoystickButton,
			JoystickAxis,
			JoystickHat,
			TouchFinger
		};

		//TODO: Proper mouse wheel support
		//TODO: Proper axis threshold support
		//TODO: Proper touch support

	private:
		DeviceType deviceType;

		union {
			SDL_Scancode key;
			Uint8 mouseButton; // SDL_BUTTON_LEFT, etc.
			struct { int axis; int direction; } joystickAxis;
			Uint8 joystickButton;
			Uint8 joystickHat;
			SDL_FingerID finger; // touchscreen finger
		} boundTo;

		int deviceInstanceID = -1; // SDL joystick/touch device index

	public:
		void disconnect()
		{
			deviceType = DeviceType::Disconnected;
		}

		void bindToKey(SDL_Scancode key)
		{
			deviceType = DeviceType::Keyboard;
			boundTo.key = key;
		}

		void bindToMouse(Uint8 button)
		{
			deviceType = DeviceType::MouseButton;
			boundTo.mouseButton = button;
		}
		
		void bindToJoystickButton(int joystickID, Uint8 button)
		{
			deviceType = DeviceType::JoystickButton;
			deviceInstanceID = joystickID;
			boundTo.joystickButton = button;
		}

		void bindToJoystickAxis(int joystickID, int axis, int direction)
		{
			deviceType = DeviceType::JoystickAxis;
			deviceInstanceID = joystickID;
			boundTo.joystickAxis.axis = axis;
			boundTo.joystickAxis.direction = direction; // -1 or 1
		}

		void bindToJoystickHat(int joystickID, Uint8 hat)
		{
			deviceType = DeviceType::JoystickHat;
			deviceInstanceID = joystickID;
			boundTo.joystickHat = hat;
		}

		DeviceType getDeviceType() const
		{
			return deviceType;
		}

		SDL_Scancode getScancode() const
		{
			if (deviceType != DeviceType::Keyboard)
				return SDL_Scancode::SDL_SCANCODE_UNKNOWN;
			else return boundTo.key;
		}

		Uint8 getMouseButton() const
		{
			if (deviceType != DeviceType::MouseButton)
				return -1;
			else return boundTo.mouseButton;
		}

		Uint8 getJoystickID() const
		{
			if (deviceType == DeviceType::JoystickAxis
				|| deviceType == DeviceType::JoystickButton
				|| deviceType == DeviceType::JoystickHat
				) return deviceInstanceID;
			else return -1;
		}

		Uint8 getJoystickAxis() const
		{
			if (deviceType != DeviceType::JoystickAxis)
				return -1;
			else return boundTo.joystickAxis.axis;
		}
		
		Uint8 getJoystickAxisDirection() const
		{
			if (deviceType != DeviceType::JoystickAxis)
				return -1;
			else return boundTo.joystickAxis.direction;
		}
		
		Uint8 getJoystickButton() const
		{
			if (deviceType != DeviceType::JoystickButton)
				return -1;
			else return boundTo.joystickButton;
		}

		Uint8 getJoystickHat() const
		{
			if (deviceType != DeviceType::JoystickHat)
				return -1;
			else return boundTo.joystickHat;
		}

		InputBinding() { disconnect(); }
		explicit InputBinding(SDL_Scancode key) { bindToKey(key); }
		explicit InputBinding(Uint8 mouseButton) { bindToMouse(mouseButton); }
		InputBinding(int joystickID, Uint8 mouseButton) { bindToJoystickButton(joystickID, mouseButton); }
		InputBinding(int joystickID, int axis, int direction) { bindToJoystickAxis(joystickID, axis, direction); }
		InputBinding(int joystickID, int hat) { bindToJoystickHat(joystickID, hat); }

		bool matchesEvent(const SDL_Event& e) const
		{
			switch (deviceType)
			{
			case DeviceType::Keyboard:
				return (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP) &&
				        e.key.keysym.scancode == boundTo.key;
			case DeviceType::MouseButton:
				return (e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_MOUSEBUTTONUP) &&
				        e.button.button == boundTo.mouseButton;
			case DeviceType::JoystickButton:
				return (e.type == SDL_JOYBUTTONDOWN || e.type == SDL_JOYBUTTONUP) &&
				        e.jbutton.which == deviceInstanceID &&
				        e.jbutton.button == boundTo.joystickButton;
			case DeviceType::JoystickAxis:
				if (e.type == SDL_JOYAXISMOTION && e.jaxis.which == deviceInstanceID)
				{
					if (e.jaxis.axis == boundTo.joystickAxis.axis) {
						if (boundTo.joystickAxis.direction < 0) return e.jaxis.value < -8000;
						if (boundTo.joystickAxis.direction > 0) return e.jaxis.value > 8000;
					}
				}
				return false;
			default:
				return false;
			}
		}
	};
}