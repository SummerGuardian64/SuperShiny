#include "Entity.h"
#include "StepContext.h"
#include "DrawContext.h"
#include <iostream>

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

ssge::Entity::Physics::Physics(Entity& entity) : entity(entity)
{
}

void ssge::Entity::Physics::step(EntityStepContext& context)
{
}

ssge::Entity::NPC::NPC(Entity& entity) : entity(entity)
{
}

void ssge::Entity::step(EntityStepContext& context)
{
	double deltaTime = context.getDeltaTime();

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

	if (sprite)
	{
		sprite->update(deltaTime);
	}

	// Increment lifespan
	//FIXME: THIS NEEDS REFINEMENT!
	lifespan += deltaTime * 60;

	std::cout << "Entity " << (void*)this << " has been there for " << lifespan << std::endl;
}

void ssge::Entity::draw(DrawContext context) const
{
	preDraw(context);

	if (sprite)
	{
		sprite->draw(context);
	}

	postDraw(context);
}

void ssge::Entity::destroy()
{
	scheduledToDestroy = true;
}

ssge::Entity::Control::Control(Entity& entity) : entity(entity)
{
}
