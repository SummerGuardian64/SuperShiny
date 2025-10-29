#include "Shiny.h"
#include "StepContext.h"
#include "DrawContext.h"
#include <iostream>
#include <memory>
#include "Game.h"
#include "EntityManager.h"
#include "Utilities.h"

using namespace ssge;

Shiny::Shiny()
{
	sprite = std::make_unique<Sprite>(Game::Sprites::shiny());
	physics = std::make_unique<Physics>(*this);
    control = std::make_unique<Control>(*this);

    control->mode = Entity::Control::Mode::Player;
    control->playable = true;
    control->playerId = 0;

    position.x = 400;
    position.y = 100;

	physics->processVelocity = true;
	physics->enablePhysics = true;
	physics->enableHorizontalMove = true;
    //physics->enableVerticalBounce = true;
    //physics->enableVerticalMove = true;
	physics->enableJump = true;
	physics->enableHorizontalCollision = true;
	physics->enableVerticalCollision = true;

    physics->jumpSpeed = 10;
    physics->jumpStrength = 13;
    physics->swimPower = 5;
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

    physics->accWalk.x = 0.5;
    physics->accRun.x = 0.5;
    physics->accAir.x = 0.3;
    physics->accWater.x = 0.4;
    physics->accWaterWalk.x = 0.3;
    physics->accWaterRun.x = 0.35;

    physics->decWalk.x = 0.5;
    physics->decRun.x = 1;
    physics->decAir.x = 0.25;
    physics->decWater.x = 0.1;
    physics->decWaterWalk.x = 0.5;
    physics->decWaterRun.x = 0.5;
    
    physics->accAir.y = 0.5;
    physics->accAirRun.y = 0.75;
    physics->accWater.y = 0.75;
    physics->accWaterRun.y = 1;
    
    physics->decAir.y = 0.75;
    physics->decAirRun.y = 1;
    physics->decWater.y = 0.5;
    physics->decWaterRun.y = 0.8;

    hitbox.x = -15;
    hitbox.y = -86;
    hitbox.w = 30;
    hitbox.h = 86;
}

EntityClassID Shiny::getEntityClassID() const
{
	return EntityClassID::Shiny;
}

void Shiny::firstStep(EntityStepContext& context)
{
	//std::cout << "Helloy!" << std::endl;
}

void Shiny::preStep(EntityStepContext& context)
{
	//std::cout << "scale.. ";

    if (auto ctrl = control.get())
    {
        if (ctrl->getPad().isJustPressed(5))
        {
            if (auto orbRef = context.entities.addEntity(EntityClassID::Orb))
            {
                auto& orb = *orbRef.get();
                orb.position = position;
                orb.position.y -= 40;
                if(physics->speed.x<0)
                    orb.getPhysics()->speed.x *= -1;
            }
        }
    }
}

void Shiny::postStep(EntityStepContext& context)
{
	//std::cout << "...floof~" << std::endl;
    if (sprite && physics)
    {
        // Flip the sprite according to entity's facing direction (side)
        auto direction = physics->side.x;
        if (direction > 0) sprite->xscale = 1;
        else if (direction < 0) sprite->xscale = -1;
    }
}

void Shiny::preDraw(DrawContext& context) const
{
}

void Shiny::postDraw(DrawContext& context) const
{
}

void Shiny::onDestroy(EntityStepContext& context)
{
	//std::cout << "Scalefloof go poof!" << std::endl;
}
