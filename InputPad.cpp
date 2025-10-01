#include "InputPad.h"

using namespace ssge;

InputPad::InputPad()
{
	this->currentButtons = 0;
	this->oldButtons = 0;
}

void InputPad::latchButtons(uint32_t directButtons)
{
	this->oldButtons = this->currentButtons;
	this->currentButtons = directButtons;
}

void InputPad::latchButtons(uint32_t justPressedButtons, uint32_t justReleasedButtons)
{
	this->oldButtons = this->currentButtons;
	this->currentButtons |= justPressedButtons;
	this->currentButtons &= ~justReleasedButtons;
}

uint32_t InputPad::getPressedButtons() const
{
	return this->currentButtons;
}

uint32_t InputPad::getJustPressedButtons() const
{
	return this->currentButtons & ~this->oldButtons;
}

uint32_t InputPad::getJustReleasedButtons() const
{
	return ~this->currentButtons & this->oldButtons;
}

uint32_t InputPad::getReleasedButtons() const
{
	return ~this->currentButtons;
}

bool InputPad::checkPressed(uint32_t buttonMask) const
{
	return ((getPressedButtons() & buttonMask) == buttonMask);
}

bool InputPad::checkJustPressed(uint32_t buttonMask) const
{
	return ((getJustPressedButtons() & buttonMask) == buttonMask);
}

bool InputPad::checkJustReleased(uint32_t buttonMask) const
{
	return ((getJustReleasedButtons() & buttonMask) == buttonMask);
}
