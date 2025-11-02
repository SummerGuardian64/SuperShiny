#pragma once
#include "Entity.h"

using namespace ssge;

class Shiny : public Entity
{
	bool dying = false;

	void animate(EntityStepContext& context);

public:
	
	enum class Sequences : int
	{
		Stopped = 0,
		Walking = 1,
		Jumping = 2,
		Falling = 3,
		Dying = 4
	};

	Shiny();

	bool isDying() const;
	void die();

	// Inherited via Entity
	EntityClassID getEntityClassID() const override;
	void firstStep(EntityStepContext& context) override;
	void preStep(EntityStepContext& context) override;
	void postStep(EntityStepContext& context) override;
	void preDraw(DrawContext& context) const override;
	void postDraw(DrawContext& context) const override;
	void onDestroy(EntityStepContext& context) override;
};