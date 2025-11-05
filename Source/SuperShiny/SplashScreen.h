#pragma once
#include "../ssge/Scene.h"
#include "SDL.h"

using namespace ssge;

namespace ssge
{
	class SplashScreen : public Scene
	{
		SDL_Texture* backgroundTexture;
		// Inherited via Scene
		SceneClassID getSceneClassID() const override;
		void init(SceneStepContext& context) override;
		void step(SceneStepContext& context) override;
		void draw(DrawContext& context) override;
	public:
		SplashScreen();
		~SplashScreen();
	};
}