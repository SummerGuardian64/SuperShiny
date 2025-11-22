#include "SplashScreen.h"
#include "../ssge/StepContext.h"
#include <memory>
#include "../ssge/GameWorld.h"
#include "../ssge/DrawContext.h"
#include "../ssge/Scene.h"
#include "SDL.h"

std::string SplashScreen::getSceneClassID() const
{
	return "SplashScreen";
}

SplashScreen::SplashScreen()
{
}

SplashScreen::~SplashScreen()
{
}

void SplashScreen::init(SceneStepContext& context)
{
	background = SdlTexture("Backgrounds/Splash.png", context.drawing.getRenderer());
}

void SplashScreen::step(SceneStepContext& context)
{
	if (context.inputs.getCurrentButtonsForPlayer(0) != 0)
	{
		context.scenes.changeScene("TitleScreen");
	}
}

void SplashScreen::draw(DrawContext& context)
{
	SDL_Renderer* renderer = context.getRenderer();
	SDL_Rect screenRect = context.getBounds();

	SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
	SDL_RenderFillRect(renderer, &screenRect);

	if (background)
		SDL_RenderCopy(renderer, background, nullptr, nullptr);

}
