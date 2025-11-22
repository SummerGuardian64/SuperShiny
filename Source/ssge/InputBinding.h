#pragma once
#include "SDL.h"
#include <string>

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
			GameControllerButton,
			GameControllerAxis,
			TouchFinger,
			TOTAL
		};

		//TODO: Proper axis threshold support
		//TODO: Proper touch support

	private:
		DeviceType deviceType;

		union {
			SDL_Scancode key;
			Uint8 mouseButton; // SDL_BUTTON_LEFT, etc.
			struct { int direction; } mouseWheel;
			struct { Uint8 axis; int direction; } joypadAxis;
			Uint8 joypadButton;
			struct { Uint8 index; Uint8 direction; } joystickHat;
			SDL_FingerID finger; // touchscreen finger
		} boundTo;

		SDL_JoystickID deviceInstanceID = -1; // SDL joystick/touch device index

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

		void bindToMouseButton(Uint8 button)
		{
			deviceType = DeviceType::MouseButton;
			boundTo.mouseButton = button;
		}

		void bindToMouseWheel(int direction)
		{
			deviceType = DeviceType::MouseWheel;
			boundTo.mouseWheel.direction = direction;
		}
		
		void bindToJoystickButton(SDL_JoystickID joystickID, Uint8 button)
		{
			deviceType = DeviceType::JoystickButton;
			deviceInstanceID = joystickID;
			boundTo.joypadButton = button;
		}

		void bindToJoystickAxis(SDL_JoystickID joystickID, int axis, int direction)
		{
			deviceType = DeviceType::JoystickAxis;
			deviceInstanceID = joystickID;
			boundTo.joypadAxis.axis = axis;
			boundTo.joypadAxis.direction = direction; // -1 or 1
		}

		void bindToJoystickHat(SDL_JoystickID joystickID, Uint8 hatIndex, Uint8 direction)
		{
			deviceType = DeviceType::JoystickHat;
			deviceInstanceID = joystickID;
			boundTo.joystickHat.index = hatIndex;
			boundTo.joystickHat.direction = direction;
		}

		void bindToControllerButton(SDL_JoystickID which, Uint8 button)
		{
			deviceType = DeviceType::GameControllerButton;
			deviceInstanceID = which;
			boundTo.joypadButton = button;
		}

		void bindToControllerAxis(SDL_JoystickID which, Uint8 axis, int direction)
		{
			deviceType = DeviceType::GameControllerAxis;
			deviceInstanceID = which;
			boundTo.joypadAxis.axis = axis;
			boundTo.joypadAxis.direction = direction; // -1 or 1
		}

		DeviceType getDeviceType() const
		{
			return deviceType;
		}

		std::string getDeviceTypeString() const
		{
			static const char* LUT[] = {
				"Disconnected",
				"Keyboard",
				"Mouse Button",
				"Mouse Wheel",
				"Joystick Button",
				"Joystick Axis",
				"Joystick Hat",
				"Touch Finger",
				nullptr
			};

			int deviceTypeInt = (int)deviceType;

			if (deviceTypeInt < 0 || deviceTypeInt >= (int)DeviceType::TOTAL)
				return "UNKNOWN";
			else return LUT[deviceTypeInt];
		}

		SDL_Scancode getScancode() const
		{
			if (deviceType != DeviceType::Keyboard)
				return SDL_Scancode::SDL_SCANCODE_UNKNOWN;
			else return boundTo.key;
		}

		std::string getScancodeString() const
		{
			return SDL_GetScancodeName(getScancode());
		}

		Uint8 getMouseButton() const
		{
			if (deviceType != DeviceType::MouseButton)
				return 0;
			else return boundTo.mouseButton;
		}

		std::string getMouseButtonString() const
		{
			switch (getMouseButton())
			{
			case 1:
				return "Left mouse button";
			case 2:
				return "Middle mouse button";
			case 3:
				return "Right mouse button";
			case 0:
				return "Unknown mouse button!";
			default:
				return "Mouse button " + std::to_string(getMouseButton());
			}
		}

		int getMouseWheelDirection() const
		{
			if (deviceType != DeviceType::MouseWheel)
				return 0;
			else return boundTo.mouseWheel.direction;
		}

		std::string getMouseWheelString() const
		{
			if (deviceType != DeviceType::MouseWheel)
				return "which isn't a mousewheel!";
			
			if (boundTo.mouseWheel.direction > 0)
				return "up";
			else if (boundTo.mouseWheel.direction < 0)
				return "down";
			else return "in an unknown direction!";
		}

		Uint8 getJoypadID() const
		{
			if (deviceType == DeviceType::JoystickAxis
				|| deviceType == DeviceType::JoystickButton
				|| deviceType == DeviceType::JoystickHat
				) return deviceInstanceID;
			else return -1;
		}

		Uint8 getJoystickAxis() const
		{
			if (deviceType != DeviceType::JoystickAxis
				&& deviceType != DeviceType::GameControllerAxis)
				return -1;
			else return boundTo.joypadAxis.axis;
		}

		int getJoystickAxisDirection() const
		{
			if (deviceType != DeviceType::JoystickAxis
				&& deviceType != DeviceType::GameControllerAxis)
				return -1;
			else return boundTo.joypadAxis.direction;
		}

		std::string getJoypadAxisString() const
		{
			if (deviceType != DeviceType::JoystickAxis
				&& deviceType != DeviceType::GameControllerAxis)
				return "of an unknown kind!";

			std::string axisString = std::to_string(boundTo.joypadAxis.axis);
			
			if (boundTo.joypadAxis.direction > 0) axisString += "+";
			else if (boundTo.joypadAxis.direction < 0) axisString += "-";
			else axisString += "?";

			return axisString;
		}
		
		Uint8 getJoypadButton() const
		{
			if (deviceType != DeviceType::JoystickButton
				&& deviceType != DeviceType::GameControllerButton)
				return -1;
			else return boundTo.joypadButton;
		}

		std::string getJoypadButtonString() const
		{
			if (deviceType == DeviceType::JoystickButton)
			{
				return std::to_string(boundTo.joypadButton);
			}
			else if (deviceType == DeviceType::GameControllerButton)
			{
				auto sdlGameControllerButton = (SDL_GameControllerButton)boundTo.joypadButton;
				return SDL_GameControllerGetStringForButton(sdlGameControllerButton);
			}
			else
			{
				return "of an unknown kind!";
			}
		}

		Uint8 getJoystickHatIndex() const
		{
			if (deviceType != DeviceType::JoystickHat)
				return -1;
			else return boundTo.joystickHat.index;
		}

		Uint8 getJoystickHatDirection() const
		{
			if (deviceType != DeviceType::JoystickHat)
				return -1;
			else return boundTo.joystickHat.direction;
		}

		bool isJoystickHatDirectionEquivalent(Uint8 directionToTest) const
		{
			if (deviceType != DeviceType::JoystickHat)
				return false;
			
			// Bitwise AND the wanted and the actual
			auto mashup = boundTo.joystickHat.direction & directionToTest;
			return mashup != 0;
		}

		std::string getJoystickHatString() const
		{
			if (deviceType != DeviceType::JoystickHat)
				return "that isn't a hat";

			std::string hatString = std::to_string(boundTo.joystickHat.index) + " ";
			
			switch (boundTo.joystickHat.direction)
			{
			case SDL_HAT_UP:
				hatString += "up";
				break;
			case SDL_HAT_DOWN:
				hatString += "down";
				break;
			case SDL_HAT_LEFT:
				hatString += "left";
				break;
			case SDL_HAT_RIGHT:
				hatString += "right";
				break;
			case SDL_HAT_LEFTUP:
				hatString += "upleft";
				break;
			case SDL_HAT_RIGHTUP:
				hatString += "upright";
				break;
			case SDL_HAT_LEFTDOWN:
				hatString += "downleft";
				break;
			case SDL_HAT_RIGHTDOWN:
				hatString += "downright";
				break;
			case SDL_HAT_CENTERED:
				hatString += "center";
				break;
			default:
				hatString += "in an unknown state";
				break;
			}

			return hatString;
		}

		std::string getJoypadName() const
		{
			if (deviceType == DeviceType::GameControllerButton
				|| deviceType == DeviceType::GameControllerAxis)
			{
				auto sdlGameController = SDL_GameControllerFromInstanceID(deviceInstanceID);
				auto sdlGameControllerName = SDL_GameControllerName(sdlGameController);
				if (sdlGameControllerName == nullptr)
					return "Unknown controller";
				else
					return SDL_GameControllerName(sdlGameController);
			}
			else return std::to_string(getJoypadID());
		}

		std::string makeBindingString() const
		{
			switch (deviceType)
			{
			case ssge::InputBinding::DeviceType::Disconnected:
				return "Disconnected";
			case ssge::InputBinding::DeviceType::Keyboard:
				return getScancodeString() + " key";
			case ssge::InputBinding::DeviceType::MouseButton:
				return getMouseButtonString();
			case ssge::InputBinding::DeviceType::MouseWheel:
				return std::string("Mouse wheel ") + getMouseWheelString();
			case ssge::InputBinding::DeviceType::JoystickButton:
				return std::string("Joystick ") + getJoypadName() + " button " + getJoypadButtonString();
			case ssge::InputBinding::DeviceType::GameControllerButton:
				return getJoypadName() + ":" + getJoypadButtonString();
			case ssge::InputBinding::DeviceType::JoystickAxis:
				return std::string("Joystick ") + getJoypadName() + " axis " + getJoypadAxisString();
			case ssge::InputBinding::DeviceType::GameControllerAxis:
				return getJoypadName() + ":" + getJoypadAxisString();
			case ssge::InputBinding::DeviceType::JoystickHat:
				return std::string("Joystick ") + getJoypadName() + " hat " + getJoystickHatString();
			case ssge::InputBinding::DeviceType::TouchFinger:
				return "FINGER UNIMPLEMENTED!";
			default:
				return "UNKNOWN";
			}
		}

		InputBinding() { disconnect(); }
		explicit InputBinding(SDL_Scancode key) { bindToKey(key); }
		explicit InputBinding(Uint8 mouseButton) { bindToMouseButton(mouseButton); }
		InputBinding(int joystickID, Uint8 mouseButton) { bindToJoystickButton(joystickID, mouseButton); }
		InputBinding(int joystickID, int axis, int direction) { bindToJoystickAxis(joystickID, axis, direction); }
		InputBinding(int joystickID, Uint8 hat, Uint8 direction) { bindToJoystickHat(joystickID, hat, direction); }

		bool matchesEvent(const SDL_Event& e) const
		{
			switch (deviceType)
			{
			case DeviceType::Keyboard:
				return (e.type == SDL_KEYDOWN
					|| e.type == SDL_KEYUP) &&
				        e.key.keysym.scancode == boundTo.key;
			case DeviceType::MouseButton:
				return (e.type == SDL_MOUSEBUTTONDOWN
					|| e.type == SDL_MOUSEBUTTONUP) &&
				        e.button.button == boundTo.mouseButton;
			case DeviceType::MouseWheel:
				return (e.type == SDL_MOUSEWHEEL
					&& e.wheel.y == boundTo.mouseWheel.direction);
			case DeviceType::JoystickButton:
				return (e.type == SDL_JOYBUTTONDOWN
					|| e.type == SDL_JOYBUTTONUP) &&
				        /*e.jbutton.which == deviceInstanceID &&*/ //FIXME: Implement autorebind!
				        e.jbutton.button == boundTo.joypadButton;
			case DeviceType::JoystickAxis:
				if (e.type == SDL_JOYAXISMOTION
					/* && e.jaxis.which == deviceInstanceID*/) // for the con //FIXME: Implement autorebind!
				{
					if (e.jaxis.axis == boundTo.joypadAxis.axis)
					{
						return true;
					}
				}
				return false;
			case DeviceType::JoystickHat:
				if (e.type == SDL_JOYHATMOTION
					/* && e.jhat.which == deviceInstanceID*/) // for the con //FIXME: Implement autorebind!
				{
					if (e.jhat.hat == boundTo.joystickHat.index)
					{
						return true;
					}
				}
				return false;
			case DeviceType::GameControllerButton:
				return (e.type == SDL_CONTROLLERBUTTONDOWN // If pressed
					|| e.type == SDL_CONTROLLERBUTTONUP) && // or released
					//e.cbutton.which == deviceInstanceID && // this device //FIXME: Implement autorebind!
					e.cbutton.button == boundTo.joypadButton; // this button
			case DeviceType::GameControllerAxis:
				if (e.type == SDL_JOYAXISMOTION
					/*&& e.caxis.which == deviceInstanceID*/ //FIXME: Implement autorebind!
					)
				{
					if (e.caxis.axis == boundTo.joypadAxis.axis)
					{
						return true;
					}
				}
				return false;
			default:
				return false;
			}
		}

		bool operator==(const InputBinding& b) const
		{
			const InputBinding& a = *this;

			if (a.deviceType != b.deviceType) return false;

			// For the con //FIXME: Implement autorebind!
			//if (a.deviceInstanceID != b.deviceInstanceID) return false;

			switch (a.deviceType)
			{
			case InputBinding::DeviceType::Disconnected:
				return true;

			case InputBinding::DeviceType::Keyboard:
				return a.boundTo.key == b.boundTo.key;

			case InputBinding::DeviceType::MouseButton:
				return a.boundTo.mouseButton == b.boundTo.mouseButton;

			case InputBinding::DeviceType::MouseWheel:
				return a.boundTo.mouseWheel.direction == b.boundTo.mouseWheel.direction;

			case InputBinding::DeviceType::JoystickButton:
			case InputBinding::DeviceType::GameControllerButton:
				return a.boundTo.joypadButton == b.boundTo.joypadButton;

			case InputBinding::DeviceType::JoystickAxis:
			case InputBinding::DeviceType::GameControllerAxis:
				return (a.boundTo.joypadAxis.axis == b.boundTo.joypadAxis.axis) &&
					(a.boundTo.joypadAxis.direction == b.boundTo.joypadAxis.direction);

			case InputBinding::DeviceType::JoystickHat:
				return (a.boundTo.joystickHat.index == b.boundTo.joystickHat.index) &&
					(a.boundTo.joystickHat.direction == b.boundTo.joystickHat.direction);

			case InputBinding::DeviceType::TouchFinger:
				return a.boundTo.finger == b.boundTo.finger;

			default:
				return false;
			}
		}

		bool operator!=(const InputBinding& b) const
		{
			const InputBinding& a = *this;

			return !(a == b);
		}
	};
}