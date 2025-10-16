#include "Entity.h"
#include "StepContext.h"
#include "DrawContext.h"
#include <iostream>
#include "Utilities.h"

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

void Entity::Physics::step(EntityStepContext& context)
{
	//// If we wanna process velocity
	//if (processVelocity)
	//{
	//	// Then execute the ordinary step
	//	SDL_FPoint tempVector = velocity;
	//	float tempScalar = context.getDeltaTime();
	//	tempVector.x *= tempScalar;
	//	tempVector.y *= tempScalar;
	//	position.x += tempVector.x;
	//	position.y += tempVector.y;
	//}

    //GML port

    //FIXME
    int fixmeMockupFloor = 650;

    ///Platformer Physics
    // Copy collision list
    //ds_list_clear(CollisionsPrevious);
    //ds_list_copy(CollisionsPrevious, Collisions);
    //ds_list_clear(Collisions);
    if (enablePhysics)
    {
        // Check direction buttons X
        side.x = enableHorizontalMove ? context.inputs.isPressed(3) - context.inputs.isPressed(2) : 0;
        // Check direction buttons Y
        side.y = enableVerticalMove ? context.inputs.isPressed(1) - context.inputs.isPressed(0) : 0;
        // Reset jump timer if jump isn't pressed
        if (!context.inputs.isPressed(4))//Jump
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
                    if (context.inputs.isJustPressed(4))//Jump just pressed
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
                if (context.inputs.isJustPressed(4)) // Jump just pressed
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
        else
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
        // Set solids
        //for (var i = 0; i < ds_list_size(SolidList); i++)
        //{
        //    var se = ds_list_find_value(SolidList, i);
        //    se.solid = true;
        //}

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
        bool applyx = true;
        bool applyy = true;
        int oldysign = (speed.y ? (speed.y > 0 ? 1 : -1) : 0);
        int oldxsign = (speed.x ? (speed.x > 0 ? 1 : -1) : 0);
        if ((enableHorizontalBounce && enableVerticalBounce) || (enableGMBounce))
        {
            applyx = false;
            applyy = false;
        }
        // Collision when falling
        if (enableVerticalCollision && !ignoreCollision)
        {
            //FIXME: This is a mockup
            if (position.y >= fixmeMockupFloor && speed.y>=0)
            {
                if (enableVerticalBounce)
                {
                    position.y -= (position.y - fixmeMockupFloor);
                    speed.y *= -1;
                    applyy = false;
                    grounded = false;
                }
                else
                {
                    position.y = fixmeMockupFloor;
                    speed.y = 0;
                    grounded = true;
                }
            }

            //FIXME: Uncomment after removing mockup!
            //if(place_meeting(x,y+SpeedY,par_Block))
        //    if (/*!place_free(x, y + SpeedY)*/ false) //TODO: Collision
        //    {
        //        if (!enableVerticalBounce)
        //        {
        //            if (speed.y != 0) // Prevent infinite loop
        //            {
        //                // Just like "Move to Contact"
        //                //while(!place_meeting(x,y+sign(SpeedY),par_Block)){y+=sign(SpeedY);}
        //                while (/*place_free(position.x, position.y + sign(speed.x))*/ false) //TODO: Collision
        //                {
        //                    position.y += sign(speed.y);
        //                }
        //            }
        //            if (speed.y > 0) { grounded = true; }
        //            if (speed.y < 0) { jumpTimer = 0; grounded = false; } // Stop the jump
        //            speed.y = 0;
        //        }
        //        else if (!enableGMBounce)
        //        {
        //            int steps = 0;
        //            if (speed.y != 0) // Prevent infinite loop
        //            {
        //                //while(!place_meeting(x,y+sign(SpeedY),par_Block))
        //                while (/*place_free(position.x, position.y + sign(speed.y))*/ false) //TODO: Collision
        //                {
        //                    position.y += sign(speed.y);
        //                    steps++;
        //                }
        //                position.y -= sign(speed.y) * (abs(speed.y) - steps);
        //                if (speed.y > 0)
        //                {
        //                    grounded = true;
        //                }
        //                speed.y *= -1;
        //                applyy = false;
        //            }
        //            grounded = false;
        //        }
        //    }
        //    else
        //    {
        //        oldysign = 0;
        //        grounded = false;
        //    }
        }
        // Apply speed
        if (applyy)
        {
            position.y += speed.y;
        }
        // Collision when walking
        if (enableHorizontalCollision && !ignoreCollision)
        {
            //if(place_meeting(x+SpeedX,y,par_Block))
            if (/*!place_free(position.x + speed.x, position.y)*/false)//TODO: Collision
            {
                if (!enableHorizontalBounce)
                {
                    if (speed.x != 0) // Prevent infinite loop
                    {
                        // Just like "Move to Contact"
                        //while(!place_meeting(x+sign(SpeedX),y,par_Block)){x+=sign(SpeedX);}
                        while (/*place_free(position.x + sign(speed.x), position.y)*/ false)//TODO: Collision
                        {
                            position.x += sign(speed.x);
                        }
                    }
                    speed.x = 0;
                }
                else if (!enableGMBounce)
                {
                    int steps = 0;
                    if (speed.x != 0) // Prevent infinite loop
                    {
                        //while(!place_meeting(x+sign(SpeedX),y,par_Block))
                        while (/*place_free(position.x + sign(speed.x) : 0), position.y)*/ false)
                        {
                            position.x += sign(speed.x);
                            steps++;
                        }
                        position.x -= sign(speed.x) * (abs(speed.x) - steps);
                        speed.x *= -1;
                        applyx = false;
                    }
                    touchesWall = true;
                }
            }
            else
            {
                oldxsign = 0;
            }
        }
        // Apply speed
        if (applyx)
        {
            position.x += speed.x;
        }
        if (!ignoreCollision)
        {
            // Add solids to collision list
            /*for (var i = 0; i < ds_list_size(SolidList); i++)
            {
                var w = ds_list_find_value(SolidList, i);
                if (ds_list_find_index(Collisions, w) == -1)
                {
                    if (place_meeting(x + oldxsign, y + oldysign, w))
                    {
                        ds_list_add(Collisions, w);
                    }
                }
            }*/
            // Do GM's built-in bounce code
            if (enableGMBounce)
            {
                //FIXME: GM bounce replacement, please!
                
                //hspeed = SpeedX;
                //vspeed = SpeedY;
                //x += hspeed;
                //y += vspeed;
                ////var tmpmask=mask_index;
                ////mask_index=BounceMask;
                ////par_Block.solid=true;
                ///*var tmpwalls=ds_queue_create();
                //for(var i=0;i<instance_number(par_Block);i++)
                //{
                //    var wall=instance_find(par_Block,i);
                //    if(place_meeting(x,y,wall))
                //    {
                //        ds_queue_enqueue(tmpwalls,instance_find(par_Block,wall));
                //        wall.solid=true;
                //    }
                //}*/
                //move_bounce_solid(true);
                ////event_user(0);
                ////par_Block.solid=false;
                ///*while(!ds_queue_empty(tmpwalls))
                //{
                //    var wall=ds_queue_dequeue(tmpwalls);
                //    wall.solid=false;
                //}
                //ds_queue_destroy(tmpwalls);*/
                //SpeedX = hspeed;
                //SpeedY = vspeed;
                //hspeed = 0;
                //vspeed = 0;
                ////mask_index=tmpmask;
            }

            //FIXME: MOCKUP
            if (position.y >= fixmeMockupFloor)
            {
                position.y = fixmeMockupFloor;
            }

            // Break out if stuck inside of a wall
            //if (place_meeting(x,y,par_Block))
            if (/*!place_free(position.x, position.y)*/ false) //TODO: Collision
            {
                // Make some better code!
                //while(place_meeting(x,y,par_Block)){y--;}
                /*hspeed = SpeedX;
                vspeed = SpeedY;
                move_outside_solid(direction, sqrt(sqr(hspeed) + sqr(vspeed)))
                    SpeedX = hspeed;
                SpeedY = vspeed;
                hspeed = 0;
                vspeed = 0;*/
            }
            // Add solids to collision list
            /*for (var i = 0; i < ds_list_size(SolidList); i++)
            {
                var w = ds_list_find_value(SolidList, i);
                if (ds_list_find_index(Collisions, w) == -1)
                {
                    if (place_meeting(x + oldxsign, y + oldysign, w))
                    {
                        ds_list_add(Collisions, w);
                    }
                }
            }*/
            if (/*!place_free(x, y)*/ false) //TODO: Collision
            {
                while (/*!place_free(position.x, position.y)*/ false)//TODO: Collision
                {
                    position.y--;
                }
            }

            // Check if touching a wall
            touchesWall = false;
            //if(place_meeting(x+1,y,par_Block))||(place_meeting(x-1,y,par_Block))
            if (/*!place_free(position.x + 1, position.y)) || (!place_free(position.x - 1, position.y)*/ false) //TODO: Collision
            {
                touchesWall = true;
            }

            // Check if grounded
            // FIXME: UNCOMMENT THIS SNIPPET!
            //grounded = false;
            //if (place_meeting(x,y+1,par_Block))
            //if (/*!place_free(position.x, position.y + 1)*/ false)
            //{
            //    grounded = true;
            //}
            //// Add solids to collision list
            //for (var i = 0; i < ds_list_size(SolidList); i++)
            //{
            //    var w = ds_list_find_value(SolidList, i);
            //    if (ds_list_find_index(Collisions, w) == -1)
            //    {
            //        if (place_meeting(x + 1, y, w))
            //        {
            //            ds_list_add(Collisions, w);
            //        }
            //        if (place_meeting(x - 1, y, w))
            //        {
            //            ds_list_add(Collisions, w);
            //        }
            //        if (place_meeting(x, y + 1, w))
            //        {
            //            ds_list_add(Collisions, w);
            //        }
            //    }
            //}
        }
        //// Restore solids
        //for (var i = 0; i < ds_list_size(SolidList); i++)
        //{
        //    var se = ds_list_find_value(SolidList, i);
        //    se.solid = false;
        //}
    }
}

Entity::NPC::NPC(Entity& entity) : entity(entity)
{
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
