#include "InputPad.h"

using namespace ssge;

void InputPad::latchButtons(uint32_t directButtons) noexcept
{
	this->oldButtons = this->currentButtons;
	this->currentButtons = directButtons;
}

void InputPad::latchButtons(
	uint32_t justPressedButtons, uint32_t justReleasedButtons) noexcept
{
	this->oldButtons = this->currentButtons;
	this->currentButtons |= justPressedButtons;
	this->currentButtons &= ~justReleasedButtons;
}

uint32_t InputPad::getPressedButtons() const noexcept
{
	return this->currentButtons;
}

uint32_t InputPad::getJustPressedButtons() const noexcept
{
	return this->currentButtons & ~this->oldButtons;
}

uint32_t InputPad::getJustReleasedButtons() const noexcept
{
	return ~this->currentButtons & this->oldButtons;
}

uint32_t InputPad::getReleasedButtons() const noexcept
{
	return ~this->currentButtons;
}

bool ssge::InputPad::isPressed(InputSet buttonIndex) const noexcept
{
	return checkPressed(1 << (unsigned)buttonIndex);
}

bool ssge::InputPad::isJustPressed(InputSet buttonIndex) const noexcept
{
	return checkJustPressed(1 << (unsigned)buttonIndex);
}

bool ssge::InputPad::isJustReleased(InputSet buttonIndex) const noexcept
{
	return checkJustReleased(1 << (unsigned)buttonIndex);
}

bool InputPad::checkPressed(uint32_t buttonMask) const noexcept
{
	return ((getPressedButtons() & buttonMask) == buttonMask);
}

bool InputPad::checkJustPressed(uint32_t buttonMask) const noexcept
{
	return ((getJustPressedButtons() & buttonMask) == buttonMask);
}

bool InputPad::checkJustReleased(uint32_t buttonMask) const noexcept
{
	return ((getJustReleasedButtons() & buttonMask) == buttonMask);
}
