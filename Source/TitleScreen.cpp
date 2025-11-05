#include "TitleScreen.h"
#include "StepContext.h"
#include <memory>
#include "GameWorld.h"
#include "DrawContext.h"

using namespace ssge;

SceneClassID TitleScreen::getSceneClassID() const
{
	return SceneClassID::TitleScreen;
}

TitleScreen::TitleScreen()
{
}

TitleScreen::~TitleScreen()
{
}

void TitleScreen::init(SceneStepContext& context)
{
	background = SdlTexture("Backgrounds/ShinyRuns_720p.png", context.drawing.getRenderer());
}

void TitleScreen::step(SceneStepContext& context)
{
	if (!offeredMenu && context.scenes.isFadeFinished())
	{
		context.menus.openMainMenu();
		offeredMenu = true;
	}
}

void TitleScreen::draw(DrawContext& context)
{
	SDL_Renderer* renderer = context.getRenderer();
	SDL_Rect screenRect = context.getBounds();

	SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
	SDL_RenderFillRect(renderer, &screenRect);

	if (background)
		SDL_RenderCopy(renderer, background, nullptr, nullptr);

}
