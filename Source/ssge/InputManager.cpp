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

bool ssge::InputManager::handleHardwareChange(SDL_Event e)
{
    // Joypad hardware change
    switch (e.type)
    {
        /*case SDL_CONTROLLERDEVICEADDED:
            onControllerAdded(e.cdevice.which); // device index
            return true;
        case SDL_CONTROLLERDEVICEREMOVED:
            onControllerRemoved(e.cdevice.which); // instance id
            return true;*/
    case SDL_JOYDEVICEADDED:
        onJoystickAdded(e.jdevice.which); // device index
        return true;
    case SDL_JOYDEVICEREMOVED:
        onJoystickRemoved(e.jdevice.which); // instance id
        return true;
    }

    // Don't swallow the event!
    return false;
}

bool ssge::InputManager::handleListeningForBinding(SDL_Event e)
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
            lastBinding.bindToMouseButton(e.button.button);
            break;
        case SDL_MOUSEWHEEL:
            lastBinding.bindToMouseWheel(e.wheel.direction);
            break;

            // GameController (XInput-style, preferred on Windows)
        case SDL_CONTROLLERBUTTONDOWN:
            lastBinding.bindToControllerButton(e.cbutton.which, e.cbutton.button);
            break;
        case SDL_CONTROLLERAXISMOTION:
            if (e.caxis.value > 16000) // Positive direction
                lastBinding.bindToControllerAxis(e.caxis.which, e.caxis.axis, 1);
            else if (e.caxis.value < -16000) // Negative direction
                lastBinding.bindToControllerAxis(e.caxis.which, e.caxis.axis, -1);
            else return true; // Not enough pushed axis. Irrelevant!
            break;

            // Legacy joysticks
        case SDL_JOYBUTTONDOWN:
            lastBinding.bindToJoystickButton(e.jbutton.which, e.jbutton.button);
            break;
        case SDL_JOYAXISMOTION:
            if (e.jaxis.value > 16000) // Positive direction
                lastBinding.bindToJoystickAxis(
                    e.jaxis.which, e.jaxis.axis, 1);
            else if (e.jaxis.value < -16000) // Negative direction
                lastBinding.bindToJoystickAxis(
                    e.jaxis.which, e.jaxis.axis, -1);
            else return true; // Not enough pushed axis. Irrelevant!
            break;
        case SDL_JOYHATMOTION:
            lastBinding.bindToJoystickHat(e.jhat.which, e.jhat.hat, e.jhat.value);
            break;
        default:
            // Bail out if nothing was pressed
            return true;
        }

        // Check if this binding is used as a fallback already
        bool alreadyExists = false;
        for (int i = 0; i < MAX_BINDINGS; i++)
        {
            if (fallbackBindings[i] == lastBinding)
            {
                // We will tolerate if the user wants to bring it back
                if (i == listeningFor)
                    continue;

                if (lastBinding.getDeviceType()
                    == InputBinding::DeviceType::Disconnected)
                    continue;

                alreadyExists = true;
                break;
            }
        }
        if (!alreadyExists)
        {
            // Bind!
            bindings[listeningFor] = lastBinding;
        }

        // Not listening anymore. We've got our binding
        stopListeningForBinding();

        // All is handled
        return true;
    }
    else
    {
        // Don't swallow the event!
        return false;
    }
}

bool ssge::InputManager::handleBindings(SDL_Event e, InputBinding* chosenBindings)
{
    bool eventSwallowed = false;

    // Clear all SDL_MOUSEWHEEL-bound inputs
    // because we don't have a MouseWheelRelease event.
    // Without this, the mousewheel inputs get jammed!
    for (int index = 0; index < 32; index++)
    {
        if (chosenBindings[index].getDeviceType()
            == InputBinding::DeviceType::MouseWheel)
        { // Clear no matter what
            directInputs &= ~(1 << index);
        }
    }

    // See if incoming event matches a binding
    for (int index = 0; index < 32; index++)
    {
        auto& binding = chosenBindings[index];
        if (binding.matchesEvent(e))
        { // We support
            if (e.type == SDL_KEYDOWN // key press
                || e.type == SDL_MOUSEBUTTONDOWN // mouse button press
                || e.type == SDL_MOUSEWHEEL // mouse wheel roll
                || e.type == SDL_JOYBUTTONDOWN // joystick button press
                || e.type == SDL_CONTROLLERBUTTONDOWN) // controller -||-
            { // Set the bit
                directInputs |= 1 << index;
                eventSwallowed = true;
            }
            else if (e.type == SDL_KEYUP // key release
                || e.type == SDL_MOUSEBUTTONUP // mouse button release
                || e.type == SDL_JOYBUTTONUP // joystick button release
                || e.type == SDL_CONTROLLERBUTTONUP) // controller -||-
            { // Clear the bit
                directInputs &= ~(1 << index);
                eventSwallowed = true;
            }
            else if (e.type == SDL_JOYHATMOTION)
            {
                // We must check hat direction
                if (binding.isJoystickHatDirectionEquivalent(e.jhat.value))
                {
                    directInputs |= 1 << index;
                    eventSwallowed = true;
                }
                else
                { // The hat has moved AWAY from the mapped direction!
                    directInputs &= ~(1 << index);
                    eventSwallowed = true;
                }
            }
            else if (e.type == SDL_JOYAXISMOTION)
            {
                // We must check the axis direction
                int wantedDirection = binding.getJoystickAxisDirection();
                Sint16 axisValue = e.jaxis.value;
                if (std::abs(axisValue) > 8000)
                {
                    // If the signs are the same,
                    // then this is what we want
                    if (wantedDirection * axisValue > 0)
                    { // Count it as pressed
                        directInputs |= 1 << index;
                        eventSwallowed = true;
                    }
                    else if (wantedDirection * axisValue < 0)
                    { // Count it as released
                        directInputs &= ~(1 << index);
                        eventSwallowed = true;
                    }
                    else
                    {
                        // WHAT!? This is impossible!
                    }
                }
                else
                { // If it's below the threshold, then it's surely released
                    directInputs &= ~(1 << index);
                    eventSwallowed = true;
                }
            }
            else if (e.type == SDL_CONTROLLERAXISMOTION)
            {
                // We must check the axis direction
                int wantedDirection = binding.getJoystickAxisDirection();
                Sint16 axisValue = e.caxis.value;
                if (std::abs(axisValue) > 8000)
                {
                    // If the signs are the same,
                    // then this is what we want
                    if (wantedDirection * axisValue > 0)
                    { // Count it as pressed
                        directInputs |= 1 << index;
                        eventSwallowed = true;
                    }
                    else if (wantedDirection * axisValue < 0)
                    { // Count it as released
                        directInputs &= ~(1 << index);
                        eventSwallowed = true;
                    }
                    else
                    {
                        // WHAT!? This is impossible!
                    }
                }
            }
        }
    }

    return eventSwallowed;
}

void InputManager::handle(SDL_Event e)
{
    if (handleHardwareChange(e)) return;
    if (handleListeningForBinding(e)) return;
    if (handleBindings(e, bindings)) return;
    if (handleBindings(e, fallbackBindings)) return;
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
    //if (SDL_IsGameController(deviceIndex))
    //    return;

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
    else return bindings[bindingIndex];
}

InputBinding* ssge::InputManager::fetchBinding(int bindingIndex)
{
    if (bindingIndex < 0 || bindingIndex >= MAX_BINDINGS)
        return nullptr; // Return empty binding on bad index
    else return &bindings[bindingIndex];
}

InputBinding* ssge::InputManager::fetchFallbackBinding(int bindingIndex)
{
    if (bindingIndex < 0 || bindingIndex >= MAX_BINDINGS)
        return nullptr; // Return empty binding on bad index
    else return &fallbackBindings[bindingIndex];
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

bool ssge::InputManager::loadFromIniFile(IniFile& iniFile)
{
    static const char* INI_SECTION = "InputBindings";

    InputBinding* inputBinding = nullptr;

    for (int i = 0; i < MAX_BINDINGS; i++)
    {
        inputBinding = &bindings[i];

        std::string bindingIdxStr = std::to_string(i);

        // DeviceType integer from INI
        int deviceTypeInt = iniFile.getInt(
            INI_SECTION, // Section
            std::string("DeviceType") + bindingIdxStr, // Key
            0 // Fallback value (Disconnected)
        );
        
        // Sanitize DeviceType integer
        if (deviceTypeInt < 0
            || deviceTypeInt >= (int)InputBinding::DeviceType::TOTAL)
        { // Fallback to disconnected
            deviceTypeInt = (int)InputBinding::DeviceType::Disconnected;
        }

        // Sanitized DeviceType for InputBinding
        auto deviceType = (InputBinding::DeviceType)deviceTypeInt;

        switch (deviceType)
        {
        case ssge::InputBinding::DeviceType::Keyboard:
            inputBinding->bindToKey((SDL_Scancode)
                iniFile.getInt(
                    INI_SECTION, // Section
                    std::string("Scancode") + bindingIdxStr, // Key
                    SDL_SCANCODE_UNKNOWN // Fallback value
                )
            );
            break;
        case ssge::InputBinding::DeviceType::MouseButton:
            inputBinding->bindToMouseButton(
                iniFile.getInt(
                    INI_SECTION, // Section
                    std::string("MouseButton") + bindingIdxStr, // Key
                    0 // Fallback value
                )
            );
            break;
        case ssge::InputBinding::DeviceType::MouseWheel:
            inputBinding->bindToMouseWheel(
                iniFile.getInt(
                    INI_SECTION, // Section
                    std::string("Direction") + bindingIdxStr, // Key
                    0 // Fallback value
                )
            );
            break;
        case ssge::InputBinding::DeviceType::JoystickButton:
            inputBinding->bindToJoystickButton(
                0, // JoystickID // TODO: Better detection of joystick slots!
                iniFile.getInt( // Button index
                    INI_SECTION, // Section
                    std::string("Button") + bindingIdxStr, // Key
                    0 // Fallback value
                )
            );
            break;
        case ssge::InputBinding::DeviceType::JoystickAxis:
            inputBinding->bindToJoystickAxis(
                0, // JoystickID // TODO: Better detection of joystick slots!
                iniFile.getInt( // Axis index
                    INI_SECTION, // Section
                    std::string("Axis") + bindingIdxStr, // Key
                    0 // Fallback value
                ),
                iniFile.getInt( // Axis direction
                    INI_SECTION, // Section
                    std::string("Direction") + bindingIdxStr, // Key
                    0 // Fallback value
                )
            );
            break;
        case ssge::InputBinding::DeviceType::JoystickHat:
            inputBinding->bindToJoystickHat(
                0, // JoystickID // TODO: Better detection of joystick slots!
                iniFile.getInt( // Hat index
                    INI_SECTION, // Section
                    std::string("Hat") + bindingIdxStr, // Key
                    0 // Fallback value
                ),
                iniFile.getInt( // Hat side
                    INI_SECTION, // Side
                    std::string("Direction") + bindingIdxStr, // Key
                    0 // Fallback value
                )
                // TODO: Sanitize further!
            );
            break;
        case ssge::InputBinding::DeviceType::GameControllerButton:
            // TODO: Support GameController when it actually works
            break;
        case ssge::InputBinding::DeviceType::GameControllerAxis:
            // TODO: Support GameController when it actually works
            break;
        case ssge::InputBinding::DeviceType::TouchFinger:
            // TODO: No touch support yet
            break;
        case ssge::InputBinding::DeviceType::Disconnected:
        default:
            inputBinding->disconnect();
            break;
        }
    }

    return false;
}

bool ssge::InputManager::saveToIniFile(IniFile& iniFile)
{
    static const char* INI_SECTION = "InputBindings";

    InputBinding* binding = nullptr;
    for (int i = 0; i < MAX_BINDINGS; i++)
    {
        binding = &bindings[i];

        std::string bindingIdxStr = std::to_string(i);

        InputBinding::DeviceType deviceType = binding->getDeviceType();

        iniFile.setInt(
            INI_SECTION,
            "DeviceType" + bindingIdxStr,
            (int)deviceType
        );

        switch (deviceType)
        {
        case ssge::InputBinding::DeviceType::Keyboard:
            iniFile.setInt(
                INI_SECTION,
                "Scancode" + bindingIdxStr,
                binding->getScancode()
            );
            break;
        case ssge::InputBinding::DeviceType::MouseButton:
            iniFile.setInt(
                INI_SECTION,
                "Button" + bindingIdxStr,
                binding->getMouseButton()
            );
            break;
        case ssge::InputBinding::DeviceType::MouseWheel:
            iniFile.setInt(
                INI_SECTION,
                "Direction" + bindingIdxStr,
                binding->getMouseWheelDirection()
            );
            break;
        case ssge::InputBinding::DeviceType::JoystickButton:
            iniFile.setInt(
                INI_SECTION,
                "Button" + bindingIdxStr,
                binding->getJoystickButton()
            );
            break;
        case ssge::InputBinding::DeviceType::JoystickAxis:
            iniFile.setInt(
                INI_SECTION,
                "Axis" + bindingIdxStr,
                binding->getJoystickAxis()
            );
            iniFile.setInt(
                INI_SECTION,
                "Direction" + bindingIdxStr,
                binding->getJoystickAxisDirection()
            );
            break;
        case ssge::InputBinding::DeviceType::JoystickHat:
            iniFile.setInt(
                INI_SECTION,
                "Hat" + bindingIdxStr,
                binding->getJoystickHatIndex()
            );
            iniFile.setInt(
                INI_SECTION,
                "Direction" + bindingIdxStr,
                binding->getJoystickHatDirection()
            );
            break;
        case ssge::InputBinding::DeviceType::GameControllerButton:
            // TODO: Support GameController when it actually works
            break;
        case ssge::InputBinding::DeviceType::GameControllerAxis:
            // TODO: Support GameController when it actually works
            break;
        case ssge::InputBinding::DeviceType::TouchFinger:
            // TODO: No touch support yet
            break;
        default:
            break;
        }
    }
    return false;
}
