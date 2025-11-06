#pragma once
#include "../ssge/Entity.h"

class Shiny : public ssge::Entity
{
	bool dying = false;

	void animate(ssge::EntityStepContext& context);

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
	ssge::EntityClassID getEntityClassID() const override;
	void firstStep(ssge::EntityStepContext& context) override;
	void preStep(ssge::EntityStepContext& context) override;
	void postStep(ssge::EntityStepContext& context) override;
	void preDraw(ssge::DrawContext& context) const override;
	void postDraw(ssge::DrawContext& context) const override;
	void onDestroy(ssge::EntityStepContext& context) override;
};