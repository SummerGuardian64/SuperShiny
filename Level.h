#pragma once
#include <vector>
#include <cstddef>
#include <SDL.h>
#include "DrawContext.h"
#include "SdlTexture.h"

namespace ssge
{
	class Level
	{
	public:
		class Block
		{
		public: // intentionally plain-old-data style
			enum class Collision
			{
				Air,
				Solid,
				Water,
				Hazard,
				DeathOnTouch,
				DeathIfFullyOutside,
				WrapAround,
				Warp,
				TOTAL
			};

			enum class Type
			{
				EMPTY,
				Grass,
				Ground,
				RedBricks,
				StoneFloor,
				PuzzlePavement,
				TOTAL
			};

			Type type{ Type::EMPTY };
			Collision collision{ Collision::Air };

			Block() = default;
			Block(const Block&) = default;
			Block(Block&&) = default;
			~Block() = default;

			int getTypeIndex() const; // index in the tileset (0-based) for draw()
		};

		class Iterator // (planned) rich iterator — declared for future use
		{
			Level& level;
			int column{ 0 };
			int row{ 0 };
			explicit Iterator(Level& lvl);
		public:
			// TODO: increment/decrement, directional moves, bounds helpers, deref, etc.
		};

	public:
		// Level never changes dimensions once constructed.
		const int columns;
		const int rows;
		const SDL_Rect blockSize; // width/height of a single block in pixels
		SdlTexture tileset;       // public on purpose (as requested)

	private:
		// Flat array allocated with columns * rows elements.
		Block* array{ nullptr };

		// Outside-collision policy per side/corner
	public:
		Block::Collision throughTopLeft{ Block::Collision::DeathIfFullyOutside };
		Block::Collision throughTop{ Block::Collision::DeathIfFullyOutside };
		Block::Collision throughTopRight{ Block::Collision::DeathIfFullyOutside };
		Block::Collision throughLeft{ Block::Collision::DeathIfFullyOutside };
		Block::Collision throughRight{ Block::Collision::DeathIfFullyOutside };
		Block::Collision throughBottomLeft{ Block::Collision::DeathIfFullyOutside };
		Block::Collision throughBottom{ Block::Collision::DeathIfFullyOutside };
		Block::Collision throughBottomRight{ Block::Collision::DeathIfFullyOutside };

		// Ctor / Dtor
		Level(int columns, int rows, SDL_Rect blockSize, SdlTexture tileset);
		~Level();

		// No copying (large arrays). Moves allowed.
		Level(const Level&) = delete;
		Level& operator=(const Level&) = delete;
		Level(Level&&) noexcept;
		Level& operator=(Level&&) noexcept;

		// Access
		inline int indexOf(int r, int c) const { return r * columns + c; }
		bool inBoundsRC(int r, int c) const;
		bool inBoundsPt(SDL_Point p) const;

		Block* getBlockAt(int row, int column);
		const Block* getConstBlockAt(int row, int column) const;

		Block* getBlockAt(SDL_Point point);
		const Block* getConstBlockAt(SDL_Point point) const;

		Block::Collision getCollisionAt(SDL_Point point) const;

		SDL_Rect calculateLevelSize() const; // total pixel rectangle of the level starting at (0,0)

		void draw(DrawContext context) const; // conservative draw (no templates)

		// Add near the end of class Level (public):
		static std::unique_ptr<Level> loadFromBytes(
			const unsigned char* data,
			std::size_t size,
			std::string* outTilesetPath = nullptr,
			std::string* outError = nullptr);
	};
}
