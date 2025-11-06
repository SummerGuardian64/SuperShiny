#include "Orb.h"
#include "../ssge/StepContext.h"
#include "../ssge/DrawContext.h"
#include <iostream>
#include <memory>
#include "../ssge/Game.h"

using namespace ssge;

Orb::Orb()
{
    //sprite = std::make_unique<Sprite>(Game::Sprites::orb());
    physics = std::make_unique<ssge::Entity::Physics>(*this);
    control = std::make_unique<ssge::Entity::Control>(*this);

    control->mode = ssge::Entity::Control::Mode::NPC;

    position.x = 400;
    position.y = 100;

    physics->velocity.x = 4;

    using Ability = ssge::Entity::Physics::Abilities::Flag;
    auto& abilities = physics->abilities;
    abilities.set(Ability::EnablePhysics);
    abilities.set(Ability::EnableHorizontalBounce);
    abilities.set(Ability::EnableHorizontalMove);
    abilities.set(Ability::EnableVerticalMove);
    abilities.set(Ability::EnableVerticalBounce);
    abilities.set(Ability::EnableJump);
    abilities.set(Ability::EnableHorizontalCollision);
    abilities.set(Ability::EnableVerticalCollision);

    abilities.maxSpeedHor = 7;
    abilities.maxSpeedUp = 10;
    abilities.maxSpeedDown = 10;
    abilities.acc = { 0.0,0.5 };
    abilities.dec = { 0.0,0.5 };

    abilities.jumpSpeed = 10;
    abilities.jumpStrength = 13;
    abilities.swimPower = 5;
    abilities.gravity = 0.7;

    //physics->gravityInWater = 0.1;

    //physics->maxWalkSpeed = 8;
    //physics->maxRunSpeed = 10;
    //physics->maxAirWalkSpeed = 7;
    //physics->maxAirRunSpeed = 4;
    //physics->maxAirJumpSpeed = 7;
    //physics->maxAirFallSpeed = 10;
    //physics->maxWalkInWaterSpeed = 3;
    //physics->maxRunInWaterSpeed = 4;
    //physics->maxWaterJumpSpeed = 4;
    //physics->maxWaterFallSpeed = 5;
    //physics->maxSwimSpeed = 8;

    //physics->accWalk.x = 0.5;
    //physics->accRun.x = 0.5;
    //physics->accAir.x = 0.3;
    //physics->accWater.x = 0.4;
    //physics->accWaterWalk.x = 0.3;
    //physics->accWaterRun.x = 0.35;

    //physics->decWalk.x = 0.5;
    //physics->decRun.x = 1;
    //physics->decAir.x = 0.25;
    //physics->decWater.x = 0.1;
    //physics->decWaterWalk.x = 0.5;
    //physics->decWaterRun.x = 0.5;

    //physics->accAir.y = 0.5;
    //physics->accAirRun.y = 0.75;
    //physics->accWater.y = 0.75;
    //physics->accWaterRun.y = 1;

    //physics->decAir.y = 0.75;
    //physics->decAirRun.y = 1;
    //physics->decWater.y = 0.5;
    //physics->decWaterRun.y = 0.8;

    hitbox.x = -32;
    hitbox.y = -32;
    hitbox.w = 64;
    hitbox.h = 64;
}

ssge::EntityClassID Orb::getEntityClassID() const
{
    return EntityClassID::Orb;
}

void Orb::firstStep(ssge::EntityStepContext& context)
{
    sprite = context.sprites.create("Orb");
    //std::cout << "BORB!" << std::endl;
}

void Orb::preStep(ssge::EntityStepContext& context)
{
}

void Orb::postStep(ssge::EntityStepContext& context)
{
    bool shouldWeBounce = false;

    if (physics)
    {
        if (!physics->abilities.collisionIgnored())
        {
            // Overal collisions
            {
                SDL_FRect collider{
                    position.x + hitbox.x - 1 - physics->oldVelocity.x,
                    position.y + hitbox.y - 1 - physics->oldVelocity.y,
                    hitbox.w + 2,
                    hitbox.h + 2
                };
                auto allCollisions = context.level.queryBlocksUnderCollider(collider);
                for (auto& collision : allCollisions)
                {
                    auto* block = context.level.getBlockAt(collision.coords);
                    if(collision.coll == ssge::Level::Block::Collision::Hazard)
                    {
                        shouldWeBounce = true;
                        block->type = 0;
                    }
                }
            }
        }
    }

    if (shouldWeBounce)
    {
        physics->velocity.x *= -1;
        physics->velocity.y *= -1;
    }
}

void Orb::preDraw(ssge::DrawContext& context) const
{
}

void Orb::postDraw(ssge::DrawContext& context) const
{
}

void Orb::onDestroy(ssge::EntityStepContext& context)
{
}
