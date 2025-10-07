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
	physics->processVelocity = true;
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
	bool up = context.inputs.isPressed(0);
	bool down = context.inputs.isPressed(1);
	bool left = context.inputs.isPressed(2);
	bool right = context.inputs.isPressed(3);
	physics->velocity.y -= up * 2;
	physics->velocity.y += down * 2;
	physics->velocity.x -= left * 2;
	physics->velocity.x += right * 2;
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
