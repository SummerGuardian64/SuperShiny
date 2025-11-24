#include "Shiny.h"
#include "../ssge/StepContext.h"
#include "../ssge/DrawContext.h"
#include <iostream>
#include <memory>
#include "../ssge/EntityManager.h"
#include "../ssge/Utilities.h"
#include "SDL.h"

void Shiny::startBubbling()
{
    if (!bubbling)
    {
        bubbling = true;
        bubbleTimer = 0;
        physics->abilities = makeBubblingAbilities();
        bubbleX = sign(sprite->xscale);
        bubbleY = 0;
    }
}

void Shiny::quitBubbling()
{
    if (bubbling)
    {
        bubbling = false;
        if(!dying) // Don't restore abilities if dying
            physics->abilities = makeRegularAbilities();
    }
}

void Shiny::animate(EntityStepContext& context)
{
    if (sprite && physics)
    {
        // Flip the sprite according to entity's facing direction (side)
        auto direction = physics->side.x;
        if (direction == 0) { direction = sign(physics->velocity.x); }
        if (direction > 0) sprite->xscale = 1;
        else if (direction < 0) sprite->xscale = -1;

        // Declare current situation

        bool grounded = physics->grounded;
        bool walking = grounded && direction;
        bool jumping = physics->velocity.y <= 0;

        // Declare current and wanted sequence

        int currentSequence = sprite->getSeqIdx();
        int wantedSequence = currentSequence;

        if (isDying())
        {
            wantedSequence = (int)Sequences::Dying;
        }
        else if (bubbling)
        {
            wantedSequence = bubbleAnim;
        }
        else if (walking)
        {
            // Linear-interpolate walking animation speed
            // FIXME: Tweak me!
            int lerp = (int)(abs(physics->velocity.x) / physics->abilities.maxSpeedHor * 100);
            sprite->setLerp(lerp);

            // We wanna walk
            wantedSequence = (int)Sequences::Walking;
        }
        else if (grounded)
        {
            if (bubbleReady)
            {
                wantedSequence = (int)Sequences::BubbleReady;
            }
            else
            {
                wantedSequence = (int)Sequences::Stopped;
            }
        }
        else if (jumping)
        {
            wantedSequence = (int)Sequences::Jumping;
        }
        else // Not grounded and not jumping means falling
        {
            wantedSequence = (int)Sequences::Falling;
        }

        // Play wanted sequence. DON'T REPLAY!
        if (currentSequence != wantedSequence)
        {
            sprite->setSequence(wantedSequence);
        }
    }
}

Entity::Physics::Abilities Shiny::makeRegularAbilities() const
{
    using Abilities = Entity::Physics::Abilities;
    using Ability = Abilities::Flag;
    Abilities abilities;
    abilities.set(Ability::EnablePhysics);
    abilities.set(Ability::EnableHorizontalMove);
    //abilities.set(Ability::EnableVerticalBounce);
    //abilities.set(Ability::EnableVerticalMove);
    abilities.set(Ability::EnableJump);
    abilities.set(Ability::EnableHorizontalCollision);
    abilities.set(Ability::EnableVerticalCollision);

    abilities.maxSpeedHor = 8;
    abilities.maxSpeedUp = 10;
    abilities.maxSpeedDown = 10;
    abilities.acc = { 0.5,0.5 };
    abilities.dec = { 0.5,0.5 };

    abilities.jumpSpeed = 9;
    abilities.jumpStrength = 15;
    abilities.swimPower = 5;
    abilities.gravity = 0.7f;
    return abilities;
}

Entity::Physics::Abilities Shiny::makeBubblingAbilities() const
{
    using Abilities = Entity::Physics::Abilities;
    using Ability = Abilities::Flag;
    Abilities abilities;
    abilities.set(Ability::EnablePhysics);
    abilities.set(Ability::EnableHorizontalMove);
    //abilities.set(Ability::EnableVerticalBounce);
    //abilities.set(Ability::EnableVerticalMove);
    abilities.reset(Ability::EnableJump);
    abilities.set(Ability::EnableHorizontalCollision);
    abilities.set(Ability::EnableVerticalCollision);

    abilities.maxSpeedHor = 8;
    abilities.maxSpeedUp = 10;
    abilities.maxSpeedDown = 10;
    abilities.acc = { 0.0,0.0 };
    abilities.dec = { 0.5,0.5 };

    abilities.jumpSpeed = 0;
    abilities.jumpStrength = 15;
    abilities.swimPower = 0;
    abilities.gravity = 0.7f;
    return abilities;
}

Shiny::Shiny()
{
	//sprite = std::make_unique<Sprite>(Game::Sprites::shiny());
	physics = std::make_unique<Entity::Physics>(*this);
    control = std::make_unique<Entity::Control>(*this);

    control->makePlayableBy(0);

    position.x = 400;
    position.y = 100;

    physics->abilities = makeRegularAbilities();

    // Refactor these into their own abilities

    //physics->gravityInWater = 0.1;

    //physics->maxWalkSpeed = 8;
    //physics->maxRunSpeed = 10;
    //physics->maxAirWalkSpeed = 7;
    //physics->maxAirRunSpeed = 4;
    //physics->maxAirJumpSpeed = 7;
    //physics->maxAirFallSpeed = 10;
    //physics->maxWalkInWaterSpeed = 3;
    //physics->maxRunInWaterSpeed = 4;
    //physics->maxWaterJumpSpeed = 4;
    //physics->maxWaterFallSpeed = 5;
    //physics->maxSwimSpeed = 8;

    //physics->accWalk.x = 0.5;
    //physics->accRun.x = 0.5;
    //physics->accAir.x = 0.3;
    //physics->accWater.x = 0.4;
    //physics->accWaterWalk.x = 0.3;
    //physics->accWaterRun.x = 0.35;

    //physics->decWalk.x = 0.5;
    //physics->decRun.x = 1;
    //physics->decAir.x = 0.25;
    //physics->decWater.x = 0.1;
    //physics->decWaterWalk.x = 0.5;
    //physics->decWaterRun.x = 0.5;
    //
    //physics->accAir.y = 0.5;
    //physics->accAirRun.y = 0.75;
    //physics->accWater.y = 0.75;
    //physics->accWaterRun.y = 1;
    //
    //physics->decAir.y = 0.75;
    //physics->decAirRun.y = 1;
    //physics->decWater.y = 0.5;
    //physics->decWaterRun.y = 0.8;

    hitbox.x = -15;
    hitbox.y = -86;
    hitbox.w = 30;
    hitbox.h = 86;
}

bool Shiny::isDying() const
{
    return dying;
}

void Shiny::die()
{
    // Cannot die twice
    if (dying)
        return;
    
    dying = true;

    // Restore regular abilities first
    physics->abilities = makeRegularAbilities();

    // Boing up into the air and have no control over it
    physics->velocity.x = 0;
    physics->velocity.y = -20;
    physics->abilities.maxSpeedUp = 20;
    physics->abilities.enable(Entity::Physics::Abilities::Flag::IgnoreCollision);
    if (control)control->ignore();
}

std::string Shiny::getEntityClassID() const
{
	return "Shiny";
}

void Shiny::firstStep(EntityStepContext& context)
{
    // Create a sprite for Shiny
    sprite = context.sprites.create("Shiny");
}

void Shiny::preStep(EntityStepContext& context)
{
    if (auto ctrl = control.get())
    {
        InputPad pad = ctrl->getPad();

        // Orb throwing. Old powerup.
        // TODO: Revive in v0.1.3+
        /*if (pad.isJustPressed(6))
        {
            if (auto orbRef = context.entities.addEntity("Orb"))
            {
                auto& orb = *orbRef.get();
                orb.position = position;
                orb.position.y -= 40;

                if(sprite->xscale<0)
                    orb.getPhysics()->velocity.x *= -1;
            }
        }*/

        // If "Dash1" is held, Shiny will stop and shoot bubbles
        if (pad.isPressed(InputSet::Dash1))
        {
            // Bubbleshooting
            if (physics->grounded)
            {
                startBubbling();
            }
        }
        else
        {
            // Quit bubbleshooting
            if (bubbling)
            {
                quitBubbling();
            }
        }

        // Bubbleshooting mechanic
        if (bubbling)
        {
            if (physics->velocity.y != 0)
            { // Shiny cannot bubbleshoot if he skids off of the ground
                quitBubbling();
            }
            else
            { // Lucky Luke PS1 kinda shooting mechanic
                bool up = pad.isPressed(InputSet::Up);
                bool down = pad.isPressed(InputSet::Down);
                bool left = pad.isPressed(InputSet::Left);
                bool right = pad.isPressed(InputSet::Right);

                if (up) bubbleY = -1;
                if (down) bubbleY = 1;
                if (left) bubbleX = -1;
                if (right) bubbleX = 1;

                if (up && !down)
                {
                    // If left XOR right is NOT pressed, we're shooting up!
                    if ((left ^ right) == false) bubbleX = 0;
                }
                else if (down && !up)
                { // Shooting down can only be diagonal
                    if (bubbleX == 0) // Sanitize horizontal direction!
                        bubbleX = sign(sprite->xscale);
                }
                else
                {
                    bubbleY = 0;
                    if (left && right)
                    { // Sanitize both left+right pressed
                        bubbleX = sign(sprite->xscale);
                    }
                }

                // Speed of the bubble being shot
                float bubbleSpeed = 10;

                if (bubbleX * bubbleY != 0)
                { // Diagonal
                    // TODO: In-house Vector math library (v0.1.3)
                    bubbleSpeed *= sqrtf(2)/2.f;
                }

                // Bubbling animation
                if (bubbleY == 0)
                {
                    bubbleAnim = (int)Sequences::BubblingForward;
                }
                else if (bubbleY < 0)
                {
                    if (bubbleX == 0)
                    {
                        bubbleAnim = (int)Sequences::BubblingUp;
                    }
                    else
                    {
                        bubbleAnim = (int)Sequences::BubblingForwardUp;
                    }
                }
                else
                {
                    bubbleAnim = (int)Sequences::BubblingForwardDown;
                }

                // Prepare to actually spawn bubbles
                if (bubbleX != 0 || bubbleY != 0)
                {
                    // Velocity vector for the bubble
                    SDL_FPoint bubbleVector{
                        (float)bubbleX * bubbleSpeed,
                        (float)bubbleY * bubbleSpeed
                    };

                    // Shoot at bubbleDelay rate
                    if (bubbleTimer > 0)
                    {
                        bubbleTimer--;
                    }
                    else
                    {
                        // Reset delay
                        bubbleTimer = bubbleDelay;
                        
                        // Create Bubble entity
                        auto bubble = context.entities.addEntity("Bubble");
                        
                        // Place the bubble on Shiny's mouth
                        bubble->position.x = position.x + (7 * sign(sprite->xscale));

                        // Have Shiny shoot at an offset every other time
                        bubble->position.y = position.y - 60 - (bubbleAux ? bubbleAuxOffset : 0);
                        bubbleAux = !bubbleAux;

                        // Set bubble vector
                        auto bubblePhysics = bubble->getPhysics();
                        bubblePhysics->velocity = bubbleVector;
                    }
                }
            }
        }
    }

    // Report hero deadth if Shiny dies off of the screen
    if (isDying())
    {
        SDL_Rect levelBounds = context.level.calculateLevelSize();
        int tipOfTheHead = (int)(position.y + hitbox.y);
        if (tipOfTheHead >= levelBounds.h)
        {
            context.gameWorld.reportHeroDeadth();
        }
    }
}

void Shiny::postStep(EntityStepContext& context)
{
    // Used to check against deadth
    bool amIDeadYet = false;

    // TODO: Implement a Rect class with helper functions

    float tipOfTheHead = position.y + hitbox.y;
    float clawbHeight = tipOfTheHead + hitbox.h;

    // Handle collisions against blocks
    if (physics)
    {
        if (!physics->abilities.collisionIgnored())
        {
            bool walkingLeft = physics->side.x == -1 || physics->velocity.x < 0;
            bool walkingRight = physics->side.x == 1 || physics->velocity.x > 0;
            bool touchingGround = physics->grounded || physics->velocity.y > 0;
            bool touchingCeiling = physics->side.y == -1 || physics->velocity.y < 0;
            // Overal collisions
            {
                SDL_FRect collider{
                    position.x + hitbox.x - (walkingLeft ? 1 : 0),
                    position.y + hitbox.y - (touchingCeiling ? 1 : 0),
                    hitbox.w + (walkingLeft ? 1 : 0) + (walkingRight ? 1 : 0),
                    hitbox.h + (touchingCeiling ? 1 : 0) + (touchingGround ? 1 : 0)
                };
                auto allCollisions = context.level.queryBlocksUnderCollider(collider);
                for (auto& collision : allCollisions)
                {
                    auto* block = context.level.getBlockAt(collision.coords);
                    std::string callback = collision.callback;

                    // Collision with hazards
                    if (collision.coll == Level::Block::Collision::Hazard
                        || collision.coll == Level::Block::Collision::DeathOnTouch
                        || collision.coll == Level::Block::Collision::DeathIfFullyOutside)
                    { // Deadth
                        // Eliminate edge cases
                        SDL_FPoint bottomLeft{
                            position.x + hitbox.x,
                            position.y + hitbox.y + hitbox.h
                        };
                        SDL_FPoint bottomRight{
                            position.x + hitbox.x + hitbox.w,
                            position.y + hitbox.y + hitbox.h
                        };
                        SDL_FPoint bottomLeftOut{
                            position.x + hitbox.x - 1,
                            position.y + hitbox.y + hitbox.h + 1
                        };
                        SDL_FPoint bottomRightOut{
                            position.x + hitbox.x + hitbox.w + 1,
                            position.y + hitbox.y + hitbox.h + 1
                        };

                        if (block != context.level.getBlockAt(bottomLeft)
                            && block == context.level.getBlockAt(bottomLeftOut))
                        { } // Don't die through the left edge
                        else if (block != context.level.getBlockAt(bottomRight)
                            && block == context.level.getBlockAt(bottomRightOut))
                        { } // Don't die through the right edge
                        else
                        { // Deadth ensured
                            amIDeadYet = true;
                        }
                    }

                    // Handling blocks with Collectable callback
                    if (callback == "Collectable")
                    { // Leave empty
                        block->type = 0;
                    }

                    // Callback: DestroyBlock>x,y<replaceCurrent
                    if (callback.substr(0, 12) == "DestroyBlock")
                    {
                        auto rightArrowIndex = callback.find('>');
                        auto commaIndex = callback.find(',');
                        auto leftArrowIndex = callback.find('<');

                        int destroyBlockX = -1;
                        int destroyBlockY = -1;
                        int replaceCurrentBlock = -1;

                        if (rightArrowIndex != std::string::npos)
                        {
                            std::size_t start = rightArrowIndex + 1;
                            std::size_t end = callback.size();

                            // If there's also a ',' after '>', stop before it
                            if (commaIndex != std::string::npos && commaIndex > rightArrowIndex)
                            {
                                end = commaIndex;
                            }

                            try
                            {
                                std::string numberSubstring = callback.substr(start, end - start);
                                destroyBlockX = std::stoi(numberSubstring);
                            }
                            catch (...)
                            {
                                // Ignore malformed number, do nothing
                            }
                        }

                        if (commaIndex != std::string::npos)
                        {
                            std::size_t start = commaIndex + 1;
                            std::size_t end = callback.size();

                            // If there's also a '<' after ',', stop before it
                            if (leftArrowIndex != std::string::npos && leftArrowIndex > commaIndex)
                            {
                                end = leftArrowIndex;
                            }

                            try
                            {
                                std::string numberSubstring = callback.substr(start, end - start);
                                destroyBlockY = std::stoi(numberSubstring);
                            }
                            catch (...)
                            {
                                // Ignore malformed number, do nothing
                            }
                        }

                        if (leftArrowIndex != std::string::npos)
                        {
                            std::size_t start = leftArrowIndex + 1;
                            std::size_t end = callback.size();

                            try
                            {
                                std::string numberSubstring = callback.substr(start, end - start);
                                replaceCurrentBlock = std::stoi(numberSubstring);
                            }
                            catch (...)
                            {
                                // Ignore malformed number, do nothing
                            }
                        }

                        Level::Block::Coords destroyBlockCoords{destroyBlockX,destroyBlockY};
                        Level::Block* destroyBlockPtr = context.level.getBlockAt(destroyBlockCoords);

                        if(destroyBlockPtr) // Making sure the block is inside of the bounds
                        {
                            destroyBlockPtr->type = 0;
                        }

                        if(replaceCurrentBlock>=0) // Optional replacement of the current block
                        {
                            block->type = replaceCurrentBlock;
                        }
                    }
                }
            }
            // Foot collisions
            {
                SDL_FRect footCollider{
                    position.x + hitbox.x,
                    clawbHeight + 1,
                    hitbox.w,
                    1
                };
                auto footCollisions = context.level.queryBlocksUnderCollider(footCollider);
                for (auto& collision : footCollisions)
                {
                    auto* block = context.level.getBlockAt(collision.coords);
                    std::string callback = collision.callback;

                    // Shiny can jump on blocks
                    if (callback.substr(0, 3) == "Box")
                    {
                        // Stepping is not enough. Falling onto must be happening
                        if (physics->oldVelocity.y > 0)
                        {
                            // Make Shiny jump
                            physics->velocity.y = -physics->abilities.jumpSpeed;

                            // Allow for holding the jump button to jump higher
                            physics->jumpTimer = physics->abilities.jumpStrength;

                            // Replace the box with what there is inside
                            block->type = makeBoxNumber(collision.callback);
                        }
                    }

                    // Shiny can terraform blocks he steps on

                    if (callback.substr(0, 9) == "Terraform")
                    {
                        auto rightArrowIndex = callback.find('>');
                        auto upArrowIndex = callback.find('^');
                        auto dashIndex = callback.find('-');

                        // Coordinates of the block under Shiny's clawbs
                        auto colSpot = collision.coords;
                        // Coordinates of the block above the block that's under Shiny's clawbs
                        auto upperBlockCoords = Level::Block::Coords(
                            colSpot.column,
                            colSpot.row - 1
                        );
                        // Block that's above the block that's under Shiny's clawbs
                        auto upperBlock = context.level.getBlockAt(upperBlockCoords);

                        // Terraform current block (Terraform->X or Terraform^Y->X)
                        if (rightArrowIndex != std::string::npos)
                        {
                            std::size_t start = rightArrowIndex + 1;
                            std::size_t end = callback.size();

                            // If there's also a '^' after '>', stop before it
                            if (upArrowIndex != std::string::npos && upArrowIndex > rightArrowIndex)
                            {
                                end = upArrowIndex;
                            }

                            try
                            {
                                std::string numberSubstring = callback.substr(start, end - start);
                                int newType = std::stoi(numberSubstring);
                                if (block)
                                { // Terraform!
                                    block->type = newType;
                                }
                            }
                            catch (...)
                            {
                                // Ignore malformed number, do nothing
                            }
                        }

                        // Terraform upper block (Terraform^Y or Terraform->X^Y)
                        if (upArrowIndex != std::string::npos)
                        {
                            std::size_t start = upArrowIndex + 1;
                            std::size_t end = callback.size();

                            // If there's also a '>' after '^', stop before it
                            if (dashIndex != std::string::npos && dashIndex > upArrowIndex)
                            {
                                end = dashIndex;
                            }

                            try
                            {
                                std::string numberSubstring = callback.substr(start, end - start);
                                int newType = std::stoi(numberSubstring);
                                if (upperBlock)
                                { // Terraform!
                                    upperBlock->type = newType;
                                }
                            }
                            catch (...)
                            {
                                // Ignore malformed number, do nothing
                            }
                        }
                    }
                }
            }
            // Head collisions
            {
                SDL_FRect headCollider{
                    position.x + hitbox.x,
                    tipOfTheHead - 1,
                    hitbox.w,
                    1
                };
                auto headCollisions = context.level.queryBlocksUnderCollider(headCollider);
                for (auto& collision : headCollisions)
                {
                    auto* block = context.level.getBlockAt(collision.coords);

                    // Shiny can break blocks with his head
                    if (collision.callback.substr(0, 3) == "Box")
                    {
                        // Only if actually going up, not scratching the box with his head
                        if (physics->oldVelocity.y < 0)
                        {
                            // Headbonk
                            physics->velocity.y = abs(physics->oldVelocity.y);

                            // Replace the box with what there is inside
                            block->type = makeBoxNumber(collision.callback);
                        }
                    }
                }
            }
        }
    }

    if(amIDeadYet)
        die();

    // Animate Shiny
    animate(context);
}

int Shiny::makeBoxNumber(std::string callback) const
{
    auto rightArrowIndex=callback.find('>');

    try
    {
        std::string numberString = callback.substr(rightArrowIndex + 1);
        auto boxNumber = std::stoi(numberString);
        return boxNumber;
    }
    catch(...)
    {// Bad index
        // Default
        return 0;
    }
}

void Shiny::preDraw(DrawContext& context) const
{
}

void Shiny::postDraw(DrawContext& context) const
{
}

void Shiny::onDestroy(EntityStepContext& context)
{
	//std::cout << "Scalefloof go poof!" << std::endl;
}
