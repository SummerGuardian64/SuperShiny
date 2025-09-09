#pragma once
#include "SuperShiny.h"

namespace ssge
{
	class SceneManager // Manages Scenes
	{
		friend class Program;
	private:
		SceneManager();
		SceneManager(const SceneManager& toCopy) = delete;
		SceneManager(SceneManager&& toMove) = delete;
		~SceneManager();

		void step(StepContext& context);
		void draw(DrawContext& context);
	};
}