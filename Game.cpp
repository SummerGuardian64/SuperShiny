#include "Game.h"
#include "Shiny.h"

using namespace ssge;

const char Game::APPLICATION_TITLE[] = "Super Shiny";

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
