#include "InputManager.h"
#include "SDL.h"

using namespace ssge;

InputManager::InputManager(PassKey<Engine> pk) {}

void InputManager::handle(SDL_Event e)
{
    for (int index = 0; index < 32; index++)
    {
        if (bindings[index].matchesEvent(e))
        {
            if (e.type == SDL_KEYDOWN || e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_JOYBUTTONDOWN)
            {
                directInputs |= 1 << index;
            }
            else if (e.type == SDL_KEYUP || e.type == SDL_MOUSEBUTTONUP || e.type == SDL_JOYBUTTONUP)
            {
                directInputs &= ~(1 << index);
            }
        }
    }
}

void InputManager::latch()
{
    pad.latchButtons(directInputs);
}

const InputPad& InputManager::getPad()
{
    return pad;
}
