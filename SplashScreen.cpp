#include "SplashScreen.h"
#include "StepContext.h"

SceneClassID ssge::SplashScreen::getSceneClassID() const
{
	return SceneClassID::SplashScreen;
}

void ssge::SplashScreen::init(ssge::SceneStepContext& context)
{
}

void ssge::SplashScreen::step(ssge::SceneStepContext& context)
{
	if (context.inputs.isJustPressed(0))
	{
		context.scenes.changeScene(std::make_unique<GameWorld>());
	}
}

void ssge::SplashScreen::draw(ssge::DrawContext& context)
{
	SDL_Renderer* renderer = context.getRenderer();
	SDL_Rect screenRect = context.getBounds();

	SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
	SDL_RenderFillRect(renderer, &screenRect);
}
