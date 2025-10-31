#include "Game.h"
#include "Shiny.h"
#include "Orb.h"
#include "Level.h"

using namespace ssge;

const char Game::APPLICATION_TITLE[] = "Super Shiny";

bool Game::init(SDL_Renderer* renderer)
{
	static bool initialized = false;
	if (initialized)
		return false;

	bool success = true;

	// Initializing sprites must be successful or else nothing is
	success &= Sprites::_init(renderer);

	return success;
}

const char* Game::getApplicationTitle()
{
    return APPLICATION_TITLE;
}

// --------------------------------------------------
// DEFINE THE STATIC FACTORY REGISTRY
// --------------------------------------------------

// NOTE: keep the order exactly the same as EntityClassID enum.
Game::Entities::FactoryFn Game::Entities::registry[] =
{
	/* 0: EntityClassID::None or Invalid */ nullptr,
	/* 1: EntityClassID::Shiny           */ []() -> std::shared_ptr<Entity> { return std::make_shared<Shiny>(); },
	/* 2: EntityCLassID::Orb             */ []() -> std::shared_ptr<Entity> { return std::make_shared<Orb>(); },
	/* 3: EntityClassID::TOTAL           */ nullptr, // Consistency-sake
};

// Now we can safely compute its size here.
int Game::Entities::getRegistrySize()
{
    return static_cast<int>(sizeof(registry) / sizeof(registry[0]));
}

bool Game::Sprites::_init(SDL_Renderer* renderer)
{
	using Image = Sprite::Image;
	using Sequence = Sprite::Animation::Sequence;

	sprdefShiny.spritesheet = SdlTexture("Sprites/Shiny.png", renderer);

	for (int y = 0; y < 1024 - 102; y += 102)
	{
		for (int x = 0; x <= 1024 - 77; x += 77)
		{
			sprdefShiny.images.push_back(Image(x, y, 77, 102, 41, 96));
		}
	}
	sprdefShiny.images[15]=Image(154, 102, 57, 92, 30, 98);

	{ // Stopped
		auto& seq = sprdefShiny.addSequence(20, 20, 0);
		seq.imageIndexes.push_back(0);
	}
	{ // Walking
		auto& seq = sprdefShiny.addSequence(20, 10, 0);
		for (int i = 1; i <= 14; i++)
		{
			seq.imageIndexes.push_back(i);
		}
	}
	{ // Jumping
		auto& seq = sprdefShiny.addSequence(20, 20, 0);
		seq.imageIndexes.push_back(15);
	}
	{ // Falling
		auto& seq = sprdefShiny.addSequence(20, 20, 0);
		seq.imageIndexes.push_back(15);
	}

	sprdefOrb.spritesheet = SdlTexture("Sprites/Orb.png", renderer);

	sprdefOrb.images.push_back(Image(0, 0, 64, 64, 32, 32));
	{
		auto& seq = sprdefOrb.addSequence(20, 20, 0);
		seq.imageIndexes.push_back(0);
	}

	return true;
}

Sprite::Definition Game::Sprites::sprdefShiny;
Sprite::Definition Game::Sprites::sprdefOrb;

Sprite::Definition& Game::Sprites::shiny() { return sprdefShiny; }

Sprite::Definition& Game::Sprites::orb() { return sprdefOrb; }
