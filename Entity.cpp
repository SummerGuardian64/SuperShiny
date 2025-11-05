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

inline SDL_FRect makeWorldAABB(const SDL_FPoint& pos, const SDL_FRect& localHitbox)
{
    return SDL_FRect{ pos.x + localHitbox.x, pos.y + localHitbox.y, localHitbox.w, localHitbox.h };
}

inline void applyResolvedWorldAABBToEntity(SDL_FPoint& pos, const SDL_FRect& localHitbox, const SDL_FRect& worldAABB)
{
    // localHitbox is relative to pos, so pos = worldBox - localOffset
    pos.x = floor(worldAABB.x - localHitbox.x);
    pos.y = floor(worldAABB.y - localHitbox.y);
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

    if (abilities.physicsEnabled())
    {
        // Store old velocity
        oldVelocity = velocity;
        // Check direction buttons X
        side.x = abilities.horzMove() ? pad.isPressed(3) - pad.isPressed(2) : 0;
        // Check direction buttons Y
        side.y = abilities.vertMove() ? pad.isPressed(1) - pad.isPressed(0) : 0;
        // Reset jump timer if jump isn't pressed
        if (!pad.isPressed(4))//Jump
        {
            jumpTimer = 0;
        }
        // Set accelleration, deccelleration and maximum speeds
        // And also, offer jump
        int situation = grounded | (inWater << 2);
        switch (situation)
        {
        case 0: // In air
            // TODO: Obtain abilities in air
            break;
        case 1: // Grounded
            // TODO: Obtain abilities when on ground
            break;
        case 2: // In water
            // TODO: Obtain abilities in water
            break;
        case 3: // At bottom of water
            // TODO: Obtain abilities at bottom of water
            break;
        }

        //if (grounded)
        //{
        //    if (inWater)
        //    {
        //        if (running)
        //        {
        //            acc.x = accWaterRun.x;
        //            dec.x = decWaterRun.x;
        //            maxSpeedHor = maxRunInWaterSpeed;
        //        }
        //        else
        //        {
        //            acc.x = accWaterWalk.x;
        //            dec.x = decWaterWalk.x;
        //            maxSpeedHor = maxWalkInWaterSpeed;
        //        }
        //    }
        //    else
        //    {
        //        // Offer an ability to jump
        //        if (enableJump)
        //        {
        //            if (pad.isJustPressed(4))//Jump just pressed
        //            {
        //                jumpTimer = jumpStrength;
        //            }
        //        }
        //        if (running)
        //        {
        //            acc.x = accRun.x;
        //            dec.x = decRun.x;
        //            maxSpeedHor = maxRunSpeed;
        //        }
        //        else
        //        {
        //            acc.x = accWalk.x;
        //            dec.x = decWalk.x;
        //            maxSpeedHor = maxWalkSpeed;
        //        }
        //    }
        //}
        //else
        //{
        //    if (inWater)
        //    {
        //        acc.x = accWater.x;
        //        dec.x = decWater.x;
        //        maxSpeedHor = maxSwimSpeed;
        //        maxSpeedUp = maxWaterJumpSpeed;
        //        maxSpeedDown = maxWaterFallSpeed;
        //    }
        //    else
        //    {
        //        acc.x = accAir.x;
        //        dec.x = decAir.x;
        //        if (running)
        //        {
        //            maxSpeedHor = maxAirRunSpeed;
        //        }
        //        else
        //        {
        //            maxSpeedHor = maxAirWalkSpeed;
        //        }
        //        maxSpeedUp = maxAirJumpSpeed;
        //        maxSpeedDown = maxAirFallSpeed;
        //    }
        //}
        //if (inWater)
        //{
        //    if (running)
        //    {
        //        acc.x = accWaterRun.x;
        //        dec.y = decWaterRun.y;
        //    }
        //    else
        //    {
        //        acc.y = accWater.y;
        //        dec.y = decWater.y;
        //    }
        //}
        //else
        //{
        //    if (running)
        //    {
        //        acc.y = accAirRun.y;
        //        dec.y = decAirRun.y;
        //    }
        //    else
        //    {
        //        acc.y = accAir.y;
        //        dec.y = decAir.y;
        //    }
        //}

        // Jump button makes a jump on ground and swim in water
        if (pad.isJustPressed(4))
        {
            // Try swimming in water
            if (inWater && abilities.canSwim())
            {
                int swimSpeed = -abilities.swimPower;
                if (velocity.y > swimSpeed)
                {
                    velocity.y = swimSpeed;
                }
            }
            else if (grounded && abilities.canJump()) // Otherwise use the ground-based jump
            {
                jumpTimer = abilities.jumpStrength;
            }
        }

        // Offer the ability to move horizontally
        if (abilities.horzMove())
        {
            velocity.x += sign(side.x) * abilities.acc.x;
            if (abs(velocity.x) > abilities.maxSpeedHor)
            {
                velocity.x = sign(velocity.x) * abilities.maxSpeedHor;
            }
        }
        // Offer the ability to move vertically
        if (abilities.vertMove())
        {
            velocity.y += sign(side.y) * abilities.acc.y;
            if ((velocity.y < 0) && (-velocity.y > abilities.maxSpeedUp))
            {
                velocity.y = sign(velocity.y) * abilities.maxSpeedUp;
            }
            if (velocity.y > abilities.maxSpeedDown)
            {
                velocity.y = sign(velocity.y) * abilities.maxSpeedDown;
            }
        }
        // Deccellerate
        if (side.x == 0)
        {
            if (abilities.dec.x > abs(velocity.x))
            {
                velocity.x = 0;
            }
            else
            {
                velocity.x -= sign(velocity.x) * abilities.dec.x;
            }
        }
        if (side.y == 0 && jumpTimer == 0 && abilities.gravity == 0)
        {
            if (abilities.dec.y > abs(velocity.y))
            {
                velocity.y = 0;
            }
            else
            {
                velocity.y -= sign(velocity.y) * abilities.dec.y;
            }
        }
        // Apply jump
        if (jumpTimer > 0)
        {
            if (velocity.y >= -abilities.jumpSpeed)
            {
                jumpTimer--;
                velocity.y = -abilities.jumpSpeed;
            }
        }
        else if (side.y == 0)
        {
            //if (inWater)
            //{
            //    velocity.y += gravityInWater;
            //}
            //else
            //{
                velocity.y += abilities.gravity;
                if (velocity.y > abilities.maxSpeedDown)
                {
                    velocity.y = sign(velocity.y) * abilities.maxSpeedDown;
                }
            //}
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

        // After you compute velocity.x, velocity.y for this fixed step:
        float dx = velocity.x;   // per-step displacement in world units (you�re already stepping at fixed dt)
        float dy = velocity.y;

        bool collisionEnabled = !abilities.collisionIgnored();
        bool horzCollision = abilities.horzCollision() && collisionEnabled;
        bool vertCollision = abilities.vertCollision() && collisionEnabled;

        // We'll apply axis-separated resolution: X then Y, using level sweeps.
        // Build world-space AABB from (position + local hitbox)
        SDL_FRect box = makeWorldAABB(position, hitbox);

        // HORIZONTAL
        bool applyx = true;
        if (horzCollision && dx != 0.f)
        {
            Level::SweepHit hx = context.level.sweepHorizontal(box, dx);
            if (hx.hit)
            {
                // We hit a solid tile�resolve at boundary and zero x-velocity (or bounce if enabled)
                box.x = hx.newX;
                if (abilities.horzBounce() && !abilities.gmBounce())
                {
                    velocity.x = -velocity.x;
                    applyx = false; // bounce handled position; don�t re-apply afterwards
                }
                else
                {
                    velocity.x = 0.f;
                }
                touchesWall = true;
                // optional: store hx.tile if you want terraforming later
            }
            else
            {
                box.x = hx.newX;
            }
            if (applyx)
            {
                // If we didn't bounce-position, adopt the new x
                // Convert resolved world AABB back to entity position (x only)
                SDL_FRect tmp = box;
                tmp.y = position.y + hitbox.y; // keep current y for now
                applyResolvedWorldAABBToEntity(position, hitbox, tmp);
            }
        }
        else
        {
            touchesWall = false;
            if (applyx)
            {
                position.x += velocity.x;
            }
        }

        // VERTICAL
        bool applyy = true;
        if (vertCollision && !abilities.collisionIgnored() && dy != 0.f)
        {
            Level::SweepHit hy = context.level.sweepVertical(box, dy);
            if (hy.hit)
            {
                box.y = hy.newY;
                if (abilities.vertBounce() && !abilities.gmBounce())
                {
                    velocity.y = -velocity.y;
                    applyy = false;
                }
                else
                {
                    // landing logic
                    if (dy > 0.f)
                    {
                        // moving down -> ground
                        grounded = true;
                    }
                    if (dy < 0.f)
                    {
                        // bonked head -> stop jump
                        jumpTimer = 0.0;
                    }
                    velocity.y = 0.f;
                }
                // optional: terraforming hook with hy.tile
            }
            else
            {
                grounded = false; // only set when actually supported by ground hit
                box.y = hy.newY;
            }
            if (applyy)
            {
                // Convert resolved world AABB back to entity position (y now included)
                applyResolvedWorldAABBToEntity(position, hitbox, box);
            }
        }
        else
        {
            grounded = false;
            if (applyy)
            {
                position.y += velocity.y;
            }
        }

        if (vertCollision)
        {
            // Probe 1px below to keep grounded accurate when dy is ~0
            SDL_FRect probe = box;
            probe.w -= 1; // FIXME: This bodge fixes the broken jump
            probe.y += 1;
            int c0, c1, r0, r1;
            context.level.rectToBlockSpan(probe, c0, c1, r0, r1);
            bool onSolid = false;
            for (int r = r0; r <= r1; ++r)
                for (int c = c0; c <= c1; ++c)
                    if (context.level.queryBlock(c, r).coll == Level::Block::Collision::Solid)
                        onSolid = true;
            grounded = grounded || onSolid;
        }

        if (collisionEnabled)
        {
            // WATER (or other mediums)
            // Detect if any overlapped tile is water; update state:
            inWater = context.level.rectInWater(box);
        }
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
	lifespan++;

	//std::cout << "Entity " << (void*)this << " has been there for " << lifespan << std::endl;
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

    if (!ignore)
    {
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
    }

    pad.latchButtons(directButtons);
}
