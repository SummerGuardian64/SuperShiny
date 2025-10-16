#include "SplashScreen.h"
#include "StepContext.h"
#include <memory>
#include "GameWorld.h"
#include "DrawContext.h"

SceneClassID ssge::SplashScreen::getSceneClassID() const
{
	return SceneClassID::SplashScreen;
}

ssge::SplashScreen::SplashScreen()
{
	backgroundTexture = nullptr;
}

ssge::SplashScreen::~SplashScreen()
{
	if (backgroundTexture)
		SDL_DestroyTexture(backgroundTexture);
}

void ssge::SplashScreen::init(ssge::SceneStepContext& context)
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

void ssge::SplashScreen::step(ssge::SceneStepContext& context)
{
	if (context.inputs.isJustPressed(4))
	{
		context.scenes.goToLevel(1);
	}
}

void ssge::SplashScreen::draw(ssge::DrawContext& context)
{
	SDL_Renderer* renderer = context.getRenderer();
	SDL_Rect screenRect = context.getBounds();
	
	SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
	SDL_RenderFillRect(renderer, &screenRect);

	if(backgroundTexture)
		SDL_RenderCopy(renderer, backgroundTexture, nullptr, nullptr);

}
