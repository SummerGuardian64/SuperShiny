#pragma once
#include <memory>
#include "Entity.h"
#include "SDL.h"

namespace ssge
{
	class Sprite;

	class Game
	{
		Game() = delete;
		Game(const Game&) = delete;
		Game(Game&&) = delete;
		~Game() = delete;

	public:
		static bool init(SDL_Renderer* renderer);

		static const int VIRTUAL_WIDTH = 1280;
		static const int VIRTUAL_HEIGHT = 720;
		static const char APPLICATION_TITLE[];
		const char* getApplicationTitle();

		class Sprites
		{
			Sprites() = delete;
			Sprites(const Sprites& toCopy) = delete;
			Sprites(Sprites&& toMove) = delete;
			~Sprites() = delete;

			friend class Game;
			static bool _init(SDL_Renderer* renderer);
			static Sprite::Definition sprdefShiny;
		public:
			static Sprite::Definition& shiny();
		};

		class Entities
		{
			Entities() = delete;
			Entities(const Entities&) = delete;
			Entities(Entities&&) = delete;
			~Entities() = delete;

		public:
			using FactoryFn = std::shared_ptr<Entity>(*)();

			// Declared, but not defined here
			static FactoryFn registry[];

			// We’ll declare a function that returns the array size.
			static int getRegistrySize();

			static std::shared_ptr<Entity> create(EntityClassID id)
			{
				int index = static_cast<int>(id);
				if (index < 0)
					return nullptr;

				int registrySize = getRegistrySize(); // call helper
				if (index >= registrySize)
					return nullptr;

				FactoryFn fn = registry[index];
				return fn ? fn() : nullptr;
			}
		};
	};
}
