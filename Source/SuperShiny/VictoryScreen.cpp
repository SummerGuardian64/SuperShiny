#include "VictoryScreen.h"
#include "../ssge/StepContext.h"
#include <memory>
#include "../ssge/GameWorld.h"
#include "../ssge/DrawContext.h"
#include "../ssge/Scene.h"
#include "SDL.h"

std::string VictoryScreen::getSceneClassID() const
{
	return "VictoryScreen";
}

VictoryScreen::VictoryScreen()
{
}

VictoryScreen::~VictoryScreen()
{
}

void VictoryScreen::init(SceneStepContext& context)
{
	background = SdlTexture("Backgrounds/Victory.png", context.drawing.getRenderer());
	context.audio.playMusicIfNotPlaying("Music/Victory.ogg",1);
}

void VictoryScreen::step(SceneStepContext& context)
{
	if(!context.audio.isMusicPlaying("Music/Victory.ogg"))
    {
        context.audio.playMusicIfNotPlaying("Music/ShinyTheme.ogg");
    }
}

void VictoryScreen::draw(DrawContext& context)
{
	SDL_Renderer* renderer = context.getRenderer();
	SDL_Rect screenRect = context.getBounds();

	SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
	SDL_RenderFillRect(renderer, &screenRect);

	if (background)
		SDL_RenderCopy(renderer, background, nullptr, nullptr);

}
