#include "Shiny.h"
#include "StepContext.h"
#include "DrawContext.h"
#include <iostream>

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

void Shiny::draw(DrawContext context) const
{
	std::cout << "Shiny has an artblock" << std::endl;
}

void Shiny::onDestroy(EntityStepContext& context)
{
	std::cout << "Scalefloof go poof!" << std::endl;
}
