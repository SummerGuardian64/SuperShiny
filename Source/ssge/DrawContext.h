#pragma once
#include "SDL.h"
#include "SDL_ttf.h"

namespace ssge
{
	class DrawContext
	{
		SDL_Renderer* renderer = nullptr;
		SDL_Rect bounds = { 0,0 };
		SDL_Point origin = { 0,0 };
		double deltaTime = 0;
		SDL_Point scrollOffset = { 0,0 };
		SDL_Texture* renderTarget = nullptr;
		TTF_Font* font = nullptr;

	public:
		DrawContext(SDL_Renderer* const renderer);
		DrawContext(SDL_Renderer* const renderer,
			int virtualWidth,
			int virtualHeight);
		DrawContext(SDL_Renderer* const renderer,
			const SDL_Rect bounds,
			const SDL_Point origin,
			const double deltaTime,
			SDL_Texture* const renderTarget = nullptr,
			TTF_Font* font = nullptr
			);
		DrawContext(const DrawContext& toCopy);
		SDL_Renderer* getRenderer() const;
		SDL_Rect getBounds() const;
		SDL_Point getOrigin() const;
		double getDeltaTime() const;
		SDL_Point getScrollOffset() const;
		SDL_Point calculateAnchorPoint() const;
		TTF_Font* getFont();

		// Context derivation

		DrawContext clone() const;
		DrawContext deriveForScrolling(SDL_Point offset) const;
		DrawContext deriveForEntity(SDL_FPoint entityPosition) const;
		DrawContext deriveWithFont(TTF_Font* font) const;

		// SDL function help

		void applyTarget() const;
		void commitFromSubContext(const DrawContext& subContext, const SDL_Point where);


	};
}