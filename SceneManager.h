#pragma once
#include "SuperShiny.h"
#include <vector>
#include <memory>

namespace ssge
{
	class SceneManager // Manages Scenes
	{
		//TODO: Refactor to friend class Factory
		friend class Engine;
	private:
		std::unique_ptr<Scene> currentScene;
		bool paused;
		bool sceneInitialized;
		std::unique_ptr<Scene> queuedScene;
		uint8_t fadeVal;

		SceneManager();
		SceneManager(const SceneManager& toCopy) = delete;
		SceneManager(SceneManager&& toMove) = delete;
		~SceneManager();

	public:
		void step(StepContext& context);
		//void draw(DrawContext& context);

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