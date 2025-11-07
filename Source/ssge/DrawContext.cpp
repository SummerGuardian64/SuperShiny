#include "DrawContext.h"

using namespace ssge;

DrawContext::DrawContext(SDL_Renderer* const renderer)
	:
	renderer(renderer),
	bounds({0,0,0,0}),
	origin({0,0}),
	deltaTime(0),
	scrollOffset({0,0}),
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
	SDL_Texture* const renderTarget,
	TTF_Font* font
	) :
	renderer(renderer),
	bounds(bounds),
	origin(origin),
	deltaTime(deltaTime),
	renderTarget(renderTarget),
	font(font)
{ }

DrawContext::DrawContext(const DrawContext& toCopy)
	:
	renderer(toCopy.renderer),
	bounds(toCopy.bounds),
	origin(toCopy.origin),
	deltaTime(toCopy.deltaTime),
	scrollOffset(toCopy.scrollOffset),
	renderTarget(toCopy.renderTarget),
	font(toCopy.font)
{ }

SDL_Renderer* DrawContext::getRenderer() const
{
	return renderer;
}

SDL_Rect DrawContext::getBounds() const
{
	return bounds;
}

SDL_Point DrawContext::getOrigin() const
{
	return origin;
}

double DrawContext::getDeltaTime() const
{
	return deltaTime;
}

SDL_Point DrawContext::getScrollOffset() const
{
	return scrollOffset;
}

SDL_Point DrawContext::calculateAnchorPoint() const
{
	SDL_Point anchor = this->origin;
	anchor.x -= scrollOffset.x;
	anchor.y -= scrollOffset.y;
	return anchor;
}

TTF_Font* ssge::DrawContext::getFont()
{
	return font;
}

DrawContext DrawContext::clone() const
{
	return DrawContext(*this);
}

DrawContext DrawContext::deriveForScrolling(SDL_Point offset) const
{
	DrawContext derivedContext = DrawContext(*this);

	derivedContext.scrollOffset = offset;

	return derivedContext;
}

DrawContext DrawContext::deriveForEntity(SDL_FPoint entityPosition) const
{
	DrawContext derivedContext = DrawContext(*this);

	derivedContext.origin = SDL_Point{ (int)entityPosition.x,(int)entityPosition.y };

	return derivedContext;
}

DrawContext ssge::DrawContext::deriveWithFont(TTF_Font* font) const
{
	DrawContext derivedContext = DrawContext(*this);

	derivedContext.font = font;

	return derivedContext;
}

void DrawContext::applyTarget() const
{
	SDL_SetRenderTarget(renderer, renderTarget);
}

void DrawContext::commitFromSubContext(const DrawContext& subContext, const SDL_Point where)
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
