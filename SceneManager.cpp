#include "SceneManager.h"
#include <iostream>
#include <algorithm>

ssge::SceneManager::SceneManager()
{
	paused = false;
	sceneInitialized = false;
	fadeVal = 0;
	wannaWrapUp = false;
}

ssge::SceneManager::~SceneManager()
{
}

void ssge::SceneManager::step(StepContext& context)
{
	std::cout << "SceneManager::step()" << std::endl;
	if (auto scene = getCurrentScene())
	{
		if (!isSceneInitialized())
		{
			SceneStepContext sceneContext = Factory::ForSceneManager::sceneStepContext(*scene, context);
			scene->init(sceneContext);
			sceneInitialized = true;
		}
		if (!isPaused() && !queuedScene)
		{
			SceneStepContext sceneContext = Factory::ForSceneManager::sceneStepContext(*scene, context);
			scene->step(sceneContext);
		}
	}

	// Are we wrapping up for quitting the program?
	if (wannaWrapUp)
	{
		// Let's fade out of the current scene first
		if (fadeVal < 255)
		{
			fadeVal = std::clamp(fadeVal + 10, 0, 255);
		}
		else
		{
			// Now we can finish
			context.engine.finish();
		}
	}
	// If we've got a queued scene
	else if (queuedScene)
	{
		// If there is no current scene, just start from a fade-in
		if (!currentScene)
		{
			fadeVal = 255;
		}
		// Let's fade out of the current scene first
		if (fadeVal < 255)
		{
			fadeVal = std::clamp(fadeVal + 10, 0, 255);
		}
		else
		{ // Then switch to the new scene
			currentScene = std::move(queuedScene);
			queuedScene = NULL;
			sceneInitialized = false;
			paused = false; // New scenes shouldn't start paused!
		}
	}
	else if (fadeVal > 0)
	{ // Then fade into the new scene
		fadeVal = std::clamp(fadeVal - 10, 0, 255);
	}
}

void ssge::SceneManager::draw(DrawContext& context)
{
	std::cout << "SceneManager::draw()" << std::endl;
	// Draw current scene
	if (auto scene = getCurrentScene())
	{
		scene->draw(context);
	}

	SDL_Renderer* renderer = context.getRenderer();
	SDL_Color backgroundColor{ 0,0,0,fadeVal };

	// Draw background color
	SDL_SetRenderDrawColor(
		renderer,
		backgroundColor.r,
		backgroundColor.g,
		backgroundColor.b,
		backgroundColor.a
	);
	
	SDL_RenderFillRect(renderer, &context.getBounds());

	// Draw fader
	//TODO: Port to SDL2
	/*
	auto faderRect = sf::RectangleShape(game.getWindowSizeFloat());
	faderRect.setFillColor(sf::Color(0, 0, 0, fadeVal));
	renderTarget.draw(faderRect);
	*/
}

void ssge::SceneManager::wrapUp()
{
	wannaWrapUp = true;
}

ssge::Scene* ssge::SceneManager::getCurrentScene() const
{
	return currentScene.get();
}

ssge::SceneClassID ssge::SceneManager::getCurrentSceneClassID() const
{
	if (!currentScene)
	{
		return ssge::SceneClassID::None;
	}
	else return currentScene->getSceneClassID();
}

ssge::Scene* ssge::SceneManager::changeScene(std::unique_ptr<ssge::Scene> newScene)
{
	if (ssge::Scene* validScene = newScene.get())
	{
		queuedScene = std::move(newScene);
		return validScene;
	}
	else
	{
		return nullptr;
	}
}

bool ssge::SceneManager::isSceneInitialized() const
{
	return sceneInitialized;
}

bool ssge::SceneManager::isPaused() const
{
	return paused;
}

uint8_t ssge::SceneManager::getFadeVal() const
{
	return fadeVal;
}

bool ssge::SceneManager::isFadeFinished() const
{
	return (fadeVal == 0);
}

void ssge::SceneManager::pause()
{
	paused = true;
}

void ssge::SceneManager::unpause()
{
	paused = false;
}

bool ssge::SceneManager::togglePause()
{
	isPaused() ? unpause() : pause();
	return isPaused();
}

void ssge::SceneManager::setPause(bool pause)
{
	paused = pause;
}

void ssge::SceneManager::shutdown()
{
	currentScene = NULL;
	queuedScene = NULL;
}
