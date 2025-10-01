#pragma once
#include <cstdint>

namespace ssge
{
	class InputPad
	{
		uint32_t currentButtons;
		uint32_t oldButtons;
	public:
		InputPad();
		void latchButtons(uint32_t directButtons);
		void latchButtons(uint32_t justPressedButtons, uint32_t justReleasedButtons);
		uint32_t getPressedButtons() const;
		uint32_t getJustPressedButtons() const;
		uint32_t getJustReleasedButtons() const;
		uint32_t getReleasedButtons() const;
		bool checkPressed(uint32_t buttonMask) const;
		bool checkJustPressed(uint32_t buttonMask) const;
		bool checkJustReleased(uint32_t buttonMask) const;
	};
}