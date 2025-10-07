#include "StepContext.h"

// We include engine headers here — only the .cpp sees the raw types.
// Headers remain free of raw public Program* signatures.
#include "Engine.h"
#include "SceneManager.h"
#include "Scene.h"
#include "GameWorld.h"
#include "EntityManager.h"
#include "InputManager.h"
#include "WindowManager.h"

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

void ssge::StepContext::Engine::wrapUp()
{
	actual->wrapUp();
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

bool ssge::StepContext::Inputs::isPressed(int buttonIndex)
{
	return this->actual->getPad().checkPressed(1 << buttonIndex);
}

bool ssge::StepContext::Inputs::isJustPressed(int buttonIndex)
{
	return this->actual->getPad().checkJustPressed(1 << buttonIndex);
}

bool ssge::StepContext::Inputs::isJustReleased(int buttonIndex)
{
	return this->actual->getPad().checkJustReleased(1 << buttonIndex);
}

ssge::StepContext::Drawing::Drawing(ssge::WindowManager* windowManager)
{
	this->windowManager = windowManager;
}

SDL_Renderer* ssge::StepContext::Drawing::getRenderer() const
{
	return windowManager->getRenderer();
}

double ssge::StepContext::getDeltaTime()
{
	return deltaTime;
}

ssge::StepContext::StepContext(ssge::Engine* actualEngine, double deltaTime)
	: engine(actualEngine),
	  scenes(actualEngine->getSceneManager()),
	  inputs(actualEngine->getInputManager()),
	  drawing(actualEngine->getWindowManager())
{
	this->deltaTime = deltaTime;
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

ssge::WindowManager* ssge::StepContext::getActualWindowManager()
{
	return drawing.windowManager;
}

// SceneStepContext impl

double ssge::SceneStepContext::getDeltaTime()
{
	return deltaTime;
}

ssge::SceneStepContext::SceneStepContext(ssge::Scene& currentScene, ssge::StepContext& stepContext)
	: engine(stepContext.getActualEngine()),
	  scenes(stepContext.getActualSceneManager()),
	  inputs(stepContext.getActualInputManager()),
	  drawing(stepContext.getActualWindowManager()),
	  currentScene(currentScene)
{
	this->deltaTime = stepContext.getDeltaTime();
}

ssge::SceneStepContext::Engine::Engine(ssge::Engine* actual)
{
	this->actual = actual;
}

void ssge::SceneStepContext::Engine::finish()
{
	actual->finish();
}

void ssge::SceneStepContext::Engine::wrapUp()
{
	actual->wrapUp();
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

bool ssge::SceneStepContext::Inputs::isPressed(int buttonIndex)
{
	return this->actual->getPad().checkPressed(1 << buttonIndex);
}

bool ssge::SceneStepContext::Inputs::isJustPressed(int buttonIndex)
{
	return this->actual->getPad().checkJustPressed(1 << buttonIndex);
}

bool ssge::SceneStepContext::Inputs::isJustReleased(int buttonIndex)
{
	return this->actual->getPad().checkJustReleased(1 << buttonIndex);
}

ssge::SceneStepContext::Drawing::Drawing(ssge::WindowManager* windowManager)
{
	this->windowManager = windowManager;
}

SDL_Renderer* ssge::SceneStepContext::Drawing::getRenderer() const
{
	return windowManager->getRenderer();
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

ssge::WindowManager* ssge::SceneStepContext::getActualWindowManager()
{
	return drawing.windowManager;
}

// GameWorldStepContext impl

double ssge::GameWorldStepContext::getDeltaTime()
{
	return deltaTime;
}

ssge::GameWorldStepContext::GameWorldStepContext(ssge::GameWorld& currentGameWorld, ssge::SceneStepContext& sceneStepContext)
	: engine(sceneStepContext.getActualEngine()),
	scenes(sceneStepContext.getActualSceneManager()),
	inputs(sceneStepContext.getActualInputManager()),
	drawing(sceneStepContext.getActualWindowManager()),
	currentScene(&currentGameWorld.getAsScene()),
	world(&currentGameWorld)
{
	this->deltaTime = sceneStepContext.getDeltaTime();
}

ssge::GameWorldStepContext::Engine::Engine(ssge::Engine* actual)
{
	this->actual = actual;
}

void ssge::GameWorldStepContext::Engine::finish()
{
	actual->finish();
}

void ssge::GameWorldStepContext::Engine::wrapUp()
{
	actual->wrapUp();
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

bool ssge::GameWorldStepContext::Inputs::isPressed(int buttonIndex)
{
	return this->actual->getPad().checkPressed(1 << buttonIndex);
}

bool ssge::GameWorldStepContext::Inputs::isJustPressed(int buttonIndex)
{
	return this->actual->getPad().checkJustPressed(1 << buttonIndex);
}

bool ssge::GameWorldStepContext::Inputs::isJustReleased(int buttonIndex)
{
	return this->actual->getPad().checkJustReleased(1 << buttonIndex);
}

ssge::GameWorldStepContext::Drawing::Drawing(ssge::WindowManager* windowManager)
{
	this->windowManager = windowManager;
}

SDL_Renderer* ssge::GameWorldStepContext::Drawing::getRenderer() const
{
	return windowManager->getRenderer();
}

ssge::GameWorldStepContext::Scene::Scene(ssge::Scene* actual) : actual(actual)
{
}

ssge::GameWorldStepContext::GameWorld::GameWorld(ssge::GameWorld* actual) : actual(actual)
{
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

ssge::WindowManager* ssge::GameWorldStepContext::getActualWindowManager()
{
	return drawing.windowManager;
}

ssge::GameWorld* ssge::GameWorldStepContext::getCurrentGameWorld()
{
	return world.actual;
}

ssge::Scene* ssge::GameWorldStepContext::getCurrentScene()
{
	return currentScene.actual;
}

// EntityStepContext impl

double ssge::EntityStepContext::getDeltaTime()
{
	return deltaTime;
}

ssge::EntityStepContext::EntityStepContext(ssge::GameWorldStepContext& gameWorldStepContext)
	: engine(gameWorldStepContext.getActualEngine()),
	scenes(gameWorldStepContext.getActualSceneManager()),
	inputs(gameWorldStepContext.getActualInputManager()),
	drawing(gameWorldStepContext.getActualWindowManager()),
	currentScene(gameWorldStepContext.getCurrentScene()),
	world(gameWorldStepContext.getCurrentGameWorld()),
	entities(&gameWorldStepContext.getCurrentGameWorld()->entities)
{
	this->deltaTime = gameWorldStepContext.getDeltaTime();
}

ssge::EntityStepContext::Engine::Engine(ssge::Engine* actual)
{
	this->actual = actual;
}

void ssge::EntityStepContext::Engine::finish()
{
	actual->finish();
}

void ssge::EntityStepContext::Engine::wrapUp()
{
	actual->wrapUp();
}

ssge::EntityStepContext::Scenes::Scenes(ssge::SceneManager* actual)
{
	this->actual = actual;
}

void ssge::EntityStepContext::Scenes::changeScene(std::unique_ptr<ssge::Scene> newScene)
{
	actual->changeScene(std::move(newScene));
}

ssge::EntityStepContext::Inputs::Inputs(ssge::InputManager* actual)
{
	this->actual = actual;
}

bool ssge::EntityStepContext::Inputs::isPressed(int buttonIndex)
{
	return this->actual->getPad().checkPressed(1 << buttonIndex);
}

bool ssge::EntityStepContext::Inputs::isJustPressed(int buttonIndex)
{
	return this->actual->getPad().checkJustPressed(1 << buttonIndex);
}

bool ssge::EntityStepContext::Inputs::isJustReleased(int buttonIndex)
{
	return this->actual->getPad().checkJustReleased(1 << buttonIndex);
}

ssge::EntityStepContext::Drawing::Drawing(ssge::WindowManager* windowManager)
{
	this->windowManager = windowManager;
}

SDL_Renderer* ssge::EntityStepContext::Drawing::getRenderer() const
{
	return windowManager->getRenderer();
}

ssge::EntityStepContext::Scene::Scene(ssge::Scene* actual) : actual(actual)
{
}

ssge::EntityStepContext::GameWorld::GameWorld(ssge::GameWorld* actual) : actual(actual)
{
}

ssge::EntityStepContext::EntityManager::EntityManager(ssge::EntityManager* actual) : actual(actual)
{
}

ssge::Engine* ssge::EntityStepContext::getActualEngine()
{
	return engine.actual;
}

ssge::SceneManager* ssge::EntityStepContext::getActualSceneManager()
{
	return scenes.actual;
}

ssge::InputManager* ssge::EntityStepContext::getActualInputManager()
{
	return inputs.actual;
}

ssge::WindowManager* ssge::EntityStepContext::getActualWindowManager()
{
	return drawing.windowManager;
}
