#pragma once
#include <cstdint>
#include "InputSet.h"

namespace ssge
{
	// Represents a hardware-independent 32-bit button pad and tracks button states through frames.
	class InputPad
	{
		// Bitmask of buttons held this frame.
		uint32_t currentButtons = 0;

		// Bitmask of buttons held last frame.
		uint32_t oldButtons = 0;
	public:
		InputPad() noexcept {};
		
		// Latch the full button bitfield for this frame; oldButtons is updated internally.
		void latchButtons(uint32_t directButtons) noexcept;

		// Latch using precomputed just-pressed / just-released masks.
		void latchButtons(
			uint32_t justPressedButtons, uint32_t justReleasedButtons) noexcept;

		// Returns a bitmask of buttons that are held down this frame.
		uint32_t getPressedButtons() const noexcept;

		// Returns a bitmask of buttons that transitioned from up->down this frame.
		uint32_t getJustPressedButtons() const noexcept;

		// Returns a bitmask of buttons that transitioned from down->up this frame.
		uint32_t getJustReleasedButtons() const noexcept;

		// Returns a bitmask of buttons that are not held down this frame.
		uint32_t getReleasedButtons() const noexcept;

		// Returns whether the indexed button is held this frame.
		bool isPressed(InputSet buttonIndex) const noexcept;

		// Returns whether the indexed button transitioned from up->down this frame.
		bool isJustPressed(InputSet buttonIndex) const noexcept;

		// Returns whether the indexed button transitioned from down->up this frame.
		bool isJustReleased(InputSet buttonIndex) const noexcept;

		// Returns whether the button(s) with this bitmask is/are held this frame.
		bool checkPressed(uint32_t buttonMask) const noexcept;

		// Returns whether the button(s) with this bitmask transitioned from up->down this frame.
		bool checkJustPressed(uint32_t buttonMask) const noexcept;

		// Returns whether the button(s) with this bitmask transitioned from down->up this frame.
		bool checkJustReleased(uint32_t buttonMask) const noexcept;
	};
}