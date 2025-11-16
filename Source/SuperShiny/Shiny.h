#pragma once
#include "../ssge/Entity.h"

class Shiny : public ssge::Entity
{
    bool bubbleAux = false;
    float bubbleAuxOffset = -6;
	bool dying = false;
	bool bubbling = false;
	int bubbleTimer = 0;
	int bubbleDelay = 10;
	int bubbleAnim = 0;
	bool bubbleReady = true;
	int bubbleX = 0;
	int bubbleY = 0;

	void startBubbling();
	void quitBubbling();
	void animate(ssge::EntityStepContext& context);

public:

	enum class Sequences : int
	{
		Stopped = 0,
		Walking = 1,
		Jumping = 2,
		Falling = 3,
		Dying = 4,
		BubbleReady = 5,
		BubblingForward = 6,
		BubblingForwardUp = 7,
		BubblingUp = 8,
		BubblingForwardDown = 9
	};

	ssge::Entity::Physics::Abilities makeRegularAbilities() const;
	ssge::Entity::Physics::Abilities makeBubblingAbilities() const;

	Shiny();

	bool isDying() const;
	void die();
	int makeBoxNumber(std::string callback) const;

	// Inherited via Entity
	std::string getEntityClassID() const override;
	void firstStep(ssge::EntityStepContext& context) override;
	void preStep(ssge::EntityStepContext& context) override;
	void postStep(ssge::EntityStepContext& context) override;
	void preDraw(ssge::DrawContext& context) const override;
	void postDraw(ssge::DrawContext& context) const override;
	void onDestroy(ssge::EntityStepContext& context) override;
};
