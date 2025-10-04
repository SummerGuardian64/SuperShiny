#include "Shiny.h"
#include "StepContext.h"
#include "DrawContext.h"

EntityClassID Shiny::getEntityClassID() const
{
	return EntityClassID::Shiny;
}

void Shiny::firstStep(EntityStepContext& context)
{
}

void Shiny::preStep(EntityStepContext& context)
{
}

void Shiny::postStep(EntityStepContext& context)
{
}

void Shiny::draw(DrawContext context) const
{
}

void Shiny::onDestroy(EntityStepContext& context)
{
}
