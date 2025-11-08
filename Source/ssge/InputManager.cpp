#include "InputManager.h"
#include "SDL.h"

using namespace ssge;

InputManager::InputManager(PassKey<Engine> pk) {}

void InputManager::handle(SDL_Event e)
{
    // Do not parse direct inputs if listening for binding
    if (isListeningForBinding())
    {
        // Release all direct inputs
        directInputs = 0;

        // See if something was pressed
        switch (e.type)
        { // And bind it!
        case SDL_KEYDOWN:
            lastBinding.bindToKey(e.key.keysym.scancode);
            break;
        case SDL_MOUSEBUTTONDOWN:
            lastBinding.bindToMouse(e.button.button);
            break;
        case SDL_JOYBUTTONDOWN:
            lastBinding.bindToJoystickButton(e.jbutton.which, e.jbutton.button);
            break;
        default:
            // Bail out if nothing was pressed
            return;
        }

        // Bind!
        bindings[listeningFor] = lastBinding;

        // Not listening anymore. We've got our binding
        stopListeningForBinding();
    }
    else
    { // Normal listening and parsing into directInputs
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
}

void InputManager::latch()
{
    pad.latchButtons(directInputs);
}

const InputPad& InputManager::getPad()
{
    return pad;
}

bool ssge::InputManager::isListeningForBinding() const
{
    // If listeningFor is valid, that means we're listening
    return listeningFor >= 0;
}

void ssge::InputManager::listenForBinding(int bindingIndex)
{
    if (bindingIndex < 0 || bindingIndex >= MAX_BINDINGS)
        this->listeningFor = -1; // Disable on invalid index
    else // Listen for binding and assign to this index
        this->listeningFor = bindingIndex;
}

void ssge::InputManager::stopListeningForBinding()
{
    listeningFor = -1;
}

InputBinding ssge::InputManager::getBinding(int bindingIndex) const
{
    if (bindingIndex < 0 || bindingIndex >= MAX_BINDINGS)
        return InputBinding(); // Return empty binding on bad index
    else bindings[bindingIndex];
}

std::string ssge::InputManager::getBindingString(int bindingIndex) const
{
    if (bindingIndex < 0 || bindingIndex >= MAX_BINDINGS)
        return "N/A"; // Invalid index
    else if (bindingIndex == listeningFor)
        return "Press a button..."; // We're listening to this one
    else // Let the binding speak for itself
        return bindings[bindingIndex].makeBindingString();
}

int ssge::InputManager::getMaxBindings() const
{
    return MAX_BINDINGS;
}
