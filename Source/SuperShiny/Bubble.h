#pragma once
#include "../ssge/Entity.h"

class Bubble : public ssge::Entity
{
	bool popped = false;
	void pop();
	void animate();
public:
	enum class Sequences : int
	{
		Flying = 0,
		Popped = 1
	};

	Bubble();
	// Inherited via Entity
	std::string getEntityClassID() const override;
	void firstStep(ssge::EntityStepContext& context) override;
	void preStep(ssge::EntityStepContext& context) override;
	void postStep(ssge::EntityStepContext& context) override;
	void preDraw(ssge::DrawContext& context) const override;
	void postDraw(ssge::DrawContext& context) const override;
	void onDestroy(ssge::EntityStepContext& context) override;
};