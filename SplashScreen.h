#pragma once
#include "Scene.h"
#include "SDL.h"

using namespace ssge;

namespace ssge
{
	class SplashScreen : public Scene
	{
		SDL_Texture* backgroundTexture;
		// Inherited via Scene
		SceneClassID getSceneClassID() const override;
		void init(ssge::SceneStepContext& context) override;
		void step(ssge::SceneStepContext& context) override;
		void draw(ssge::DrawContext& context) override;
	public:
		SplashScreen();
		~SplashScreen();
	};
}