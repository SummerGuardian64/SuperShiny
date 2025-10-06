#pragma once
#include "SDL.h"

namespace ssge
{
	class DrawContext
	{
		SDL_Renderer* const renderer;
		SDL_Rect bounds;
		const SDL_Point origin;
		const double deltaTime;
		SDL_Point scrollOffset;
		SDL_Texture* const renderTarget;

	public:
		DrawContext(SDL_Renderer* const renderer);
		DrawContext(SDL_Renderer* const renderer,
			const SDL_Rect bounds,
			const SDL_Point origin,
			const double deltaTime,
			SDL_Texture* const renderTarget = nullptr
			);
		DrawContext(const DrawContext& toCopy);
		SDL_Renderer* getRenderer() const;
		SDL_Rect getBounds() const;
		SDL_Point getOrigin() const;
		double getDeltaTime() const;
		SDL_Point getScrollOffset() const;

		// Context derivation

		DrawContext clone() const;
		DrawContext deriveForScrolling(SDL_Point offset) const;

		// SDL function help

		void applyTarget() const;
		void commitFromSubContext(const DrawContext& subContext, const SDL_Point where);


	};
}