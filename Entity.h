#pragma once
#include "SDL.h"
#include <chrono>
#include "Sprite.h"

namespace ssge
{
	enum class EntityClassID {
		UNKNOWN,
		Shiny, // Currently the only entity we have in the game
		TOTAL
	};

	class EntityStepContext;
	class DrawContext;

	class Entity
	{
		uint32_t lifespan{ 0 };
		bool scheduledToDestroy{ false };
	public:
		Entity();
		virtual EntityClassID getEntityClassID() const = 0;
		std::unique_ptr<Sprite> sprite;
		SDL_FPoint position{ 0.0f, 0.0f };
		SDL_FPoint velocity{ 0.0f, 0.0f };
		bool processVelocity{ true };
		SDL_FPoint setVelocityMagnitude(const SDL_FPoint& v, float speed);
		//std::unique_ptr<Collider> collider;
		SDL_FRect rect{ 0.0f, 0.0f, 0.0f, 0.0f };
		SDL_Color color{ 255, 255, 255, 255 };
		uint32_t getLifespan() const { return lifespan; }
		float getLeftEdge() const { return rect.x; }
		float getRightEdge() const { return rect.x + rect.w; }
		float getTopEdge() const { return rect.y; }
		float getBottomEdge() const { return rect.y + rect.h; }
		float getHorizontalCenter() const { return rect.x + rect.w / 2.0f; }
		float getVerticalCenter() const { return rect.y + rect.h / 2.0f; }
		SDL_FRect getBounds() const { return rect; }
		SDL_FPoint getSize() const { return SDL_FPoint{ rect.w, rect.h }; }
		float getDistance(const Entity& other) const;
		//std::shared_ptr<Collision> collideWith(Entity& other);
		virtual void firstStep(EntityStepContext& context) = 0;
		virtual void preStep(EntityStepContext& context) = 0;
		void step(EntityStepContext& context);
		bool isScheduledToDestroy() const { return scheduledToDestroy; }
		virtual void postStep(EntityStepContext& context) = 0;
		virtual void preDraw(DrawContext& context) const = 0;
		void draw(DrawContext context) const;
		virtual void postDraw(DrawContext& context) const = 0;
		void destroy();
		virtual void onDestroy(EntityStepContext& context) = 0;
		virtual ~Entity() = default;
	};
}