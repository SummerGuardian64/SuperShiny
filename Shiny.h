#pragma once
#include "Entity.h"

using namespace ssge;

class Shiny : public Entity
{
	// Inherited via Entity
	EntityClassID getEntityClassID() const override;
	void firstStep(EntityStepContext& context) override;
	void preStep(EntityStepContext& context) override;
	void postStep(EntityStepContext& context) override;
	void draw(DrawContext context) const override;
	void onDestroy(EntityStepContext& context) override;
};