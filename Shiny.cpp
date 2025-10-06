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
