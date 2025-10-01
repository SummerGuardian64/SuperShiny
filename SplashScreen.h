#pragma once
#include "Scene.h"

using namespace ssge;

namespace ssge
{
	class SplashScreen : public Scene
	{
		// Inherited via Scene
		SceneClassID getSceneClassID() const override;
		void init(ssge::SceneStepContext& context) override;
		void step(ssge::SceneStepContext& context) override;
		void draw(ssge::DrawContext& context) override;
	};
}