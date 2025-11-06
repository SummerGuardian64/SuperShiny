#include "TitleScreen.h"
#include "../ssge/StepContext.h"
#include <memory>
#include "../ssge/GameWorld.h"
#include "../ssge/DrawContext.h"
#include "../ssge/Scene.h"
#include "SDL.h"

ssge::SceneClassID TitleScreen::getSceneClassID() const
{
	return ssge::SceneClassID::TitleScreen;
}

TitleScreen::TitleScreen()
{
}

TitleScreen::~TitleScreen()
{
}

void TitleScreen::init(ssge::SceneStepContext& context)
{
	background = SdlTexture("Backgrounds/ShinyRuns_720p.png", context.drawing.getRenderer());
}

void TitleScreen::step(ssge::SceneStepContext& context)
{
	if (!offeredMenu && context.scenes.isFadeFinished())
	{
		context.menus.openMainMenu();
		offeredMenu = true;
	}
}

void TitleScreen::draw(ssge::DrawContext& context)
{
	SDL_Renderer* renderer = context.getRenderer();
	SDL_Rect screenRect = context.getBounds();

	SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
	SDL_RenderFillRect(renderer, &screenRect);

	if (background)
		SDL_RenderCopy(renderer, background, nullptr, nullptr);

}
