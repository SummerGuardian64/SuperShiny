#include "Shiny.h"
#include "StepContext.h"
#include "DrawContext.h"
#include <iostream>
#include <memory>
#include "Game.h"

using namespace ssge;

Shiny::Shiny()
{
	sprite = std::make_unique<Sprite>(Game::Sprites::shiny());
	physics = std::make_unique<Physics>(*this);

    position.x = 400;
    position.y = 100;

	physics->processVelocity = true;
	physics->enablePhysics = true;
	physics->enableHorizontalMove = true;
	physics->enableJump = true;
	physics->enableHorizontalCollision = true;
	physics->enableVerticalCollision = true;

    physics->jumpSpeed = 6.5;
    physics->jumpStrength = 8;
    physics->swimPower = 5;
    physics->gravity = 0.25;
    physics->gravityInWater = 0.1;

    physics->maxWalkSpeed = 3;
    physics->maxRunSpeed = 4;
    physics->maxAirWalkSpeed = 3;
    physics->maxAirRunSpeed = 4;
    physics->maxAirJumpSpeed = 7;
    physics->maxAirFallSpeed = 10;
    physics->maxWalkInWaterSpeed = 3;
    physics->maxRunInWaterSpeed = 4;
    physics->maxWaterJumpSpeed = 4;
    physics->maxWaterFallSpeed = 5;
    physics->maxSwimSpeed = 8;

    physics->accWalk.x = 0.2;
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
}

EntityClassID Shiny::getEntityClassID() const
{
	return EntityClassID::Shiny;
}

void Shiny::firstStep(EntityStepContext& context)
{
	std::cout << "Helloy!" << std::endl;
}

void Shiny::preStep(EntityStepContext& context)
{
	std::cout << "scale.. ";
	/*bool up = context.inputs.isPressed(0);
	bool down = context.inputs.isPressed(1);
	bool left = context.inputs.isPressed(2);
	bool right = context.inputs.isPressed(3);
	physics->velocity.y -= up * 2;
	physics->velocity.y += down * 2;
	physics->velocity.x -= left * 2;
	physics->velocity.x += right * 2;*/
}

void Shiny::postStep(EntityStepContext& context)
{
	std::cout << "...floof~" << std::endl;
}

void Shiny::preDraw(DrawContext& context) const
{
}

void Shiny::postDraw(DrawContext& context) const
{
}

void Shiny::onDestroy(EntityStepContext& context)
{
	std::cout << "Scalefloof go poof!" << std::endl;
}
