#pragma once
#include "SDL.h"
#include <chrono>
#include "Sprite.h"
#include "InputPad.h"

namespace ssge
{
	enum class EntityClassID {
		UNKNOWN,
		Shiny, // Currently the only entity we have in the game
		TOTAL
	};

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

		//private: //TODO: Harden later
		public:
			Mode mode;
			bool ignore;
			bool playable;
			int playerId;
			//TBA: ControlAI

		private:
			InputPad pad;
		public:
			//TODO: Harden later. Make work now.
			/*Mode getMode() const { return mode; };
			void setMode(Mode mode) {
				if ((int)mode >= 0 || mode < Mode::TOTAL) this->mode = mode;
			};
			bool isIgnored() const { return ignore; };
			bool setIgnore(bool ignore) { this->ignore = ignore; };
			void setIgnored(bool ignored) { ignore = ignored; }
			void ignore() { ignore = true; };
			void restore() { ignore = false; };
			bool isPlayable() const { return playable; };
			void setPlayable(bool playable) { this->playable = playable; };
			void makePlayable() { playable = true; };
			void makeUnplayable() { playable = false; };
			int getPlayerId() const { return playerId; };
			void setPlayerId(int playerId) { this->playerId = playerId; };*/

			//The reason for decoupling is because maybe a player is controlling?
			//Maybe NPC/AI logic? Maybe it's a replay or a cutscene?
			//And it MUST be independent of the real input to avoid the pause abuse bug
			const InputPad& getPad() const { return pad; };

			//TBA: InputManager will call all latch functions
			//void latch(InputContext context);
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
			// This will have all physics variables

			//TODO: Harden later
		public:
			// References Entity's position
			SDL_FPoint& position;
			// References Entity's hitbox
			SDL_FRect& hitbox;

			SDL_FPoint velocity{ 0.0f, 0.0f };
			bool processVelocity{ true };
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
			float getDistance(const Entity& other) const;
			//std::shared_ptr<Collision> collideWith(Entity& other); //TODO: May be not needed

		public:
			Entity& entity;
			Physics(Entity& entity);
			void step(EntityStepContext& context);
		};

	public:

		uint32_t lifespan{ 0 };
		bool scheduledToDestroy{ false };

	protected:
		std::unique_ptr<Control> control;
		std::unique_ptr<Physics> physics;

	public:
		Entity();
		virtual EntityClassID getEntityClassID() const = 0;
		uint32_t getLifespan() const { return lifespan; }
		SDL_FPoint position{ 0.0f, 0.0f };
		std::unique_ptr<Sprite> sprite;
		SDL_FRect hitbox{ 0.0f, 0.0f, 0.0f, 0.0f };
		
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