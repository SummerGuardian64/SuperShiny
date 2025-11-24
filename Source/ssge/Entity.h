#pragma once
#include "SDL.h"
#include <chrono>
#include "Sprite.h"
#include "InputPad.h"
#include <cstdint>
#include <string>

namespace ssge
{
	class EntityStepContext;
	class DrawContext;

	class Entity
	{
	public:
		class Control
		{
		public:
			Entity& entity;
			Control(Entity& entity);

			enum class Mode
			{
				None,
				Player,
				AI,
				Replay,
				NPC,
				Cutscene,
				TOTAL
			};

		private:
			Mode mode = Mode::None;
			bool _ignore = false;
			bool playable = false;
			int playerId = -1;
			//TBA: ControlAI

		private:
			InputPad pad;
		public:
			Mode getMode() const { return mode; };
			void setMode(Mode mode) {
				if ((int)mode >= 0 && mode < Mode::TOTAL) this->mode = mode;
			};
			bool isIgnored() const { return _ignore; };
			bool ignored() const { return _ignore; };
			bool setIgnore(bool ignore) { _ignore = ignore; return _ignore; };
			bool setIgnored(bool ignored) { _ignore = ignored; return _ignore; }
			void ignore(bool ignored = true) { _ignore = ignored; };
			void restore() { _ignore = false; };
			bool isPlayable() const { return playable; };
			void setPlayable(bool playable) { this->playable = playable; };
			void makePlayable() { playable = true; };
			void makeUnplayable() { playable = false; };
			int getPlayerId() const { return playerId; };
			void setPlayerId(int playerId) { this->playerId = playerId; };
			void makePlayableBy(int playerId)
			{
				setMode(Mode::Player);
				ignore(false);
				makePlayable();
				setPlayerId(playerId);
			}

			//The reason for decoupling is because maybe a player is controlling?
			//Maybe NPC/AI logic? Maybe it's a replay or a cutscene?
			//And it MUST be independent of the real input to avoid the pause abuse bug
			const InputPad& getPad() const { return pad; };

			//TBA: InputManager will call all latch functions
			//void latch(InputContext context);
			void latch(EntityStepContext& context);
		};
		class NPC
		{
			// This will have NPC code execute

		public:
			Entity& entity;
			NPC(Entity& entity);
			void step(EntityStepContext& context);

			// These are the inputs that will be used to latch the controls into InputPad
			// Think of it like the NPC's actual joystick
			uint32_t directInputs;
		};
		class Physics
		{
		public:
			// Limits of physical movement
			struct Abilities
			{
				// Ability flags
				enum class Flag : uint32_t {
					None = 0u,
					EnablePhysics = 1u << 0,
					EnableHorizontalMove = 1u << 1,
					EnableVerticalMove = 1u << 2,
					EnableJump = 1u << 3,
					EnableRun = 1u << 4,
					EnableSwim = 1u << 5,
					EnableHorizontalCollision = 1u << 6,
					EnableVerticalCollision = 1u << 7,
					IgnoreCollision = 1u << 8,
					EnableHorizontalBounce = 1u << 9,
					EnableVerticalBounce = 1u << 10,
					EnableGMBounce = 1u << 11,
					EnableGMCollision = 1u << 12,
				};

				// bitwise ops for Flag
				friend constexpr Flag operator|(Flag a, Flag b) {
					return static_cast<Flag>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
				}
				friend constexpr Flag operator&(Flag a, Flag b) {
					return static_cast<Flag>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
				}
				friend constexpr Flag operator~(Flag a) {
					return static_cast<Flag>(~static_cast<uint32_t>(a));
				}

				// storage
				Flag bits = Flag::None;

				// core helpers
				constexpr bool has(Flag f) const noexcept {
					return (static_cast<uint32_t>(bits) & static_cast<uint32_t>(f)) != 0u;
				}
				void set(Flag f) noexcept {
					bits = bits | f;
				}
				void reset(Flag f) noexcept {
					bits = static_cast<Flag>(static_cast<uint32_t>(bits) & ~static_cast<uint32_t>(f));
				}
				void set(Flag f, bool on) noexcept {
					on ? set(f) : reset(f);
				}
				void clear() noexcept { bits = Flag::None; }

				// sugar getters (read nicely at callsites)

				bool physicsEnabled()        const noexcept { return has(Flag::EnablePhysics); }
				bool horzMove()              const noexcept { return has(Flag::EnableHorizontalMove); }
				bool vertMove()              const noexcept { return has(Flag::EnableVerticalMove); }
				bool canJump()               const noexcept { return has(Flag::EnableJump); }
				bool canRun()                const noexcept { return has(Flag::EnableRun); }
				bool canSwim()               const noexcept { return has(Flag::EnableSwim); }
				bool horzCollision()         const noexcept { return has(Flag::EnableHorizontalCollision); }
				bool vertCollision()         const noexcept { return has(Flag::EnableVerticalCollision); }
				bool collisionIgnored()      const noexcept { return has(Flag::IgnoreCollision); }
				bool horzBounce()            const noexcept { return has(Flag::EnableHorizontalBounce); }
				bool vertBounce()            const noexcept { return has(Flag::EnableVerticalBounce); }
				bool gmBounce()              const noexcept { return has(Flag::EnableGMBounce); }
				bool gmCollision()           const noexcept { return has(Flag::EnableGMCollision); }

				// optional sugar setters (chainable)

				Abilities& enable(Flag f) { set(f); return *this; }
				Abilities& disable(Flag f) { reset(f); return *this; }
				Abilities& enable(Flag f, bool on) { set(f, on); return *this; }

				// ---- current limits (your original fields) ----

				float      maxSpeedHor = 0.f;  // Maximum horizontal-movement speed
				float      maxSpeedUp = 0.f;   // Maximum upward speed
				float      maxSpeedDown = 0.f; // Maximum downward speed
				SDL_FPoint acc = { 0,0 };      // Acceleration
				SDL_FPoint dec = { 0,0 };      // Deceleration
				float      jumpSpeed = 0.f;    // Upward speed while jumpTimer>0
				double     jumpStrength = 0.0; // Alloted time for holding jump
				float      swimPower = 0.f;    // ???
				float      gravity = 0.f;      // Gravity is calculated like downward acceleration
			};

			// This will have all physics variables

			//TODO: Harden later
		public:
			// References Entity's position
			SDL_FPoint& position;
			// References Entity's hitbox
			SDL_FRect& hitbox;

			SDL_FPoint setVelocityMagnitude(const SDL_FPoint& v, float speed);
			//std::unique_ptr<Collider> collider; //TODO: May be not needed
			float getLeftEdge() const { return hitbox.x; }
			float getRightEdge() const { return hitbox.x + hitbox.w; }
			float getTopEdge() const { return hitbox.y; }
			float getBottomEdge() const { return hitbox.y + hitbox.h; }
			float getHorizontalCenter() const { return hitbox.x + hitbox.w / 2.0f; }
			float getVerticalCenter() const { return hitbox.y + hitbox.h / 2.0f; }
			SDL_FRect getBounds() const { return hitbox; }
			SDL_FPoint getSize() const { return SDL_FPoint{ hitbox.w, hitbox.h }; }
			//std::shared_ptr<Collision> collideWith(Entity& other); //TODO: May be not needed

			// Current abilities
			Abilities abilities;

			// Current states
			bool running = false;
			bool inWater = false;
			bool grounded = false;
			bool touchesWall = false;

			// Current variables

			SDL_FPoint velocity = { 0,0 }; // Current speed as a vector
			double jumpTimer = 0;       // Time remaining for holding jump
			SDL_Point dir = { 0,0 };    // Direction of movement
			SDL_Point side = { 0,0 };   // Side that's being pushed
			SDL_FPoint oldVelocity = { 0,0 };

			//float gravityInWater = 0;

			//float maxWalkSpeed = 0;
			//float maxRunSpeed = 0;
			//float maxAirWalkSpeed = 0;
			//float maxAirRunSpeed = 0;
			//float maxAirJumpSpeed = 0;
			//float maxAirFallSpeed = 0;
			//float maxWalkInWaterSpeed = 0;
			//float maxRunInWaterSpeed = 0;
			//float maxWaterJumpSpeed = 0;
			//float maxWaterFallSpeed = 0;
			//float maxSwimSpeed = 0;

			//SDL_FPoint accWalk = { 0,0 };
			//SDL_FPoint accRun = { 0,0 };
			//SDL_FPoint accAir = { 0,0 };
			//SDL_FPoint accWater = { 0,0 };
			//SDL_FPoint accWaterWalk = { 0,0 };
			//SDL_FPoint accAirRun = { 0,0 };
			//SDL_FPoint accWaterRun = { 0,0 };

			//SDL_FPoint decWalk = { 0,0 };
			//SDL_FPoint decRun = { 0,0 };
			//SDL_FPoint decAir = { 0,0 };
			//SDL_FPoint decWater = { 0,0 };
			//SDL_FPoint decWaterWalk = { 0,0 };
			//SDL_FPoint decAirRun = { 0,0 };
			//SDL_FPoint decWaterRun = { 0,0 };

		public:
			Entity& entity;
			Physics(Entity& entity);
			void step(EntityStepContext& context);
		};

	public:

		uint32_t lifespan = 0;
		bool scheduledToDestroy{ false };

	protected:
		std::unique_ptr<Control> control;
		std::unique_ptr<Physics> physics;

	public:
		Entity();
		virtual std::string getEntityClassID() const = 0;
		uint32_t getLifespan() const { return lifespan; }
		SDL_FPoint position{ 0.0f, 0.0f };
		std::unique_ptr<Sprite> sprite;
		SDL_FRect hitbox{ 0.0f, 0.0f, 0.0f, 0.0f };

		Control* getControl();
		InputPad getPad();
		Physics* getPhysics();

		void latch(EntityStepContext& context);
		virtual void firstStep(EntityStepContext& context) = 0;
		virtual void preStep(EntityStepContext& context) = 0;
		void step(EntityStepContext& context);
		bool isScheduledToDestroy() const { return scheduledToDestroy; }
		virtual void postStep(EntityStepContext& context) = 0;
		virtual void preDraw(DrawContext& context) const = 0;
		void draw(DrawContext context) const;
		virtual void postDraw(DrawContext& context) const = 0;
		void destroy();
		virtual void onDestroy(EntityStepContext& context) = 0;
		virtual ~Entity() = default;
	};
}
