#pragma once
#include "Entity.h"

using namespace ssge;

class Shiny : public Entity
{
public:
	Shiny();
	// Inherited via Entity
	EntityClassID getEntityClassID() const override;
	void firstStep(EntityStepContext& context) override;
	void preStep(EntityStepContext& context) override;
	void postStep(EntityStepContext& context) override;
	void preDraw(DrawContext& context) const override;
	void postDraw(DrawContext& context) const override;
	void onDestroy(EntityStepContext& context) override;
};