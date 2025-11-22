#pragma once
#include "../ssge/Entity.h"

using namespace ssge;

class Orb : public Entity
{
public:
	Orb();
	// Inherited via Entity
	std::string getEntityClassID() const override;
	void firstStep(EntityStepContext& context) override;
	void preStep(EntityStepContext& context) override;
	void postStep(EntityStepContext& context) override;
	void preDraw(DrawContext& context) const override;
	void postDraw(DrawContext& context) const override;
	void onDestroy(EntityStepContext& context) override;
};