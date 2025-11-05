#pragma once
#include "../ssge/Scene.h"
#include "../ssge/SdlTexture.h"

using namespace ssge;

namespace ssge
{
	class TitleScreen : public Scene
	{
		SdlTexture background;
		bool offeredMenu = false;

		// Inherited via Scene
		SceneClassID getSceneClassID() const override;
		void init(SceneStepContext& context) override;
		void step(SceneStepContext& context) override;
		void draw(DrawContext& context) override;
	public:
		TitleScreen();
		~TitleScreen();
	};
}