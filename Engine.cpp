#include "Engine.h"

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
	scenes->changeScene(std::make_unique<GameWorld>());
	
	return true;
}

void ssge::Engine::handle(SDL_Event e)
{
	switch (e.type)
	{
	case SDL_EventType::SDL_KEYDOWN:
		break;
	case SDL_EventType::SDL_KEYUP:
		break;
	case SDL_EventType::SDL_JOYBUTTONDOWN:
		break;
	case SDL_EventType::SDL_JOYBUTTONUP:
		break;
	case SDL_EventType::SDL_JOYAXISMOTION:
		break;
	case SDL_EventType::SDL_JOYBALLMOTION:
		break;
	case SDL_EventType::SDL_JOYHATMOTION:
		break;
	case SDL_EventType::SDL_JOYDEVICEADDED:
		break;
	case SDL_EventType::SDL_JOYDEVICEREMOVED:
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

void ssge::Engine::finish()
{
	wannaFinish = true;
}

void ssge::Engine::wrapUp()
{
	wannaWrapUp = true;
}
