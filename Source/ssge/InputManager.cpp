#include "InputManager.h"
#include "SDL.h"

using namespace ssge;

InputManager::InputManager(PassKey<Engine> pk) {}

void InputManager::init(PassKey<Engine> pk)
{

    // Enable joystick and game controller events
    SDL_JoystickEventState(SDL_ENABLE);
    SDL_GameControllerEventState(SDL_ENABLE);

    // Open every game device that's plugged in
    prepareJoypadSlots();
}

void InputManager::handle(SDL_Event e)
{
    // Joypad hardware change
    switch (e.type)
    {
    case SDL_CONTROLLERDEVICEADDED:
        onControllerAdded(e.cdevice.which); // device index
        return;
    case SDL_CONTROLLERDEVICEREMOVED:
        onControllerRemoved(e.cdevice.which); // instance id
        return;
    case SDL_JOYDEVICEADDED:
        onJoystickAdded(e.jdevice.which); // device index
        return;
    case SDL_JOYDEVICEREMOVED:
        onJoystickRemoved(e.jdevice.which); // instance id
        return;
    }

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

        // All is handled
        return;
    }
    else
    { // Normal listening and parsing into directInputs

        // Clear all SDL_MOUSEWHEEL-bound inputs
        // because we don't have a MouseWheelRelease event.
        // Without this, the mousewheel inputs get jammed!
        for (int index = 0; index < 32; index++)
        {
            if (bindings[index].getDeviceType()
                == InputBinding::DeviceType::MouseWheel)
            { // Clear
                directInputs &= ~(1 << index);
            }
        }

        // See if incoming event matches a binding
        for (int index = 0; index < 32; index++)
        {
            if (bindings[index].matchesEvent(e))
            { // We support
                if (e.type == SDL_KEYDOWN // key press
                 || e.type == SDL_MOUSEBUTTONDOWN // mouse button press
                 || e.type == SDL_MOUSEWHEEL // mouse wheel roll
                 || e.type == SDL_JOYBUTTONDOWN) // joystick button press
                { // Set the bit
                    directInputs |= 1 << index;
                }
                else if (e.type == SDL_KEYUP // key release
                      || e.type == SDL_MOUSEBUTTONUP // mouse button release
                      || e.type == SDL_JOYBUTTONUP) // joystick button release
                { // Clear the bit
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

// Joypad stuff

void InputManager::prepareJoypadSlots()
{
    int n = SDL_NumJoysticks();
    for (int i = 0; i < n; i++)
    {
        if (SDL_IsGameController(i))
            onControllerAdded(i);
        else
            onJoystickAdded(i);
    }
}

void InputManager::onControllerAdded(int deviceIndex)
{
    int slot = getFreeJoypadSlot();
    if (slot < 0)
        return;

    SDL_GameController* c = SDL_GameControllerOpen(deviceIndex);
    if (!c)
        return;

    joypadSlots[slot].ctrl = c;
    SDL_Joystick* j = SDL_GameControllerGetJoystick(c);
    joypadSlots[slot].instance = SDL_JoystickInstanceID(j);
}

void InputManager::onControllerRemoved(SDL_JoystickID id)
{
    int slot = getJoypadSlotByInstance(id);
    if (slot < 0)
        return;

    if (joypadSlots[slot].ctrl)
    {
        SDL_GameControllerClose(joypadSlots[slot].ctrl);
        joypadSlots[slot].ctrl = nullptr;
    }

    joypadSlots[slot].joy = nullptr; // just in case
    joypadSlots[slot].instance = -1;
}

void InputManager::onJoystickAdded(int deviceIndex)
{
    // Only if NOT a GameController
    if (SDL_IsGameController(deviceIndex))
        return;

    int slot = getFreeJoypadSlot();
    if (slot < 0)
        return;

    SDL_Joystick* j = SDL_JoystickOpen(deviceIndex);
    if (!j)
        return;

    joypadSlots[slot].joy = j;
    joypadSlots[slot].instance = SDL_JoystickInstanceID(j);
}

void InputManager::onJoystickRemoved(SDL_JoystickID id)
{
    int slot = getJoypadSlotByInstance(id);
    if (slot < 0)
        return;

    if (joypadSlots[slot].joy)
    {
        SDL_JoystickClose(joypadSlots[slot].joy);
        joypadSlots[slot].joy = nullptr;
    }
    joypadSlots[slot].ctrl = nullptr; // just in case
    joypadSlots[slot].instance = -1;
}

int InputManager::getJoypadSlotByInstance(SDL_JoystickID id) const
{
    for (int i = 0; i < MAX_JOYPADS; i++)
    {
        if (joypadSlots[i].instance == id)
        {
            return i;
        }
    }
    return -1;
}

InputManager::Joypad* ssge::InputManager::getJoypadByInstance(SDL_JoystickID id)
{
    for (int i = 0; i < MAX_JOYPADS; i++)
    {
        Joypad* joypad = &joypadSlots[i];
        if (joypad->instance == id)
        {
            return joypad;
        }
    }
    return nullptr;
}

int InputManager::getFreeJoypadSlot() const
{
    for (int i = 0; i < MAX_JOYPADS; i++)
    {
        if (!joypadSlots[i].ctrl && !joypadSlots[i].joy)
        {
            return i;
        }
    }
    return -1;
}

InputManager::Joypad* ssge::InputManager::getFreeJoyPad()
{
    for (int i = 0; i < MAX_JOYPADS; i++)
    {
        Joypad* joypad = &joypadSlots[i];
        if (joypad->ctrl && joypad->joy)
        {
            return joypad;
        }
    }
    return nullptr;
}

const InputPad& InputManager::getPad() const
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
