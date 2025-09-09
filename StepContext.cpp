#include "StepContext.h"

ssge::StepContext::StepContext(ssge::Program& program) : program(&program)
{
}

ssge::StepContext::~StepContext()
{
}

ssge::StepContext::Program::Program(ssge::Program* actualProgram)
{
    this->actualProgram = actualProgram;
}

ssge::StepContext::Program::~Program()
{
}

void ssge::StepContext::Program::exit()
{
    actualProgram->requestExit();
}

ssge::StepContext::InputManager::InputManager(ssge::InputManager* actualInputManager)
{
    this->actualInputManager = actualInputManager;
}

ssge::StepContext::InputManager::~InputManager()
{
}
