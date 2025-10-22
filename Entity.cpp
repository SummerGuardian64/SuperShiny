#include "Entity.h"
#include "StepContext.h"
#include "DrawContext.h"
#include <iostream>
#include "Utilities.h"
#include "Level.h"

using namespace ssge;

Entity::Entity()
{
}

SDL_FPoint Entity::Physics::setVelocityMagnitude(const SDL_FPoint& v, float speed)
{
	return SDL_FPoint();
}

float Entity::Physics::getDistance(const Entity& other) const
{
	//FIXME: UNIMPLEMENTED!
	return 0.0f;
}

Entity::Physics::Physics(Entity& entity)
	: entity(entity),
	  position(entity.position),
	  hitbox(entity.hitbox)
{
}

inline SDL_FRect makeWorldAABB(const SDL_FPoint& pos, const SDL_FRect& localHitbox) {
    return SDL_FRect{ pos.x + localHitbox.x, pos.y + localHitbox.y, localHitbox.w, localHitbox.h };
}

inline void applyResolvedWorldAABBToEntity(SDL_FPoint& pos, const SDL_FRect& localHitbox, const SDL_FRect& worldAABB) {
    // localHitbox is relative to pos, so pos = worldBox - localOffset
    pos.x = worldAABB.x - localHitbox.x;
    pos.y = worldAABB.y - localHitbox.y;
}

void Entity::Physics::step(EntityStepContext& context)
{
	// Platformer Physics

    // Make a snapshot of the current pad
    InputPad pad;
    if (auto ctrl = entity.control.get())
    {
        pad = ctrl->getPad();
    }

    if (enablePhysics)
    {
        // Check direction buttons X
        side.x = enableHorizontalMove ? pad.isPressed(3) - pad.isPressed(2) : 0;
        // Check direction buttons Y
        side.y = enableVerticalMove ? pad.isPressed(1) - pad.isPressed(0) : 0;
        // Reset jump timer if jump isn't pressed
        if (!pad.isPressed(4))//Jump
        {
            jumpTimer = 0;
        }
        // Set accelleration, deccelleration and maximum speeds
        // And also, offer jump
        if (grounded)
        {
            if (inWater)
            {
                if (running)
                {
                    acc.x = accWaterRun.x;
                    dec.x = decWaterRun.x;
                    maxSpeedHor = maxRunInWaterSpeed;
                }
                else
                {
                    acc.x = accWaterWalk.x;
                    dec.x = decWaterWalk.x;
                    maxSpeedHor = maxWalkInWaterSpeed;
                }
            }
            else
            {
                // Offer an ability to jump
                if (enableJump)
                {
                    if (pad.isJustPressed(4))//Jump just pressed
                    {
                        jumpTimer = jumpStrength;
                    }
                }
                if (running)
                {
                    acc.x = accRun.x;
                    dec.x = decRun.x;
                    maxSpeedHor = maxRunSpeed;
                }
                else
                {
                    acc.x = accWalk.x;
                    dec.x = decWalk.x;
                    maxSpeedHor = maxWalkSpeed;
                }
            }
        }
        else
        {
            if (inWater)
            {
                acc.x = accWater.x;
                dec.x = decWater.x;
                maxSpeedHor = maxSwimSpeed;
                maxSpeedUp = maxWaterJumpSpeed;
                maxSpeedDown = maxWaterFallSpeed;
            }
            else
            {
                acc.x = accAir.x;
                dec.x = decAir.x;
                if (running)
                {
                    maxSpeedHor = maxAirRunSpeed;
                }
                else
                {
                    maxSpeedHor = maxAirWalkSpeed;
                }
                maxSpeedUp = maxAirJumpSpeed;
                maxSpeedDown = maxAirFallSpeed;
            }
        }
        if (inWater)
        {
            if (running)
            {
                acc.x = accWaterRun.x;
                dec.y = decWaterRun.y;
            }
            else
            {
                acc.y = accWater.y;
                dec.y = decWater.y;
            }
        }
        else
        {
            if (running)
            {
                acc.y = accAirRun.y;
                dec.y = decAirRun.y;
            }
            else
            {
                acc.y = accAir.y;
                dec.y = decAir.y;
            }
        }
        // Offer the ability to swim
        if (enableSwim)
        {
            if (inWater)
            {
                if (pad.isJustPressed(4)) // Jump just pressed
                {
                    if (speed.y > -swimPower)
                    {
                        speed.y = -swimPower;
                    }
                }
            }
        }
        // Offer the ability to move horizontally
        if (enableHorizontalMove)
        {
            speed.x += sign(side.x) * acc.x;
            if (abs(speed.x) > maxSpeedHor)
            {
                speed.x = sign(speed.x) * maxSpeedHor;
            }
        }
        // Offer the ability to move vertically
        if (enableVerticalMove)
        {
            speed.y += sign(side.y) * acc.y;
            if ((speed.y < 0) && (-speed.y > maxSpeedUp))
            {
                speed.y = sign(speed.y) * maxSpeedUp;
            }
            if (speed.y > maxSpeedDown)
            {
                speed.y = sign(speed.y) * maxSpeedDown;
            }
        }
        // Deccellerate
        if (side.x == 0)
        {
            if (dec.x > abs(speed.x))
            {
                speed.x = 0;
            }
            else
            {
                speed.x -= sign(speed.x) * dec.x;
            }
        }
        if (side.y == 0 && jumpTimer == 0 && gravity == 0)
        {
            if (dec.y > abs(speed.y))
            {
                speed.y = 0;
            }
            else
            {
                speed.y -= sign(speed.y) * dec.y;
            }
        }
        // Apply jump
        if (jumpTimer > 0)
        {
            if (speed.y >= -jumpSpeed)
            {
                jumpTimer--;
                speed.y = -jumpSpeed;
            }
        }
        else if (side.y == 0)
        {
            if (inWater)
            {
                speed.y += gravityInWater;
            }
            else
            {
                speed.y += gravity;
            }
        }

        // COLLISION

        // Some slope thing
        /*if(Grounded)
        {
          if(place_meeting(x+SpeedX,y,par_Block))
          {
            if(!place_meeting(x+SpeedY,y-16,par_Block))
            {
              while(place_meeting(x+SpeedY,y,par_Block))
              {
                y--;
              }
            }
          }
        }*/

        // After you compute speed.x, speed.y for this fixed step:
        float dx = speed.x;   // per-step displacement in world units (you�re already stepping at fixed dt)
        float dy = speed.y;

        // We'll apply axis-separated resolution: X then Y, using level sweeps.
        // Build world-space AABB from (position + local hitbox)
        SDL_FRect box = makeWorldAABB(position, hitbox);

        // HORIZONTAL
        bool applyx = true;
        if (enableHorizontalCollision && !ignoreCollision && dx != 0.f)
        {
            Level::SweepHit hx = context.level.sweepHorizontal(box, dx);
            if (hx.hit)
            {
                // We hit a solid tile�resolve at boundary and zero x-speed (or bounce if enabled)
                box.x = hx.newX;
                if (enableHorizontalBounce && !enableGMBounce)
                {
                    speed.x = -speed.x;
                    applyx = false; // bounce handled position; don�t re-apply afterwards
                }
                else
                {
                    speed.x = 0.f;
                }
                touchesWall = true;
                // optional: store hx.tile if you want terraforming later
            }
            else {
                box.x = hx.newX;
            }
        }
        if (applyx)
        {
            // If we didn�t bounce-position, adopt the new x
            // Convert resolved world AABB back to entity position (x only)
            SDL_FRect tmp = box;
            tmp.y = position.y + hitbox.y; // keep current y for now
            applyResolvedWorldAABBToEntity(position, hitbox, tmp);
        }

        // VERTICAL
        bool applyy = true;
        if (enableVerticalCollision && !ignoreCollision && dy != 0.f)
        {
            Level::SweepHit hy = context.level.sweepVertical(box, dy);
            if (hy.hit)
            {
                box.y = hy.newY;
                if (enableVerticalBounce && !enableGMBounce) {
                    speed.y = -speed.y;
                    applyy = false;
                }
                else
                {
                    // landing logic
                    if (dy > 0.f) { grounded = true; }     // moving down -> ground
                    if (dy < 0.f) { jumpTimer = 0.0; }     // bonked head -> stop jump
                    speed.y = 0.f;
                }
                // optional: terraforming hook with hy.tile
            }
            else
            {
                grounded = false; // only set when actually supported by ground hit
                box.y = hy.newY;
            }
        }
        if (applyy)
        {
            // Convert resolved world AABB back to entity position (y now included)
            applyResolvedWorldAABBToEntity(position, hitbox, box);
        }

        // Probe 1px below to keep grounded accurate when dy is ~0
        SDL_FRect probe = box;
        probe.y += 1;
        int c0, c1, r0, r1;
        context.level.rectToTileSpan(probe, c0, c1, r0, r1);
        bool onSolid = false;
        for (int r = r0; r <= r1; ++r)
            for (int c = c0; c <= c1; ++c)
                if (context.level.queryTile(c, r).coll == Level::Block::Collision::Solid)
                    onSolid = true;
        grounded = grounded || onSolid;

        // WATER (or other mediums)
        // Detect if any overlapped tile is water; update state:
        inWater = context.level.rectInWater(box);

    }
}

Entity::NPC::NPC(Entity& entity) : entity(entity)
{
}

void ssge::Entity::latch(EntityStepContext& context)
{
    if (auto c = control.get())
    {
        c->latch(context);
    }
}

void Entity::step(EntityStepContext& context)
{
	double deltaTime = context.deltaTime;

	// If this is the entity's first step, execute it
	if (lifespan == 0)
	{
		firstStep(context);
	}

	// First, execute the entity-specific pre-step code
	preStep(context);

	// Now we process physics (if we have them, that is)
	if (physics)
	{
		physics->step(context);
	}
	
	// Finally, execute the entity-specific post-step code
	postStep(context);

	if (sprite)
	{
		sprite->update(deltaTime);
	}

	// Increment lifespan
	//FIXME: THIS NEEDS REFINEMENT!
	lifespan += deltaTime * 60;

	std::cout << "Entity " << (void*)this << " has been there for " << lifespan << std::endl;
}

void Entity::draw(DrawContext context) const
{
	preDraw(context);

	if (sprite)
	{
		sprite->draw(context);
	}

	postDraw(context);
}

void Entity::destroy()
{
	scheduledToDestroy = true;
}

Entity::Control::Control(Entity& entity) : entity(entity)
{
	mode = Mode::None;
	ignore = false;
	playable = false;
	playerId = -1;
}

ssge::Entity::Control* ssge::Entity::getControl()
{
    return control.get();
}

ssge::InputPad ssge::Entity::getPad()
{
    if (auto ctrl = getControl())
    {
        return ctrl->getPad();
    }
    else InputPad();
}

ssge::Entity::Physics* ssge::Entity::getPhysics()
{
    return physics.get();
}

void ssge::Entity::Control::latch(EntityStepContext& context)
{
    uint32_t directButtons = 0;

    switch (mode)
    {
    case ssge::Entity::Control::Mode::Player:
        directButtons = context.inputs.getCurrentButtonsForPlayer(playerId);
        break;
    case ssge::Entity::Control::Mode::AI:
        //TODO: UNIMPLEMENTED!
        break;
    case ssge::Entity::Control::Mode::Replay:
        //TODO: UNIMPLEMENTED!
        break;
    case ssge::Entity::Control::Mode::NPC:
        //TODO: UNIMPLEMENTED!
        break;
    case ssge::Entity::Control::Mode::Cutscene:
        //TODO: UNIMPLEMENTED!
        break;
    //case ssge::Entity::Control::Mode::None:
    //case ssge::Entity::Control::Mode::TOTAL:
    //TODO: Error handling
    default:
        break;
    }

    pad.latchButtons(directButtons);
}
