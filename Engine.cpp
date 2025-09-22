#include "Engine.h"

ssge::Engine::Engine(ssge::Program& program) : program(program)
{
	scenes = new SceneManager();
	inputs = new InputManager();
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
	auto context = SceneManagerStepContext(&program, this);
	scenes->step(context);
	return false;
}

void ssge::Engine::render(SDL_Renderer* renderer)
{
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
