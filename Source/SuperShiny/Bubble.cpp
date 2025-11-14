#include "Bubble.h"
#include "../ssge/StepContext.h"
#include "../ssge/DrawContext.h"
#include <iostream>
#include <memory>
#include "../ssge/EntityManager.h"
#include "../ssge/Utilities.h"
#include "SDL.h"

void Bubble::pop()
{
    popped = true;
    physics->velocity.x = 0;
    physics->velocity.y = 0;
}

void Bubble::animate()
{
    if (sprite && physics)
    {
        int currentSequence = sprite->getSeqIdx();
        int wantedSequence = currentSequence;

        if (popped)
        {
            wantedSequence = (int)Sequences::Popped;
        }
        else
        {
            wantedSequence = (int)Sequences::Flying;
        }

        // Play wanted sequence. DON'T REPLAY!
        if (currentSequence != wantedSequence)
        {
            sprite->setSequence(wantedSequence);
        }
    }
}

Bubble::Bubble()
{
	physics = std::make_unique<ssge::Entity::Physics>(*this);
	control = std::make_unique<ssge::Entity::Control>(*this);
	control->mode = ssge::Entity::Control::Mode::NPC;
	auto& abilities = physics->abilities;
	using Ability = ssge::Entity::Physics::Abilities::Flag;
    abilities.set(Ability::EnablePhysics);
	abilities.set(Ability::EnableHorizontalCollision);
	abilities.set(Ability::EnableVerticalCollision);
	abilities.maxSpeedHor = 32.f;
	abilities.maxSpeedUp = 32.f;
	abilities.maxSpeedDown = 32.f;
    hitbox.x = -8.f;
    hitbox.y = -8.f;
    hitbox.w = 16.f;
    hitbox.h = 16.f;
}

std::string Bubble::getEntityClassID() const
{
	return "Bubble";
}

void Bubble::firstStep(ssge::EntityStepContext& context)
{
	sprite = context.sprites.create("Bubble");
}

void Bubble::preStep(ssge::EntityStepContext& context)
{
}

void Bubble::postStep(ssge::EntityStepContext& context)
{
    if (physics)
    {
        if (!physics->abilities.collisionIgnored())
        {
            if (!popped)
            {
                bool poppedThisFrame = false;

                // Lifespan too long
                if (getLifespan() > 60 * 10)
                    destroy();

                if (physics->velocity.x == 0 && physics->velocity.y == 0)
                {
                    pop();
                }

                SDL_FPoint velocity = physics->velocity;
                bool goingLeft = velocity.x < 0;
                bool goingRight = velocity.x > 0;
                bool goingUp = velocity.y < 0;
                bool goingDown = velocity.y > 0;
                // Overal collisions
                {
                    SDL_Rect bounds = context.level.calculateLevelSize();
                    
                    SDL_FRect collider{
                        position.x + hitbox.x - /*(goingLeft ? 1 : 0)*/1,
                        position.y + hitbox.y - /*(goingUp ? 1 : 0)*/1,
                        hitbox.w + /*(goingLeft ? 1 : 0)*/1 + /*(goingRight ? 1 : 0)*/1,
                        hitbox.h + /*(goingDown ? 1 : 0)*/1 + /*(goingUp ? 1 : 0)*/1
                    };

                    // Out of bounds - DESTROY!
                    if (collider.x + collider.w < bounds.x
                        || collider.x > bounds.x + bounds.w
                        || collider.y + collider.h < bounds.y
                        || collider.y > bounds.y + bounds.h)
                    {
                        this->destroy();
                    }

                    auto allCollisions = context.level.queryBlocksUnderCollider(collider);
                    for (auto& collision : allCollisions)
                    {
                        auto* block = context.level.getBlockAt(collision.coords);

                        using Collision = ssge::Level::Block::Collision;

                        switch (collision.coll)
                        {
                        case Collision::Hazard:
                            if (block)
                            {
                                block->type = 0;
                            }
                            [[fallthrough]];
                        case Collision::Solid:
                        case Collision::DeathOnTouch:
                        case Collision::DeathIfFullyOutside:
                            poppedThisFrame = true;
                            break;
                        default:
                            break;
                        }

                        std::string callback = collision.callback;

                        if (callback.substr(0, 9) == "Bubblable")
                        {
                            auto rightArrowIndex = callback.find('>');

                            // Coordinates of the block under Shiny's clawbs
                            auto colSpot = collision.coords;

                            // Terraform current block (Terraform->X or Terraform^Y->X)
                            if (rightArrowIndex != std::string::npos)
                            {
                                std::size_t start = rightArrowIndex + 1;
                                std::size_t end = callback.size();

                                try
                                {
                                    std::string numberSubstring = callback.substr(start, end - start);
                                    int newType = std::stoi(numberSubstring);
                                    if (block)
                                    {
                                        block->type = newType;
                                    }
                                }
                                catch (...)
                                {
                                    // Ignore malformed number, do nothing
                                }
                            }
                        }
                    }
                }

                if(poppedThisFrame)
                    pop();
            }
            else
            {
                if (sprite->isFinished())
                    destroy();
            }
        }
    }

    animate();
}

void Bubble::preDraw(ssge::DrawContext& context) const
{
}

void Bubble::postDraw(ssge::DrawContext& context) const
{
}

void Bubble::onDestroy(ssge::EntityStepContext& context)
{
}
