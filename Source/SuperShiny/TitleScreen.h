#pragma once
#include "../ssge/Scene.h"
#include "../ssge/SdlTexture.h"

class TitleScreen : public ssge::Scene
{
	SdlTexture background;

	// Inherited via Scene
	std::string getSceneClassID() const override;
	void init(ssge::SceneStepContext& context) override;
	void step(ssge::SceneStepContext& context) override;
	void draw(ssge::DrawContext& context) override;
public:
	TitleScreen();
	~TitleScreen();
};