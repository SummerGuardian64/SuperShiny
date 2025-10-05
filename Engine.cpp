#include "Engine.h"
#include "SceneManager.h"
#include "InputManager.h"
#include <memory>
#include "Factory.h"
#include "StepContext.h"
#include "DrawContext.h"
#include "Program.h"

#include "SplashScreen.h"

ssge::Engine::Engine(ssge::Program& program) : program(program)
{
	scenes = new SceneManager();
	inputs = new InputManager();
	wannaFinish = false;
	wannaWrapUp = false;
}

ssge::Engine::~Engine()
{
	shutdown();
}

bool ssge::Engine::init()
{
	if (!prepareInitialState()) return false;
	
	return true;
}

bool ssge::Engine::loadInitialResources()
{
	return true;
}

bool ssge::Engine::prepareInitialState()
{
	//TODO: Initialize first scene
	//scenes->changeScene(std::make_unique<GameWorld>());
	scenes->changeScene(std::make_unique<SplashScreen>());
	inputs->bindings[0].bindToKey(SDL_Scancode::SDL_SCANCODE_E);

	return true;
}

void ssge::Engine::handle(SDL_Event e)
{
	switch (e.type)
	{
	case SDL_EventType::SDL_KEYDOWN:
	case SDL_EventType::SDL_KEYUP:
	case SDL_EventType::SDL_JOYBUTTONDOWN:
	case SDL_EventType::SDL_JOYBUTTONUP:
	case SDL_EventType::SDL_JOYAXISMOTION:
	case SDL_EventType::SDL_JOYBALLMOTION:
	case SDL_EventType::SDL_JOYHATMOTION:
	case SDL_EventType::SDL_JOYDEVICEADDED:
	case SDL_EventType::SDL_JOYDEVICEREMOVED:
		inputs->handle(e);
		break;
	default:
		break;
	}
}

bool ssge::Engine::update(double deltaTime)
{
	// Stop updating the engine if it's finished
	if (wannaFinish)
		return false;

	// Start wrapping things up. This is a gentle quit request
	if (wannaWrapUp)
		scenes->wrapUp();

	// Latch all inputs the way the NES does it
	inputs->latch();
	
	// Step the scenes via SceneManager
	auto context = Factory::ForEngine::stepContext(this);
	scenes->step(context);

	// Let this be the final update if we're finished
	return !wannaFinish;
}

void ssge::Engine::render(DrawContext context)
{
	scenes->draw(context);
}

void ssge::Engine::shutdown()
{
	if (scenes)
	{
		delete scenes;
		scenes = nullptr;
	}
	if (inputs)
	{
		delete inputs;
		inputs = nullptr;
	}
}

ssge::SceneManager* ssge::Engine::getSceneManager()
{
	return scenes;
}

ssge::InputManager* ssge::Engine::getInputManager()
{
	return inputs;
}

ssge::WindowManager* ssge::Engine::getWindowManager()
{
	return program.window;
}

void ssge::Engine::finish()
{
	wannaFinish = true;
}

void ssge::Engine::wrapUp()
{
	wannaWrapUp = true;
}
