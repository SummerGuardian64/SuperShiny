#include "DrawContext.h"

using namespace ssge;

ssge::DrawContext::DrawContext(SDL_Renderer* const renderer)
	:
	renderer(renderer),
	bounds({0,0,0,0}),
	origin({0,0}),
	deltaTime(0),
	renderTarget(nullptr)
{
	// Set bounds to match the render texture size
	SDL_Texture* renderTexture = SDL_GetRenderTarget(renderer);
	if (renderTexture)
	{
		SDL_QueryTexture(renderTexture, nullptr, nullptr, &bounds.w, &bounds.h);
	}
}

DrawContext::DrawContext(
	SDL_Renderer* const renderer,
	const SDL_Rect bounds,
	const SDL_Point origin,
	const double deltaTime,
	SDL_Texture* const renderTarget
	) :
	renderer(renderer),
	bounds(bounds),
	origin(origin),
	deltaTime(deltaTime),
	renderTarget(renderTarget)
{ }

SDL_Renderer* DrawContext::getRenderer() const
{
	return renderer;
}

SDL_Rect ssge::DrawContext::getBounds() const
{
	return bounds;
}

SDL_Point ssge::DrawContext::getOrigin() const
{
	return origin;
}

double ssge::DrawContext::getDeltaTime() const
{
	return deltaTime;
}

void ssge::DrawContext::applyTarget() const
{
	SDL_SetRenderTarget(renderer, renderTarget);
}

void ssge::DrawContext::commitFromSubContext(const DrawContext& subContext, const SDL_Point where)
{
	// Nothing to commit if renderTarget is null
	if (subContext.renderTarget == nullptr)
		return;

	// We take the sub-context's bounds as the source
	SDL_Rect srcRect = subContext.bounds;

	// We take the where-point as the top-left corner of the destination
	// and offset it with the sub-context's origin.
	// Sub-context's bound size determines the size of both source and destination.
	SDL_Rect dstRect = {
		where.x - subContext.origin.x, // X
		where.y - subContext.origin.y, // Y
		subContext.bounds.w, // Width
		subContext.bounds.h // Height
	};

	// Commit
	SDL_RenderCopy(
		renderer,
		subContext.renderTarget,
		&srcRect,
		&dstRect
	);
}
