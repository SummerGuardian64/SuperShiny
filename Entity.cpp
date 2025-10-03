#include "Entity.h"
#include "StepContext.h"
#include "DrawContext.h"

ssge::Entity::Entity()
{
}

SDL_FPoint ssge::Entity::setVelocityMagnitude(const SDL_FPoint& v, float speed)
{
	return SDL_FPoint();
}

float ssge::Entity::getDistance(const Entity& other) const
{
	//FIXME: UNIMPLEMENTED!
	return 0.0f;
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
	//FIXME: THIS NEEDS REFINEMENT!
	lifespan += context.getDeltaTime() * 60;
}

void ssge::Entity::draw(DrawContext context) const
{
}

void ssge::Entity::destroy()
{
	scheduledToDestroy = true;
}
