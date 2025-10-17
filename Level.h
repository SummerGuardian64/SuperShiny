#pragma once
#include <vector>
#include <cstddef>
#include <SDL.h>
#include "DrawContext.h"
#include "SdlTexture.h"

namespace ssge
{
    struct TileIndex { int col = -1, row = -1; };

    inline bool operator==(const TileIndex& a, const TileIndex& b) {
        return a.col == b.col && a.row == b.row;
    }

    // Mapping helpers:
    inline int worldToCol(float x, int tileW) { return (int)std::floor(x / (float)tileW); }
    inline int worldToRow(float y, int tileH) { return (int)std::floor(y / (float)tileH); }

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

		struct TilesetMeta
		{
			int tileW = 0;
			int tileH = 0;
			int columns = 0;  // tiles per row; 0 means "infer after load"
			bool isValid() const { return tileW > 0 && tileH > 0; }
			void inferColumnsFromTexture(const SdlTexture& tilesetTexture)
			{
				int tilesetWidth = 0, tilesetHeight = 0;
				if (!tilesetTexture || !isValid())
					return;

				SDL_QueryTexture(
					tilesetTexture,
					nullptr,
					nullptr,
					&tilesetWidth,
					&tilesetHeight
				);

				// Depending on how many whole tiles fit into the width of the texture,
				// that's how many columns we shall have
				columns = tilesetWidth / tileW;
			}
			SDL_Rect makeRectForTile(int index) const
			{
				if (columns == 0)
					return SDL_Rect{ 0,0,0,0 };

				SDL_Rect rect;
				rect.w = tileW;
				rect.h = tileH;
				rect.x = (index % columns) * tileW;
				rect.y = (index / columns) * tileH;
				return rect;
			}
		};

		struct BlockQuery {
			TileIndex tile;                   // which tile we touched/checked
			Level::Block::Type type = Level::Block::Type::EMPTY;
			Level::Block::Collision coll = Level::Block::Collision::Air;
			bool insideLevel = false;
		};

		struct SweepHit {
			bool hit = false;
			TileIndex tile;                   // solid tile we hit
			float newX = 0.f;                 // resolved x (after axis move)
			float newY = 0.f;                 // resolved y (after axis move)
		};

	public:
		// Level never changes dimensions once constructed.
		const int columns;
		const int rows;
		const SDL_Rect blockSize; // width/height of a single block in pixels
	private:
		SdlTexture tileset;
		TilesetMeta tilesMeta;

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

        // Return tile indices overlapped by a rect (clamped to level bounds).
        void rectToTileSpan(const SDL_FRect& r, int& col0, int& col1, int& row0, int& row1) const;

        // Query a tile (with OOB policy)
        BlockQuery queryTile(int col, int row) const;

        // Is any overlapped tile "water"?
        bool rectInWater(const SDL_FRect& r) const;

        // Axis-separated sweep: move horizontally by dx, collide with solids.
        SweepHit sweepHorizontal(const SDL_FRect& rect, float dx) const;

        // Axis-separated sweep: move vertically by dy, collide with solids.
        SweepHit sweepVertical(const SDL_FRect& rect, float dy) const;

		const SdlTexture& getTilesetTexture() const;
		const TilesetMeta getTilesetMeta() const;
		void setTileset(SdlTexture& SdlTexture);

		void draw(DrawContext context) const; // conservative draw (no templates)

		// Add near the end of class Level (public):
		static std::unique_ptr<Level> loadFromBytes(
			const unsigned char* data,
			std::size_t size,
			std::string* outTilesetPath = nullptr,
			std::string* outError = nullptr);
	};
}
