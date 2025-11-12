#include "Level.h"
#include <cstring>
#include <algorithm>
#include <string>
#include "SdlTexture.h"
#include <fstream>
#include <vector>

namespace ssge
{
	const Level::Block::Type Level::Block::Type::EMPTY{ 0 };

	// ---------------- Block ----------------

	int Level::Block::getTypeIndex() const
	{
		return type.getIndex();
	}

	// -------------- Iterator ---------------
	Level::Iterator::Iterator(Level& lvl) : level(lvl) {}

	// --------------- Level -----------------
	Level::Level(int cols, int rws, SDL_Rect blkSize, SdlTexture tex)
		: columns(cols)
		, rows(rws)
		, blockSize(blkSize)
		, tileset(std::move(tex))
		, nextSection(0)
	{
		const std::size_t count = static_cast<std::size_t>(columns) * static_cast<std::size_t>(rows);
		array = (count > 0) ? new Block[count] : nullptr;
		tilesMeta.tileW = blockSize.w;
		tilesMeta.tileH = blockSize.h;

		for (std::size_t i = 0; i < MAX_BLOCK_DEFINITIONS; i++)
		{
			blockDefinitions[i] = Block::Definition();
		}
	}

	Level::~Level()
	{
		delete[] array;
		array = nullptr;
	}

	Level::Level(Level&& other) noexcept
		: columns(other.columns)
		, rows(other.rows)
		, blockSize(other.blockSize)
		, tileset(std::move(other.tileset))
		, array(other.array)
		, throughTopLeft(other.throughTopLeft)
		, throughTop(other.throughTop)
		, throughTopRight(other.throughTopRight)
		, throughLeft(other.throughLeft)
		, throughRight(other.throughRight)
		, throughBottomLeft(other.throughBottomLeft)
		, throughBottom(other.throughBottom)
		, throughBottomRight(other.throughBottomRight)
		, nextSection(other.nextSection)
		, backgrounds(std::move(other.backgrounds))
	{
		other.array = nullptr;

		for (std::size_t i = 0; i < MAX_BLOCK_DEFINITIONS; i++)
		{
			blockDefinitions[i] = std::move(other.blockDefinitions[i]);
		}
	}

	Level& Level::operator=(Level&& other) noexcept
	{
		if (this != &other)
		{
			delete[] array;
			// const members (columns/rows/blockSize) cannot be assigned � but
			// we only allow move on a freshly-constructed temporary in practice.
			// If you truly need assignment, drop the const on these dims.
			// Here we just adopt the underlying storage and public resources:
			tileset = std::move(other.tileset);
			array = other.array;
			throughTopLeft = other.throughTopLeft;
			throughTop = other.throughTop;
			throughTopRight = other.throughTopRight;
			throughLeft = other.throughLeft;
			throughRight = other.throughRight;
			throughBottomLeft = other.throughBottomLeft;
			throughBottom = other.throughBottom;
			throughBottomRight = other.throughBottomRight;
			other.array = nullptr;
			for (std::size_t i = 0; i < MAX_BLOCK_DEFINITIONS; i++)
			{
				blockDefinitions[i] = std::move(other.blockDefinitions[i]);
			}
		}
		return *this;
	}

	bool Level::inBoundsRC(int r, int c) const
	{
		return (r >= 0 && r < rows && c >= 0 && c < columns);
	}

	bool Level::inBoundsPt(SDL_FPoint p) const
	{
		if (p.x < 0 || p.y < 0) return false;
		const SDL_Rect size = calculateLevelSize();
		return p.x < size.w && p.y < size.h;
	}

	Level::Block* Level::getBlockAt(Level::Block::Coords coords)
	{
		if (!array || !inBoundsRC(coords.row, coords.column)) return nullptr;
		return &array[indexOf(coords.row, coords.column)];
	}

	const Level::Block* Level::getConstBlockAt(Level::Block::Coords coords) const
	{
		if (!array || !inBoundsRC(coords.row, coords.column)) return nullptr;
		return &array[indexOf(coords.row, coords.column)];
	}

	Level::Block* Level::getBlockAt(SDL_FPoint point)
	{
		if (!array || !inBoundsPt(point)) return nullptr;
		const int c = (int)point.x / blockSize.w;
		const int r = (int)point.y / blockSize.h;
		return getBlockAt(Block::Coords(c, r));
	}

	const Level::Block* Level::getConstBlockAt(SDL_FPoint point) const
	{
		if (!array || !inBoundsPt(point)) return nullptr;
		const int c = (int)point.x / blockSize.w;
		const int r = (int)point.y / blockSize.h;
		return getConstBlockAt(Block::Coords(c, r));
	}

	Level::Block::Collision Level::getCollisionAt(SDL_FPoint point) const
	{
		const SDL_Rect size = calculateLevelSize();
		if (point.y < 0)
		{
			if (point.x < 0) return throughTopLeft;
			if (point.x >= size.w) return throughTopRight;
			return throughTop;
		}
		if (point.y >= size.h)
		{
			if (point.x < 0) return throughBottomLeft;
			if (point.x >= size.w) return throughBottomRight;
			return throughBottom;
		}
		if (point.x < 0) return throughLeft;
		if (point.x >= size.w) return throughRight;

		const Block* b = getConstBlockAt(point);
		return b ? getBlockCollisionType(*b) : Block::Collision::Air;
	}

	Level::Block::Collision Level::getBlockCollisionType(const Block& block) const
	{
		// Get the block's type index
		auto blockTypeIndex = block.getTypeIndex();

		// Treat invalid index same way as 0-index
		if (blockTypeIndex < 0 || blockTypeIndex >= MAX_BLOCK_DEFINITIONS)
			blockTypeIndex = 0; // FALLBACK!

		return blockDefinitions[blockTypeIndex].collision;
	}

	std::string Level::getBlockCallbackString(const Block& block) const
	{
		// Get the block's type index
		auto blockTypeIndex = block.getTypeIndex();

		// Treat invalid index same way as 0-index
		if (blockTypeIndex < 0 || blockTypeIndex >= MAX_BLOCK_DEFINITIONS)
			blockTypeIndex = 0; // FALLBACK!

		return blockDefinitions[blockTypeIndex].callback;
	}

	SDL_Rect Level::calculateLevelSize() const
	{
		SDL_Rect r{0, 0, 0, 0};
		r.w = columns * blockSize.w;
		r.h = rows * blockSize.h;
		return r;
	}

	// Return tile indices overlapped by a rect (clamped to level bounds).
	void Level::rectToBlockSpan(const SDL_FRect& r, int& col0, int& col1, int& row0, int& row1) const
	{
		const float eps = 0.0001f;
		const int w = blockSize.w, h = blockSize.h;

		if (r.w <= 0.f || r.h <= 0.f)
		{
			// Degenerate rect -> clamp to a single tile under r.x,r.y
			int c = std::max(0, std::min(columns - 1, worldToCol(r.x, w)));
			int rI = std::max(0, std::min(rows - 1, worldToRow(r.y, h)));
			col0 = col1 = c;
			row0 = row1 = rI;
			return;
		}

		int c0 = worldToCol(r.x, w);
		int c1 = worldToCol(r.x + r.w - eps, w);
		int r0 = worldToRow(r.y, h);
		int r1 = worldToRow(r.y + r.h - eps, h);

		// Won't need this anymore
		// Clamp to level bounds
		/*c0 = std::max(0, std::min(columns - 1, c0));
		c1 = std::max(0, std::min(columns - 1, c1));
		r0 = std::max(0, std::min(rows - 1, r0));
		r1 = std::max(0, std::min(rows - 1, r1));*/

		// Normalize if inverted due to rounding
		if (c1 < c0) std::swap(c0, c1);
		if (r1 < r0) std::swap(r0, r1);

		col0 = c0; col1 = c1; row0 = r0; row1 = r1;
	}

	std::vector<Level::BlockQuery> Level::queryBlocksUnderCollider(SDL_FRect collider) const
	{
		std::vector<Level::BlockQuery> bigQuery;

		int col0, col1, row0, row1;
		rectToBlockSpan(collider, col0, col1, row0, row1);

		for (int r = row0; r <= row1; r++)
		{
			for (int c = col0; c <= col1; c++)
			{
				auto q = queryBlock(c, r);
				bigQuery.push_back(q);
			}
		}

		return bigQuery;
	}

	// Query a tile (with OOB policy)
	Level::BlockQuery Level::queryBlock(int col, int row) const
	{
		using Collision = Level::Block::Collision;

		BlockQuery q;
		q.coords = { col,row };

		// OOB is like a Tic-Tac-Toe index here, hehe
		// 012
		// 345
		// 678
		// 4 means in level
		int oob = 4;
		if (col < 0)        oob -= 1; // OOB to the left
		if (col >= columns) oob += 1; // OOB to the right
		if (row < 0)        oob -= 3; // OOB upwards
		if (row >= rows)    oob += 3; // OOB downwards

		const Collision* oobLut[] =
		{
			&throughTopLeft,
			&throughTop,
			&throughTopRight,
			&throughLeft,
			nullptr, // Not out of bounds, but inside of level
			&throughRight,
			&throughBottomLeft,
			&throughBottom,
			&throughBottomRight
		};

		const Collision* collision = oobLut[oob];

		if (collision) // Out of bounds
		{
			q.insideLevel = false;
			// Decide collision for OOB
 			q.coll = *collision;
			q.type = Level::Block::Type::EMPTY;
			return q;
		}
		else // Not out of bounds
		{
			q.insideLevel = true;
			const Block& b = array[row * columns + col];
			q.coll = getBlockCollisionType(b);
			q.type = b.type;
			q.callback = getBlockCallbackString(b);
		}
		return q;
	}

	Level::BlockQuery Level::queryBlock(SDL_FPoint positionInLevel) const
	{
		int column = worldToCol(std::floor(positionInLevel.x), blockSize.w);
		int row = worldToRow(std::floor(positionInLevel.y), blockSize.h);
		return queryBlock(column, row);
	}

	// Is any overlapped tile "water"?
	bool Level::rectInWater(const SDL_FRect& r) const
	{
		int c0, c1, r0, r1;
		rectToBlockSpan(r, c0, c1, r0, r1);
		for (int rI = r0; rI <= r1; ++rI)
			for (int cI = c0; cI <= c1; ++cI)
				if (queryBlock(cI, rI).coll == Level::Block::Collision::Water)
					return true;
		return false;
	}

	static constexpr float EPS = 0.0001f;

	// Axis-separated sweep: move horizontally by dx, collide with solids.
	Level::SweepHit Level::sweepHorizontal(const SDL_FRect& rect, float dx) const
	{
		SweepHit out; out.hit = false;
		const int w = blockSize.w, h = blockSize.h;
		SDL_FRect box = rect;

		int col0, col1, row0, row1;
		rectToBlockSpan(box, col0, col1, row0, row1);

		if (dx > 0.f)
		{
			float right0 = box.x + box.w;
			float right1 = right0 + dx;
			// INCLUDE boundary tile at right0
			int startCol = worldToCol(right0, w);
			int endCol = worldToCol(right1 - EPS, w);

			for (int c = startCol; c <= endCol; ++c)
			{
				for (int rI = row0; rI <= row1; ++rI)
				{
					auto q = queryBlock(c, rI);
					if (q.coll == Block::Collision::Solid)
					{
						float tileLeft = float(c * w);
						out.hit = true;
						out.coords = { c,rI };
						out.newX = tileLeft - box.w;
						out.newY = box.y;
						return out;
					}
				}
			}
			out.newX = rect.x + dx;
			out.newY = rect.y;
			return out;
		}
		else if (dx < 0.f)
		{
			float left0 = box.x;
			float left1 = left0 + dx;
			// For left, step into columns we cross moving left; use a tiny -EPS to include boundary
			int startCol = worldToCol(left0 - EPS, w);
			int endCol = worldToCol(left1 + EPS, w);

			for (int c = startCol; c >= endCol; --c)
			{
				for (int rI = row0; rI <= row1; ++rI)
				{
					auto q = queryBlock(c, rI);
					if (q.coll == Block::Collision::Solid)
					{
						float tileRight = float((c + 1) * w);
						out.hit = true;
						out.coords = { c,rI };
						out.newX = tileRight;
						out.newY = box.y;
						return out;
					}
				}
			}
			out.newX = rect.x + dx;
			out.newY = rect.y;
			return out;
		}
		else
		{
			out.newX = rect.x; out.newY = rect.y; return out;
		}
	}


	// Axis-separated sweep: move vertically by dy, collide with solids.
	Level::SweepHit Level::sweepVertical(const SDL_FRect& rect, float dy) const
	{
		SweepHit out; out.hit = false;
		const int w = blockSize.w, h = blockSize.h;
		SDL_FRect box = rect;
		box.w -= 1; // FIXME: This apparently fixes a faulty jumpstucky problem

		int col0, col1, row0, row1;
		rectToBlockSpan(box, col0, col1, row0, row1);

		if (dy > 0.f)
		{
			float bottom0 = box.y + box.h;
			float bottom1 = bottom0 + dy;
			// INCLUDE boundary tile at bottom0
			int startRow = worldToRow(bottom0, h);
			int endRow = worldToRow(bottom1 - EPS, h);

			for (int rI = startRow; rI <= endRow; ++rI)
			{
				for (int cI = col0; cI <= col1; ++cI)
				{
					auto q = queryBlock(cI, rI);
					if (q.coll == Block::Collision::Solid)
					{
						float tileTop = float(rI * h);
						out.hit = true;
						out.coords = { cI,rI };
						out.newY = tileTop - box.h;
						out.newX = box.x;
						return out;
					}
				}
			}
			out.newX = rect.x;
			out.newY = rect.y + dy;
			return out;
		}
		else if (dy < 0.f)
		{
			float top0 = box.y;
			float top1 = top0 + dy;
			// For up, include boundary with -EPS
			int startRow = worldToRow(top0 - EPS, h);
			int endRow = worldToRow(top1 + EPS, h);

			for (int rI = startRow; rI >= endRow; --rI)
			{
				for (int cI = col0; cI <= col1; ++cI)
				{
					auto q = queryBlock(cI, rI);
					if (q.coll == Block::Collision::Solid)
					{
						float tileBottom = float((rI + 1) * h);
						out.hit = true;
						out.coords = { cI,rI };
						out.newY = tileBottom;
						out.newX = box.x;
						return out;
					}
				}
			}
			out.newX = rect.x;
			out.newY = rect.y + dy;
			return out;
		}
		else
		{
			out.newX = rect.x; out.newY = rect.y; return out;
		}
	}

	const SdlTexture& Level::getTilesetTexture() const
	{
		return tileset;
	}

	const Level::TilesetMeta Level::getTilesetMeta() const
	{
		return tilesMeta;
	}

	void Level::setTileset(SdlTexture SdlTexture)
	{
		this->tileset = std::move(SdlTexture);
		tilesMeta.inferColumnsFromTexture(tileset);
	}

	bool Level::loadTileset(SDL_Renderer* renderer)
	{
		setTileset(SdlTexture(tilesetTexturePath.c_str(), renderer));
		return tileset.isValid();
	}

	bool Level::loadBackgrounds(SDL_Renderer* renderer)
	{
		bool success = true;
		for (auto& background : backgrounds)
		{
			success &= background.loadTexture(renderer);
		}
		return success;
	}

	bool Level::loadTextures(SDL_Renderer* renderer)
	{
		bool success = true;
		success &= loadTileset(renderer);
		success &= loadBackgrounds(renderer);
		return success;
	}

	void Level::draw(DrawContext context) const
	{
		SDL_Renderer* renderer = context.getRenderer();
		if (!renderer || !array) return;

		// Mitigate division by zero
		if (!tilesMeta.isValid())
			return;

		auto viewport = context.getBounds();
		auto scroll = context.getScrollOffset();
		SDL_Rect tile{ 0,0,tilesMeta.tileW,tilesMeta.tileW };
		int leftOffset = scroll.x;
		int topOffset = scroll.y;
		int leftExtent = std::clamp(scroll.x / tile.w, 0, columns);
		int rightExtent = std::clamp(leftExtent + (viewport.w + tile.w - 1) / tile.w + 1, 0, columns);
		int topExtent = std::clamp(scroll.y / tile.h, 0, rows);
		int bottomExtent = std::clamp(topExtent + (viewport.h + tile.h - 1) / tile.h + 1, 0, rows);

		for (const auto& background : backgrounds)
		{
			if (!background.isValid())
				continue; // Skip invalid/unused/uninited backgrounds

			const SdlTexture& texture = background.getTexture();
			SDL_Texture* rawTexture = texture.get();

			// Query background dimensions
			auto dimensions = background.getTexture().getDimensions();
			int bkgWidth = dimensions.w;
			int bkgHeight = dimensions.h;

			// Sanitize dimensions!
			if (bkgWidth <= 0 || bkgHeight <= 0)
				continue; // TODO: Error logging

			int parallaxOffsetX = (float)(background.getParallaxX() * (float)leftOffset * -1);
			// Make sure the first background tile is visible (horizontally)
			while (parallaxOffsetX + bkgWidth < 0)
				parallaxOffsetX += bkgWidth;

			int parallaxOffsetY = (float)(background.getParallaxY() * (float)topOffset * -1);
			// Make sure the first background tile is visible (vertically)
			while (parallaxOffsetY + bkgHeight < 0)
				parallaxOffsetY += bkgHeight;

			for (int bkgTileY = parallaxOffsetY; bkgTileY < viewport.h; bkgTileY += bkgHeight)
			{
				for (int bkgTileX = parallaxOffsetX; bkgTileX < viewport.w; bkgTileX += bkgWidth)
				{
					SDL_Rect dest{ // Where to put this tile
						bkgTileX,
						bkgTileY,
						bkgWidth,
						bkgHeight
					};
					SDL_RenderCopy(renderer, rawTexture, nullptr, &dest);
				}
			}
		}

		if (tileset)
		{
			for (int r = topExtent; r < bottomExtent; ++r)
			{
				for (int c = leftExtent; c < rightExtent; ++c)
				{
					// Fetch the block
					const Block& b = array[indexOf(r, c)];
					if (b.type.isEmpty())
						continue; // Empty block is invisible

					// Get block type index
					auto blockTypeIndex = b.getTypeIndex();
					if (blockTypeIndex < 0 || blockTypeIndex >= MAX_BLOCK_DEFINITIONS)
						continue; // Out of bounds. Skip.

					// Get index of the tile that we should draw
					auto blockTileIndex = blockDefinitions[blockTypeIndex].tileIndex;

					SDL_Rect src = tilesMeta.makeRectForTile(blockTileIndex);

					SDL_Rect dst{
						c * blockSize.w - leftOffset,
						r * blockSize.h - topOffset,
						blockSize.w,
						blockSize.h
					};

					SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
					SDL_RenderCopy(renderer, tileset, &src, &dst);
				}
			}
		}
	}

	// Level loader

	Level::Loader::Loader(PassKey<GameWorld> pk)
	{}
	std::unique_ptr<Level> Level::Loader::loadLevel(const char* path)
	{
		do // gotophobia
		{
			if (!loadIni(path))
				return nullptr; // goto failure

			newLevel = parseAndConstruct();

			if (!newLevel)
				break; // goto failure

			if (!parseAfterConstruct())
				break; // goto failure

			if (!parseBackgrounds())
				break; // goto failure

			if (!parseBlockDefinitions())
				break; // goto failure

			if (!parseGrid())
				break; // goto failure

			if (!parseSpawnList())
				break; // goto failure

			return std::move(newLevel);
		} while (0); // We only needed this block because of gotophobia

		// Failure
		newLevel = nullptr;
		return nullptr;
	}
	const std::vector<Level::Loader::Spawn>& Level::Loader::getSpawnList() const
	{
		return spawnList;
	}
	int Level::Loader::getHeroIndex() const
	{
		return heroIndex;
	}
	std::string Level::Loader::getErrorLog() const
	{
		return errorLog;
	}
	std::unique_ptr<Level> Level::Loader::parseAndConstruct()
	{
		int columns;
		int rows;
		SDL_Rect blockSize = { 0,0,0,0 };

		try { columns = std::stoi(getValue("Grid", "Columns")); }
		catch (...) { logError("Couldn't read [Grid]Columns"); return nullptr; }

		try { rows = std::stoi(getValue("Grid", "Rows")); }
		catch (...) { logError("Couldn't read [Grid]Rows"); return nullptr; }

		try { blockSize.w = std::stoi(getValue("Tileset", "TileWidth")); }
		catch (...) { logError("Couldn't read [Tileset]TileWidth"); return nullptr; }

		try { blockSize.h = std::stoi(getValue("Tileset", "TileHeight")); }
		catch (...) { logError("Couldn't read [Tileset]TileHeight"); return nullptr; }

		std::string tilesetTexturePath = getValue("Tileset", "Texture");
		if (tilesetTexturePath.empty())
		{
			logError("Tileset texture path not specified");
			return nullptr;
		}

		auto newLevel = std::make_unique<Level>(columns, rows, blockSize);

		newLevel->tilesetTexturePath=tilesetTexturePath;

		return std::move(newLevel);
	}
	bool Level::Loader::parseAfterConstruct()
	{
		newLevel->throughTopLeft = parseOOB("TopLeft");
		newLevel->throughTop = parseOOB("Top");
		newLevel->throughTopRight = parseOOB("TopRight");
		newLevel->throughLeft = parseOOB("Left");
		newLevel->throughRight = parseOOB("Right");
		newLevel->throughBottomLeft = parseOOB("BottomLeft");
		newLevel->throughBottom = parseOOB("Bottom");
		newLevel->throughBottomRight = parseOOB("BottomRight");

		try { newLevel->nextSection = std::stoi(getValue("SSGELEV1", "NextSection", "-1"));
		} catch (...) {
			// TODO: Better error handling
			logError("Failed to specify next section! Player may be stuck here!");
			return false;
		}

		return true;
	}
	bool Level::Loader::parseBackgrounds()
	{
		int howManyBackgrounds = getInt("Background", "Layers", 0);

		for (int bkgIndex = 0; bkgIndex < howManyBackgrounds; bkgIndex++)
		{
			std::string layerCaption = "Background.Layer" + std::to_string(bkgIndex);
			std::string bkgTexturePath = getValue(layerCaption, "Texture");
			int parallaxX = getFloat(layerCaption, "ParallaxX", 1);
			int parallaxY = getFloat(layerCaption, "ParallaxY", 1);

			newLevel->backgrounds.emplace_back(bkgTexturePath, parallaxX, parallaxY);
		}

		return true;
	}
	bool Level::Loader::parseBlockDefinitions()
	{
		for (int i = 0; i < MAX_BLOCK_DEFINITIONS; i++)
		{
			std::string blockDefinition = getValue("Blocks", i);
			if (blockDefinition.empty())
				continue;

			newLevel->blockDefinitions[i].tileIndex = parseTileIndex(blockDefinition);
			newLevel->blockDefinitions[i].collision = parseBlockCollision(blockDefinition);
			newLevel->blockDefinitions[i].callback = parseCallbackName(blockDefinition);
		}
		return true;
	}
	bool Level::Loader::parseGrid()
	{
		int rows = newLevel->rows;
		int columns = newLevel->columns;

		for (int row = 0; row < rows; row++)
		{
			std::string rowString = getValue("Grid", row);
			int len = std::min(columns, (int)rowString.length());
			for (int column = 0; column < len; column++)
			{
				unsigned char blockChar = rowString[column];
				if (blockChar < ' ' || blockChar == '`' || blockChar>'~')
					continue;

				int blockIndex = blockChar - ' ' - (blockChar > '`' ? 1 : 0);

				newLevel->array[column + row * columns].type = blockIndex;
			}
		}

		return true;
	}
	bool Level::Loader::parseSpawnList()
	{
		try
		{
			int spawnEntries = 0;
			spawnEntries = std::stoi(getValue("SpawnList", "Entries"));
			heroIndex = std::stoi(getValue("SpawnList", "Hero"));
			if (heroIndex < 0)
			{
				logError("Invalid hero index!");
				return false;
			}
			if (heroIndex >= spawnEntries)
			{
				logError("Hero index out of bounds!");
				return false;
			}
			for (int i = 0; i < spawnEntries; i++)
			{
				Spawn entry;
				entry.where.x = (float)std::stoi(getValue("SpawnList", "Spawn" + std::to_string(i) + "X"));
				entry.where.y = (float)std::stoi(getValue("SpawnList", "Spawn" + std::to_string(i) + "Y"));
				entry.what = getValue("SpawnList", "Spawn" + std::to_string(i) + "Entity");
				entry.callback = getValue("SpawnList", "Spawn" + std::to_string(i) + "Callback");
				spawnList.push_back(entry);
			}

			return true;
		}
		catch (...)
		{ // We should have spawned something!!!
			// TODO: Better error handling. We don't know which field is busted!
			logError("Spawn list could not be read!");
			return false;
		}
	}
	Level::Block::Collision Level::Loader::parseOOB(std::string side)
	{
		try
		{
			return parseCollision(getValue("SSGELEV1", "OOBCol" + side));
		}
		catch (...)
		{
			logError("Caught OOB collision parsing error!");
			return Block::Collision::Air;
		}
	}
	Level::Block::Collision Level::Loader::parseCollision(std::string collisionString)
	{
		static const char* lut[] =
		{
			"Air",
			"Solid",
			"Water",
			"Hazard",
			"DeathOnTouch",
			"DeathIfFullyOutside",
			"WrapAround",
			"NextSection",
			nullptr
		};

		const char** current = lut;

		for (int i = 0; *current; i++, current++)
		{
			if (collisionString == *current)
				return Block::Collision(i);
		}

		// Fallback
		logError("Unknown collision specification!");
		return Block::Collision::Air;
	}
	Level::Block::Collision Level::Loader::parseBlockCollision(std::string blockItemValue)
	{
		try
		{
			auto colonIndex = blockItemValue.find(':');
			if (colonIndex <= 0)
			{
				return parseCollision(blockItemValue);
			}
			return parseCollision(blockItemValue.substr(0, colonIndex));
		}
		catch (...)
		{
			logError("Caught block collision parsing exception!");
			return Block::Collision::Air;
		}
	}
	int Level::Loader::parseTileIndex(std::string blockItemValue)
	{
		try
		{
			auto colonIndex = blockItemValue.find(':');
			auto endOfNumber = blockItemValue.find('@');
			std::string numberSubstring;
			if (endOfNumber == -1)
			{
				numberSubstring = blockItemValue.substr(colonIndex + 1);
			}
			else
			{
				numberSubstring = blockItemValue.substr(colonIndex + 1, endOfNumber - colonIndex - 1);
			}
			return std::stoi(numberSubstring);
		}
		catch (...)
		{
			// Fallback
			logError("Caught tile index parsing exception! Block will be invisible!");
			return -1;
		}
	}
	std::string Level::Loader::parseCallbackName(std::string blockItemValue)
	{
		try
		{
			auto monkeyIndex = blockItemValue.find('@');
			if (monkeyIndex == -1)
			{
				// No callback here
				return "";
			}
			else
			{
				return blockItemValue.substr(monkeyIndex + 1);
			}
		}
		catch (...)
		{
			// Fallback
			logError("Caught block callback parsing exception!");
			return "";
		}
	}
	Level::Background::Background(std::string path, float parallaxX, float parallaxY)
		: path(path), parallaxX(parallaxX), parallaxY(parallaxY) {}
	
	const SdlTexture& Level::Background::getTexture() const
	{
		return texture;
	}
	void Level::Background::setTexture(SdlTexture texture)
	{
		this->texture = std::move(texture);
	}
	bool Level::Background::loadTexture(SDL_Renderer* renderer)
	{
		setTexture(SdlTexture(path.c_str(), renderer));
		return texture.isValid();
	}
	
	std::string Level::Background::getPath() const { return path; }
	void Level::Background::setPath(std::string path)
		{ this->path = path; }

	float Level::Background::getParallaxX() const { return parallaxX;}
	void Level::Background::setParallaxX(float parallaxX)
		{ this->parallaxX = parallaxX; }

	float Level::Background::getParallaxY() const { return parallaxY; }
	void Level::Background::setParallaxY(float parallaxY)
	{
		this->parallaxY = parallaxY;
	}

	bool Level::Background::isValid() const { return texture.isValid(); }
}
