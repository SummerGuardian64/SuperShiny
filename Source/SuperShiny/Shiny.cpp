#include "Shiny.h"
#include "../ssge/StepContext.h"
#include "../ssge/DrawContext.h"
#include <iostream>
#include <memory>
#include "../ssge/EntityManager.h"
#include "../ssge/Utilities.h"
#include "SDL.h"

using ssge::sign;
using ssge::Level;

void Shiny::startBubbling()
{
    if (!bubbling)
    {
        bubbling = true;
        bubbleTimer = bubbleDelay;
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
        physics->abilities = makeRegularAbilities();
    }
}

void Shiny::animate(ssge::EntityStepContext& context)
{
    if (sprite && physics)
    {
        // Flip the sprite according to entity's facing direction (side)
        auto direction = physics->side.x;
        if (direction == 0) { sign(physics->velocity.x); }
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

ssge::Entity::Physics::Abilities Shiny::makeRegularAbilities() const
{
    using Abilities = ssge::Entity::Physics::Abilities;
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

ssge::Entity::Physics::Abilities Shiny::makeBubblingAbilities() const
{
    using Abilities = ssge::Entity::Physics::Abilities;
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
	physics = std::make_unique<ssge::Entity::Physics>(*this);
    control = std::make_unique<ssge::Entity::Control>(*this);

    control->mode = ssge::Entity::Control::Mode::Player;
    control->playable = true;
    control->playerId = 0;

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

    // Boing up into the air and have no control over it
    physics->velocity.x = 0;
    physics->velocity.y = -20;
    physics->abilities.maxSpeedUp = 20;
    physics->abilities.enable(ssge::Entity::Physics::Abilities::Flag::IgnoreCollision);
    control->ignore = true;
}

std::string Shiny::getEntityClassID() const
{
	return "Shiny";
}

void Shiny::firstStep(ssge::EntityStepContext& context)
{
    sprite = context.sprites.create("Shiny");
	//std::cout << "Helloy!" << std::endl;
}

void Shiny::preStep(ssge::EntityStepContext& context)
{
	//std::cout << "scale.. ";

    if (auto ctrl = control.get())
    {
        ssge::InputPad pad = ctrl->getPad();

        if (pad.isJustPressed(5))
        {
            if (auto orbRef = context.entities.addEntity("Orb"))
            {
                auto& orb = *orbRef.get();
                orb.position = position;
                orb.position.y -= 40;

                if(sprite->xscale<0)
                    orb.getPhysics()->velocity.x *= -1;
            }
        }

        if (pad.isPressed(6))
        {
            // Bubbleshooting
            if (physics->grounded && physics->side.x == 0)
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

        if (bubbling)
        {
            if (physics->velocity.y != 0)
            {
                quitBubbling();
            }
            else
            {
                bool up = pad.isPressed(0);
                bool down = pad.isPressed(1);
                bool left = pad.isPressed(2);
                bool right = pad.isPressed(3);

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
                        bubbleX = sprite->xscale;
                    }
                }

                float bubbleSpeed = 10;

                if (bubbleX * bubbleY != 0)
                { // Diagonal
                    bubbleSpeed = sqrtf(10);
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

                if (bubbleX + bubbleY != 0)
                {
                    SDL_FPoint bubbleVector{
                        (float)bubbleX * bubbleSpeed,
                        (float)bubbleY * bubbleSpeed
                    };

                    if (bubbleTimer > 0)
                    {
                        bubbleTimer--;
                    }
                    else
                    {
                        bubbleTimer = bubbleDelay;
                        // TODO: BUBBLE!

                    }
                }
            }
        }
    }

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

void Shiny::postStep(ssge::EntityStepContext& context)
{
    bool amIDeadYet = false;
    float tipOfTheHead = position.y + hitbox.y;
    float clawbHeight = tipOfTheHead + hitbox.h;

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
                    if (collision.coll == Level::Block::Collision::Hazard
                        || collision.coll == Level::Block::Collision::DeathOnTouch
                        || collision.coll == Level::Block::Collision::DeathIfFullyOutside)
                    {
                        // Eliminate ugly edge case
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
                        { } // Ugly edge case
                        else if (block != context.level.getBlockAt(bottomRight)
                            && block == context.level.getBlockAt(bottomRightOut))
                        { } // Another ugly edge case
                        else
                        {
                            amIDeadYet = true;
                        }
                    }
                    if (collision.callback == "Collectable")
                    {
                        block->type = 0;
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
                    if (callback.substr(0, 3) == "Box")
                    {
                        if (physics->oldVelocity.y > 0)
                        {
                            physics->velocity.y = -physics->abilities.jumpSpeed;
                            physics->jumpTimer = physics->abilities.jumpStrength;
                            block->type = 0;
                        }
                    }
                    if (callback.substr(0, 9) == "Terraform")
                    {
                        auto rightArrowIndex = callback.find('>');
                        auto upArrowIndex = callback.find('^');

                        // TODO: Terraformation
                        // A BlockType that's terraformable will have a callback like this:
                        // Terraform->10
                        // Terraform->14^22
                        // Terraform^43
                        // Terraform^28->7
                        // Number after -> means that the current block will be terraformed to that
                        block->type = 0; // TODO: Terraform to the correct number!
                        // Number after ^ means that the block above the stepped one will be terraformed
                        auto colSpot = collision.coords;
                        auto upperBlockCoords = Level::Block::Coords(
                            colSpot.column,
                            colSpot.row - 1
                        );
                        auto upperBlock = context.level.getBlockAt(upperBlockCoords);
                        upperBlock->type = 0; // TOO: Terraform to the correct number
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
                    if (collision.callback.substr(0, 3) == "Box")
                    {
                        if (physics->oldVelocity.y < 0)
                        {
                            physics->velocity.y = -physics->oldVelocity.y;
                            block->type = 0;
                        }
                    }
                }
            }
        }
    }

    if(amIDeadYet)
        die();

    animate(context);
}

void Shiny::preDraw(ssge::DrawContext& context) const
{
}

void Shiny::postDraw(ssge::DrawContext& context) const
{
}

void Shiny::onDestroy(ssge::EntityStepContext& context)
{
	//std::cout << "Scalefloof go poof!" << std::endl;
}
