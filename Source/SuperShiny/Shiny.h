#pragma once
#include "../ssge/Entity.h"

using namespace ssge;

class Shiny : public Entity
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
	void animate(EntityStepContext& context);

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

	Entity::Physics::Abilities makeRegularAbilities() const;
	Entity::Physics::Abilities makeBubblingAbilities() const;

	Shiny();

	bool isDying() const;

	// Called when Shiny starts dying
	void die();

	// Helper function to read Box block callbacks
	int makeBoxNumber(std::string callback) const;

	// Inherited via Entity
	std::string getEntityClassID() const override;
	void firstStep(EntityStepContext& context) override;
	void preStep(EntityStepContext& context) override;
	void postStep(EntityStepContext& context) override;
	void preDraw(DrawContext& context) const override;
	void postDraw(DrawContext& context) const override;
	void onDestroy(EntityStepContext& context) override;
};
