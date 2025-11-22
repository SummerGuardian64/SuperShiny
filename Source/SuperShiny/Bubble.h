#pragma once
#include "../ssge/Entity.h"

using namespace ssge;

class Bubble : public Entity
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
	void firstStep(EntityStepContext& context) override;
	void preStep(EntityStepContext& context) override;
	void postStep(EntityStepContext& context) override;
	void preDraw(DrawContext& context) const override;
	void postDraw(DrawContext& context) const override;
	void onDestroy(EntityStepContext& context) override;
};