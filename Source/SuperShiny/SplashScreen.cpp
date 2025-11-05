#include "SplashScreen.h"
#include "../ssge/StepContext.h"
#include <memory>
#include "../ssge/GameWorld.h"
#include "../ssge/DrawContext.h"

using namespace ssge;

SceneClassID SplashScreen::getSceneClassID() const
{
	return SceneClassID::SplashScreen;
}

SplashScreen::SplashScreen()
{
	backgroundTexture = nullptr;
}

SplashScreen::~SplashScreen()
{
	if (backgroundTexture)
		SDL_DestroyTexture(backgroundTexture);
}

void SplashScreen::init(SceneStepContext& context)
{
	SDL_Renderer* renderer = context.drawing.getRenderer();
	SDL_Surface* surface = SDL_LoadBMP("Splash.bmp");
	if (!surface) {
		printf("SDL_LoadBMP failed: %s\n", SDL_GetError());
	}
	else
	{
		SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
		SDL_FreeSurface(surface); // free the RAM copy, texture lives on the GPU now

		if (!texture) {
			printf("SDL_CreateTextureFromSurface failed: %s\n", SDL_GetError());
		}

		backgroundTexture = texture;
	}
}

void SplashScreen::step(SceneStepContext& context)
{
	if (context.inputs.getCurrentButtonsForPlayer(0) != 0)
	{
		context.scenes.changeScene(SceneClassID::TitleScreen);
	}
}

void SplashScreen::draw(DrawContext& context)
{
	SDL_Renderer* renderer = context.getRenderer();
	SDL_Rect screenRect = context.getBounds();
	
	SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
	SDL_RenderFillRect(renderer, &screenRect);

	if(backgroundTexture)
		SDL_RenderCopy(renderer, backgroundTexture, nullptr, nullptr);

}
