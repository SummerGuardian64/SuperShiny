#pragma once
#include "../ssge/Scene.h"
#include "SDL.h"

class SplashScreen : public ssge::Scene
{
	SDL_Texture* backgroundTexture;
	// Inherited via Scene
	std::string getSceneClassID() const override;
	void init(ssge::SceneStepContext& context) override;
	void step(ssge::SceneStepContext& context) override;
	void draw(ssge::DrawContext& context) override;
public:
	SplashScreen();
	~SplashScreen();
};