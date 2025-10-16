#include "Game.h"
#include "Shiny.h"
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

	sprdefShiny.images.push_back(Image(0, 0, 128, 128, 64, 64));
	{
		auto& seq = sprdefShiny.addSequence(20, 20, 0);
		seq.imageIndexes.push_back(0);
	}

	return true;
}

Sprite::Definition Game::Sprites::sprdefShiny;

Sprite::Definition& Game::Sprites::shiny()
{
	return sprdefShiny;
}

// Example static level data (see below for format & builder)
extern const unsigned char g_Level_01[] =
{
	'S','S','G','E','L','E','V','1', // Magic number "SSGELEV1"
	16,0,0,0, // columns
	16,0,0,0, // rows
	32,0,0,0, // blockWidth
	32,0,0,0, // blockHeight
	(unsigned char)Level::Block::Collision::Solid,
	(unsigned char)Level::Block::Collision::Air,
	(unsigned char)Level::Block::Collision::Solid,
	(unsigned char)Level::Block::Collision::Solid,
	(unsigned char)Level::Block::Collision::Solid,
	(unsigned char)Level::Block::Collision::Solid,
	(unsigned char)Level::Block::Collision::Solid,
	(unsigned char)Level::Block::Collision::Solid,
	21,0,'T','i','l','e','s','e','t','s','\\','k','i','p','t','i','l','e','s','.','p','n','g',
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,4,1,4,1,5,1,5,1,5,1,5,1,3,1,3,1,3,1,0,0,0,0,
	0,0,0,0,0,0,0,0,4,1,4,1,4,1,5,1,5,1,5,1,5,1,2,1,2,1,2,1,3,1,3,1,
	4,1,4,1,4,1,4,1,4,1,4,1,4,1,2,1,2,1,2,1,2,1,2,1,2,1,2,1,2,1,2,1,
	2,1,2,1,2,1,2,1,2,1,2,1,2,1,2,1,2,1,2,1,2,1,2,1,2,1,2,1,2,1,2,1
};
extern const std::size_t   g_Level_01_Size = sizeof(g_Level_01);

const unsigned char* Game::Levels::getBytes(int id, std::size_t& outSize)
{
	switch (id) {
	case 1: outSize = g_Level_01_Size; return g_Level_01;
	default: outSize = 0; return nullptr;
	}
}
