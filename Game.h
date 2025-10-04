#pragma once
#include <memory>
#include "Entity.h"

namespace ssge
{
	class Game
	{
		Game() = delete;
		Game(const Game&) = delete;
		Game(Game&&) = delete;
		~Game() = delete;

	public:
		static const int VIRTUAL_WIDTH = 1280;
		static const int VIRTUAL_HEIGHT = 720;
		static const char APPLICATION_TITLE[];
		const char* getApplicationTitle();

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
