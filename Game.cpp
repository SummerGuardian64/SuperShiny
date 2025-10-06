#include "Game.h"
#include "Shiny.h"

using namespace ssge;

const char Game::APPLICATION_TITLE[] = "Super Shiny";

bool ssge::Game::init(SDL_Renderer* renderer)
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
	/* 3: EntityClassID::TOTAL           */ nullptr, // Consistency-sake
};

// Now we can safely compute its size here.
int Game::Entities::getRegistrySize()
{
    return static_cast<int>(sizeof(registry) / sizeof(registry[0]));
}

bool ssge::Game::Sprites::_init(SDL_Renderer* renderer)
{
	using Image = ssge::Sprite::Image;
	using Sequence = ssge::Sprite::Animation::Sequence;

	//sprdefShiny.spritesheet
	sprdefShiny.images.push_back(Image(0, 0, 64, 64, 32, 32));
	{
		auto& seq = sprdefShiny.addSequence(20, 20, 0);
		seq.imageIndexes.push_back(0);
	}

	return true;
}

Sprite::Definition Game::Sprites::sprdefShiny;

Sprite::Definition& ssge::Game::Sprites::shiny()
{
	return sprdefShiny;
}
