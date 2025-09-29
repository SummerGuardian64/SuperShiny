#pragma once
#include "SuperShiny.h"

namespace ssge
{
	class DrawContext
	{
		SDL_Renderer* const renderer;
		SDL_Rect bounds;
		const SDL_Point origin;
		const double deltaTime;
		SDL_Texture* const renderTarget;

	public:
		DrawContext(SDL_Renderer* const renderer);
		DrawContext(SDL_Renderer* const renderer,
			const SDL_Rect bounds,
			const SDL_Point origin,
			const double deltaTime,
			SDL_Texture* const renderTarget = nullptr
			);
		SDL_Renderer* getRenderer() const;
		SDL_Rect getBounds() const;
		SDL_Point getOrigin() const;
		double getDeltaTime() const;

		// Context derivation



		// SDL function help

		void applyTarget() const;
		void commitFromSubContext(const DrawContext& subContext, const SDL_Point where);


	};
}