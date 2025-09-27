#include "StepContext.h"

// We include engine headers here — only the .cpp sees the raw types.
// Headers remain free of raw public Program* signatures.
#include "Engine.h"
#include "SceneManager.h"
#include "GameWorld.h"
#include "Scene.h"

using namespace ssge;

// StepContext impl

ssge::StepContext::Engine::Engine(ssge::Engine* actual)
{
	this->actual = actual;
}

void ssge::StepContext::Engine::finish()
{
	actual->finish();
}

ssge::StepContext::Scenes::Scenes(ssge::SceneManager* actual)
{
	this->actual = actual;
}

void ssge::StepContext::Scenes::changeScene(std::unique_ptr<ssge::Scene> newScene)
{
	actual->changeScene(std::move(newScene));
}

ssge::StepContext::Inputs::Inputs(ssge::InputManager* actual)
{
	this->actual = actual;
}

ssge::StepContext::StepContext(ssge::Engine* actualEngine) 
	: engine(actualEngine),
	  scenes(actualEngine->getSceneManager()),
	  inputs(actualEngine->getInputManager())
{
	;
}

ssge::Engine* ssge::StepContext::getActualEngine()
{
	return engine.actual;
}

ssge::SceneManager* ssge::StepContext::getActualSceneManager()
{
	return scenes.actual;
}

ssge::InputManager* ssge::StepContext::getActualInputManager()
{
	return inputs.actual;
}

// SceneStepContext impl

ssge::SceneStepContext::SceneStepContext(ssge::Scene& currentScene, ssge::StepContext& stepContext)
	: engine(stepContext.getActualEngine()),
	  scenes(stepContext.getActualSceneManager()),
	  inputs(stepContext.getActualInputManager()),
	  currentScene(currentScene)
{
	;
}

ssge::SceneStepContext::Engine::Engine(ssge::Engine* actual)
{
	this->actual = actual;
}

void ssge::SceneStepContext::Engine::finish()
{
	actual->finish();
}

ssge::SceneStepContext::Scenes::Scenes(ssge::SceneManager* actual)
{
	this->actual = actual;
}

void ssge::SceneStepContext::Scenes::changeScene(std::unique_ptr<ssge::Scene> newScene)
{
	actual->changeScene(std::move(newScene));
}

ssge::SceneStepContext::Inputs::Inputs(ssge::InputManager* actual)
{
	this->actual = actual;
}

ssge::Engine* ssge::SceneStepContext::getActualEngine()
{
	return engine.actual;
}

ssge::SceneManager* ssge::SceneStepContext::getActualSceneManager()
{
	return scenes.actual;
}

ssge::InputManager* ssge::SceneStepContext::getActualInputManager()
{
	return inputs.actual;
}

// GameWorldStepContext impl

ssge::GameWorldStepContext::GameWorldStepContext(ssge::GameWorld& currentGameWorld, ssge::SceneStepContext& sceneStepContext)
	: engine(sceneStepContext.getActualEngine()),
	scenes(sceneStepContext.getActualSceneManager()),
	inputs(sceneStepContext.getActualInputManager()),
	currentScene(currentGameWorld.getAsScene()),
	world(currentGameWorld)
{
	;
}

ssge::GameWorldStepContext::Engine::Engine(ssge::Engine* actual)
{
	this->actual = actual;
}

void ssge::GameWorldStepContext::Engine::finish()
{
	actual->finish();
}

ssge::GameWorldStepContext::Scenes::Scenes(ssge::SceneManager* actual)
{
	this->actual = actual;
}

void ssge::GameWorldStepContext::Scenes::changeScene(std::unique_ptr<ssge::Scene> newScene)
{
	actual->changeScene(std::move(newScene));
}

ssge::GameWorldStepContext::Inputs::Inputs(ssge::InputManager* actual)
{
	this->actual = actual;
}

ssge::Engine* ssge::GameWorldStepContext::getActualEngine()
{
	return engine.actual;
}

ssge::SceneManager* ssge::GameWorldStepContext::getActualSceneManager()
{
	return scenes.actual;
}

ssge::InputManager* ssge::GameWorldStepContext::getActualInputManager()
{
	return inputs.actual;
}

