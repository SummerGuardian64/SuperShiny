#pragma once
#include "../ssge/Scene.h"
#include "SDL.h"
#include "../ssge/SdlTexture.h"

using namespace ssge;

class SplashScreen : public Scene
{
	SdlTexture background;
	// Inherited via Scene
	std::string getSceneClassID() const override;
	void init(SceneStepContext& context) override;
	void step(SceneStepContext& context) override;
	void draw(DrawContext& context) override;
public:
	SplashScreen();
	~SplashScreen();
};
