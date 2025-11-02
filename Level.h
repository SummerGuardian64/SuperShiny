#pragma once
#include <vector>
#include <cstddef>
#include <SDL.h>
#include "DrawContext.h"
#include "SdlTexture.h"
#include "PassKey.h"

namespace ssge
{
	class GameWorld;

    class Level
	{
		static inline int worldToCol(float x, int tileW) { return (int)std::floor(x / (float)tileW); }
		static inline int worldToRow(float y, int tileH) { return (int)std::floor(y / (float)tileH); }
		static inline int colToWorld(int col, int tileW) { return col * tileW; }
		static inline int rowToWorld(int row, int tileH) { return row * tileH; }
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
				NextSection,
				TOTAL
			};

			struct Coords
			{
				int column = -1, row = -1;
				inline bool operator==(const Coords& other) {
					return column == other.column && row == other.row;
				}
				Coords() = default;
				Coords(int c, int r) : column(c), row(r) {};
			};


			// inside Level::Block, or wherever you keep it
			class Type
			{
				int index;
			public:
				// default = empty
				constexpr Type() : index(0) {}

				// explicit to avoid accidental narrowing from pointers
				explicit constexpr Type(int idx) : index(idx) {}

				// copy/move are fine as default
				Type(const Type&) = default;
				Type(Type&&) = default;

				// assignment from another Type
				Type& operator=(const Type&) = default;
				Type& operator=(Type&&) = default;

				// assignment from int
				Type& operator=(int value)
				{
					index = value;
					return *this;
				}

				// accessors
				
				int getIndex() const { return index; }
				int toInt()   const { return index; }
				int asInt()   const { return index; }

				// comparisons

				bool isEmpty() const { return *this == EMPTY; }
				
				bool operator==(int value) const { return index == value; }
				bool operator!=(int value) const { return index != value; }

				bool operator==(const Type& other) const { return index == other.index; }
				bool operator!=(const Type& other) const { return index != other.index; }

				// this is the magic so you can do: if (blockType) ...
				// "non-empty" == "true"
				explicit operator bool() const
				{
					return index != 0;
				}

				// static EMPTY
				static const Type EMPTY;				
			};

			struct Definition
			{
				// Tile to draw. -1 = Don't draw
				int tileIndex = -1;
				Collision collision;
				std::string callback;
				//TODO: Harden with PassKey
				Definition() = default;
			};

			Type type{ Type::EMPTY };

			Block() = default;
			Block(const Block&) = default;
			Block(Block&&) = default;
			~Block() = default;

			int getTypeIndex() const; // index in the tileset (0-based) for draw()
		};

		class Iterator // (planned) rich iterator � declared for future use
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
				if (columns == 0 || index == -1)
					return SDL_Rect{ 0,0,0,0 };

				SDL_Rect rect;
				rect.w = tileW;
				rect.h = tileH;
				rect.x = (index % columns) * tileW;
				rect.y = (index / columns) * tileH;
				return rect;
			}
		};

		struct BlockQuery
		{
			Level::Block::Coords coords;     // where is the block on the grid
			Level::Block::Type type = Level::Block::Type::EMPTY;
			Level::Block::Collision coll = Level::Block::Collision::Air;
			std::string callback;
			SDL_FPoint worldTouch = { 0,0 }; // where was the block touched in the world
			SDL_FPoint blockTouch = { 0,0 }; // where was the block touched in its own coords
			bool insideLevel = false;
		};

		struct SweepHit
		{
			bool hit = false;
			Level::Block::Coords coords;      // solid block we hit
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
		static const int MAX_BLOCK_DEFINITIONS = 94;
		Block::Definition blockDefinitions[MAX_BLOCK_DEFINITIONS];

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
		int nextSection;

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
		bool inBoundsPt(SDL_FPoint p) const;

		Block* getBlockAt(Level::Block::Coords coords);
		const Block* getConstBlockAt(Level::Block::Coords coordsn) const;

		Block* getBlockAt(SDL_FPoint point);
		const Block* getConstBlockAt(SDL_FPoint point) const;

		Block::Collision getCollisionAt(SDL_FPoint positionInLevel) const;
		Block::Collision getBlockCollisionType(const Block& block) const;
		std::string getBlockCallbackString(const Block& block) const;

		SDL_Rect calculateLevelSize() const; // total pixel rectangle of the level starting at (0,0)

        // Return block indices overlapped by a rect (clamped to level bounds).
        void rectToBlockSpan(const SDL_FRect& r, int& col0, int& col1, int& row0, int& row1) const;

		// Queries all blocks that overlap the specified collider
		std::vector<BlockQuery> queryBlocksUnderCollider(SDL_FRect collider) const;

        // Query a block (with OOB policy)
        BlockQuery queryBlock(int col, int row) const;

		// Query a block at specific position in level
		BlockQuery queryBlock(SDL_FPoint positionInLevel) const;

        // Is any overlapped tile "water"?
        bool rectInWater(const SDL_FRect& r) const;

        // Axis-separated sweep: move horizontally by dx, collide with solids.
        SweepHit sweepHorizontal(const SDL_FRect& rect, float dx) const;

        // Axis-separated sweep: move vertically by dy, collide with solids.
        SweepHit sweepVertical(const SDL_FRect& rect, float dy) const;

		const SdlTexture& getTilesetTexture() const;
		const TilesetMeta getTilesetMeta() const;
		void setTileset(SdlTexture& SdlTexture);
		std::string tilesetTexturePath;
		bool loadTileset(SDL_Renderer* renderer);

		void draw(DrawContext context) const; // conservative draw (no templates)

		class Loader
		{
			// INI-related stuff

			struct Item
			{
				std::string key;
				std::string value;
				Item() = default;
				Item(std::string key, std::string value) : key(key), value(value) {};
			};
			struct Section
			{
				std::string caption;
				std::vector<Item> items;
				Section() = default;
				Section(std::string caption) : caption(caption) {};
			};
			std::vector<Section> sections;

			// Loading and parsing
			// Please execute these functions in proper order!

			bool loadIni(const char* path); // Reads INI file into sections
			std::unique_ptr<Level> newLevel; // Level we're making
			std::unique_ptr<Level> parseAndConstruct(); // Parses arguments for Level ctor and creates the level
			bool parseAfterConstruct(); // Populates all data fields using sections
			bool parseBlockDefinitions(); // Parse Block definitions
			bool parseGrid(); // Parse grid
			bool parseSpawnList(); // Parse spawn list
			
			// Post-load traversal

			std::string getValue(std::string caption, std::string key) const;
			std::string getValue(std::string caption, std::string key, std::string fallback) const;
			std::string getValue(std::string caption, int numericKey) const;

			// Advanced parsing

			Block::Collision parseOOB(std::string side);
			Block::Collision parseCollision(std::string collisionString);
			Block::Collision parseBlockCollision(std::string blockItemValue);
			int parseTileIndex(std::string blockItemValue);
			std::string parseCallbackName(std::string blockItemValue);

			// Error logging
			std::string errorLog;
			void logError(std::string error);

			// Spawn list

		public:
			struct Spawn
			{
				SDL_FPoint where;
				std::string what;
				std::string callback;
			};
		private:
			std::vector<Spawn> spawnList;
			int heroIndex = -1;

		public:
			Loader(PassKey<GameWorld> pk);
			std::unique_ptr<Level> loadLevel(const char* path);
			const std::vector<Spawn>& getSpawnList() const;
			int getHeroIndex() const;
			std::string getErrorLog() const;
			Loader(const Loader& toCopy) = default;
			Loader(Loader&& toMove) = default;
			~Loader() = default;
		};
	};
}
