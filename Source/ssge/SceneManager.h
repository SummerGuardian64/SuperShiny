#pragma once
#include <vector>
#include <memory>
#include "Scene.h"
#include "PassKey.h"

namespace ssge
{
	class Engine;

	class StepContext;
	class DrawContext;

	class SceneManager // Manages Scenes
	{
		std::unique_ptr<Scene> currentScene;
		bool paused = false;
		bool wannaPause = false;
		bool sceneInitialized = false;
		std::unique_ptr<Scene> queuedScene;
		uint8_t fadeVal = 0;
		bool wannaWrapUp = false;

	public:
		SceneManager(PassKey<Engine> pk);
		SceneManager(const SceneManager& toCopy) = delete;
		SceneManager(SceneManager&& toMove) = delete;

		void step(StepContext& context);
		void draw(DrawContext& context);
		void wrapUp();

		Scene* getCurrentScene() const;
		SceneClassID getCurrentSceneClassID() const;
		Scene* changeScene(std::unique_ptr<Scene> newScene);
		bool isSceneInitialized() const;
		bool isPaused() const;
		uint8_t getFadeVal() const;
		bool isFadeFinished() const;
		void pause();
		void unpause();
		bool togglePause();
		void setPause(bool pause);
		void shutdown();
	};
}