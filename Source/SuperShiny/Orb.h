#pragma once
#include "../ssge/Entity.h"

class Orb : public ssge::Entity
{
public:
	Orb();
	// Inherited via Entity
	std::string getEntityClassID() const override;
	void firstStep(ssge::EntityStepContext& context) override;
	void preStep(ssge::EntityStepContext& context) override;
	void postStep(ssge::EntityStepContext& context) override;
	void preDraw(ssge::DrawContext& context) const override;
	void postDraw(ssge::DrawContext& context) const override;
	void onDestroy(ssge::EntityStepContext& context) override;
};