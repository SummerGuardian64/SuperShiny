#pragma once

namespace ssge
{
	class Game // This class holds game-specific information
	{
		Game() = delete;
		Game(const Game& toCopy) = delete;
		Game(Game&& toMove) = delete;
		~Game() = delete;

	public:
		// Application-specific details
		static const int VIRTUAL_WIDTH = 1280;
		static const int VIRTUAL_HEIGHT = 720;
		static const char APPLICATION_TITLE[];
		const char* getApplicationTitle();
		
		class Sprites // Registry of sprite definitions for this game
		{
			Sprites() = delete;
			Sprites(const Sprites& toCopy) = delete;
			Sprites(Sprites&& toMove) = delete;
			~Sprites() = delete;
		};

	};
}