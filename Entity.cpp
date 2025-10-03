#include "Entity.h"

ssge::Entity::Entity()
{
}

SDL_FPoint ssge::Entity::setVelocityMagnitude(const SDL_FPoint& v, float speed)
{
	return SDL_FPoint();
}

void ssge::Entity::step(EntityStepContext& context)
{
	// If this is the entity's first step, execute it
	if (lifespan == 0)
	{
		firstStep(context);
	}

	// First, execute the entity-specific pre-step code
	preStep(context);

	// If we wanna process velocity
	if (processVelocity)
	{
		// Then execute the ordinary step
		SDL_FPoint tempVector = velocity;
		float tempScalar = context.getDeltaTime();
		tempVector.x *= tempScalar;
		tempVector.y *= tempScalar;
		position.x += tempVector.x;
		position.y += tempVector.y;
	}

	// Finally, execute the entity-specific post-step code
	postStep(context);

	// Increment lifespan
	lifespan += context.getDeltaTime();
}

void ssge::Entity::draw(DrawContext context) const
{
}

void ssge::Entity::destroy()
{
	scheduledToDestroy = true;
}
