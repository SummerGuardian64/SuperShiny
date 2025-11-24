#include "SceneManager.h"
#include <iostream>
#include <algorithm>
#include "SDL.h"
#include "StepContext.h"
#include "DrawContext.h"
#include "Accessor.h"

using namespace ssge;

void SceneManager::step(StepContext& context)
{
	paused = wannaPause;

	if (auto scene = getCurrentScene())
	{
		if (!isSceneInitialized())
		{
			SceneStepContext sceneStepContext(
				PassKey<SceneManager>(),
				context.deltaTime,
				context.engine,
				context.game,
				context.audio,
				context.scenes,
				context.inputs.accessDowngrade(),
				context.drawing,
				context.menus,
				CurrentSceneAccess(scene)
			);
			scene->init(sceneStepContext);
			sceneInitialized = true;
		}
		if (!isPaused() && !queuedScene)
		{
			SceneStepContext sceneStepContext(
				PassKey<SceneManager>(),
				context.deltaTime,
				context.engine,
				context.game,
				context.audio,
				context.scenes,
				context.inputs.accessDowngrade(),
				context.drawing,
				context.menus,
				CurrentSceneAccess(scene)
			);
			scene->step(sceneStepContext);
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
			queuedScene = nullptr;
			sceneInitialized = false;
			paused = false; // New scenes shouldn't start paused!
			wannaPause = false;
		}
	}
	else if (fadeVal > 0)
	{ // Then fade into the new scene
		fadeVal = std::clamp(fadeVal - 10, 0, 255);
	}
}

void SceneManager::draw(DrawContext& context) const
{
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

	SDL_Rect bounds = context.getBounds();
	SDL_RenderFillRect(renderer, &bounds);
}

void SceneManager::wrapUp()
{
	wannaWrapUp = true;
}

Scene* SceneManager::getCurrentScene() const
{
	return currentScene.get();
}

std::string SceneManager::getCurrentSceneClassID() const
{
	if (!currentScene)
	{
		return "";
	}
	else return currentScene->getSceneClassID();
}

Scene* SceneManager::changeScene(std::unique_ptr<Scene> newScene)
{
	if (Scene* validScene = newScene.get())
	{
		queuedScene = std::move(newScene);
		return validScene;
	}
	else
	{
		return nullptr;
	}
}

bool SceneManager::isSceneInitialized() const
{
	return sceneInitialized;
}

bool SceneManager::isPaused() const
{
	return paused;
}

uint8_t SceneManager::getFadeVal() const
{
	return fadeVal;
}

bool SceneManager::isFadeFinished() const
{
	return (fadeVal == 0);
}

void SceneManager::pause()
{
	wannaPause = true;
}

void SceneManager::unpause()
{
	wannaPause = false;
}

bool SceneManager::togglePause()
{
	isPaused() ? unpause() : pause();
	return isPaused();
}

void SceneManager::setPause(bool pause)
{
	wannaPause = pause;
}

void SceneManager::shutdown()
{
	currentScene = nullptr;
	queuedScene = nullptr;
}
