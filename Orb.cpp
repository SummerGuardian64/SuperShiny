#include "Orb.h"
#include "StepContext.h"
#include "DrawContext.h"
#include <iostream>
#include <memory>
#include "Game.h"

using namespace ssge;

Orb::Orb()
{
    sprite = std::make_unique<Sprite>(Game::Sprites::orb());
    physics = std::make_unique<Physics>(*this);
    control = std::make_unique<Control>(*this);

    control->mode = Entity::Control::Mode::NPC;

    position.x = 400;
    position.y = 100;

    physics->speed.x = 4;

    physics->processVelocity = true;
    physics->enablePhysics = true;
    physics->enableHorizontalBounce = true;
    physics->enableHorizontalMove = true;
    physics->enableVerticalBounce = true;
    physics->enableVerticalMove = true;
    physics->enableJump = true;
    physics->enableHorizontalCollision = true;
    physics->enableVerticalCollision = true;

    physics->gravity = 0.7;
    physics->gravityInWater = 0.1;

    physics->maxWalkSpeed = 8;
    physics->maxRunSpeed = 10;
    physics->maxAirWalkSpeed = 7;
    physics->maxAirRunSpeed = 4;
    physics->maxAirJumpSpeed = 7;
    physics->maxAirFallSpeed = 10;
    physics->maxWalkInWaterSpeed = 3;
    physics->maxRunInWaterSpeed = 4;
    physics->maxWaterJumpSpeed = 4;
    physics->maxWaterFallSpeed = 5;
    physics->maxSwimSpeed = 8;

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

EntityClassID Orb::getEntityClassID() const
{
    return EntityClassID::Orb;
}

void Orb::firstStep(EntityStepContext& context)
{
    std::cout << "BORB!" << std::endl;
}

void Orb::preStep(EntityStepContext& context)
{
}

void Orb::postStep(EntityStepContext& context)
{
}

void Orb::preDraw(DrawContext& context) const
{
}

void Orb::postDraw(DrawContext& context) const
{
}

void Orb::onDestroy(EntityStepContext& context)
{
}
