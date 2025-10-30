#include "Level.h"
#include <cstring>
#include <algorithm>
#include <string>
#include "SdlTexture.h"

namespace ssge
{
	// read helpers (little-endian, bounds-checked)
	inline bool read_u32(const unsigned char*& p, const unsigned char* end, uint32_t& out) {
		if (end - p < 4) return false;
		out = (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
		p += 4; return true;
	}
	inline bool read_s32(const unsigned char*& p, const unsigned char* end, int32_t& out) {
		uint32_t u; if (!read_u32(p, end, u)) return false; out = (int32_t)u; return true;
	}
	inline bool read_u16(const unsigned char*& p, const unsigned char* end, uint16_t& out) {
		if (end - p < 2) return false;
		out = (uint16_t)p[0] | ((uint16_t)p[1] << 8);
		p += 2; return true;
	}
	inline bool read_u8(const unsigned char*& p, const unsigned char* end, uint8_t& out) {
		if (end - p < 1) return false; out = *p++; return true;
	}
	inline bool read_bytes(const unsigned char*& p, const unsigned char* end, char* dst, std::size_t n) {
		if ((std::size_t)(end - p) < n) return false;
		std::memcpy(dst, p, n); p += n; return true;
	}

	inline bool check_magic(const unsigned char*& p, const unsigned char* end) {
		static const char kMagic[8] = { 'S','S','G','E','L','E','V','1' };
		if (end - p < 8) return false;
		if (std::memcmp(p, kMagic, 8) != 0) return false;
		p += 8; return true;
	}

	inline Level::Block::Collision toCollision(uint8_t v) {
		using C = Level::Block::Collision;
		return (v < (uint8_t)C::TOTAL) ? (C)v : C::Air;
	}
	inline Level::Block::Type toType(uint8_t v)
	{
		return Level::Block::Type(v);
	}


	std::unique_ptr<Level> Level::loadFromBytes(
		const unsigned char* data,
		std::size_t size,
		std::string* outTilesetPath,
		std::string* outError)
	{
		if (outTilesetPath) outTilesetPath->clear();
		if (!data || size < 8) {
			if (outError) *outError = "Level bytes: null/too small";
			return nullptr;
		}

		const unsigned char* p = data;
		const unsigned char* end = data + size;

		if (!check_magic(p, end)) {
			if (outError) *outError = "Level bytes: bad magic (expect SSGELEV1)";
			return nullptr;
		}

		uint32_t cols = 0, rows = 0; int32_t bw = 0, bh = 0;
		if (!read_u32(p, end, cols) || !read_u32(p, end, rows) ||
			!read_s32(p, end, bw) || !read_s32(p, end, bh)) {
			if (outError) *outError = "Level bytes: header truncated";
			return nullptr;
		}
		if (cols == 0 || rows == 0 || bw <= 0 || bh <= 0) {
			if (outError) *outError = "Level bytes: invalid dimensions/block size";
			return nullptr;
		}

		uint8_t oc[8] = { 0 };
		for (int i = 0; i < 8; ++i) {
			if (!read_u8(p, end, oc[i])) {
				if (outError) *outError = "Level bytes: outside-collision truncated";
				return nullptr;
			}
		}

		uint16_t pathLen = 0;
		if (!read_u16(p, end, pathLen)) {
			if (outError) *outError = "Level bytes: missing tileset path length";
			return nullptr;
		}

		std::string tilesetPath;
		if (pathLen > 0) {
			tilesetPath.resize(pathLen);
			if (!read_bytes(p, end, &tilesetPath[0], pathLen)) {
				if (outError) *outError = "Level bytes: truncated tileset path";
				return nullptr;
			}
			if (outTilesetPath) *outTilesetPath = tilesetPath;
		}

		const std::size_t cellCount = (std::size_t)cols * (std::size_t)rows;
		const std::size_t needed = cellCount * 2u;
		if ((std::size_t)(end - p) < needed) {
			if (outError) *outError = "Level bytes: truncated block data";
			return nullptr;
		}

		// Construct empty level (tileset optional; you can load it later using tilesetPath).
		SDL_Rect blockRect{ 0,0,bw,bh };
		std::unique_ptr<Level> lvl = std::make_unique<Level>((int)cols, (int)rows, blockRect, SdlTexture{ nullptr });

		// Outside collision policy
		lvl->throughTopLeft = toCollision(oc[0]);
		lvl->throughTop = toCollision(oc[1]);
		lvl->throughTopRight = toCollision(oc[2]);
		lvl->throughLeft = toCollision(oc[3]);
		lvl->throughRight = toCollision(oc[4]);
		lvl->throughBottomLeft = toCollision(oc[5]);
		lvl->throughBottom = toCollision(oc[6]);
		lvl->throughBottomRight = toCollision(oc[7]);

		// Block payload
		for (uint32_t r = 0; r < rows; ++r) {
			for (uint32_t c = 0; c < cols; ++c) {
				uint8_t t = 0, co = 0;
				read_u8(p, end, t);
				read_u8(p, end, co); // REMOVE ME WHEN YOU REMOVE COLLISION DATA!
				Block* b = lvl->getBlockAt((int)r, (int)c);
				if (b) {
					b->type = toType(t);
				}
			}
		}

		// If you later want Level to load tileset here, call your asset loader with tilesetPath.
		// Otherwise, return Level and let GameWorld/Engine handle tileset based on outTilesetPath.

		return lvl;
	}

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
	{
		const std::size_t count = static_cast<std::size_t>(columns) * static_cast<std::size_t>(rows);
		array = (count > 0) ? new Block[count] : nullptr;
		tilesMeta.tileW = blockSize.w;
		tilesMeta.tileH = blockSize.h;

		for (std::size_t i = 0; i < MAX_BLOCK_DEFINITIONS; i++)
		{
			blockDefinitions[i] = Block::Definition();
		}

		//HARDCODED PLACEHOLDER

		blockDefinitions[0].tileIndex = -1;
		blockDefinitions[1].tileIndex = 0;
		blockDefinitions[2].tileIndex = 1;
		blockDefinitions[3].tileIndex = 2;
		blockDefinitions[4].tileIndex = 3;
		blockDefinitions[5].tileIndex = 4;
		blockDefinitions[6].tileIndex = 5;
		blockDefinitions[7].tileIndex = 6;

		blockDefinitions[0].collision = Block::Collision::Air;
		blockDefinitions[1].collision = Block::Collision::Solid;
		blockDefinitions[2].collision = Block::Collision::Solid;
		blockDefinitions[3].collision = Block::Collision::Solid;
		blockDefinitions[4].collision = Block::Collision::Solid;
		blockDefinitions[5].collision = Block::Collision::Solid;
		blockDefinitions[6].collision = Block::Collision::Solid;
		blockDefinitions[7].collision = Block::Collision::Solid;

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

	bool Level::inBoundsPt(SDL_Point p) const
	{
		if (p.x < 0 || p.y < 0) return false;
		const SDL_Rect size = calculateLevelSize();
		return p.x < size.w && p.y < size.h;
	}

	Level::Block* Level::getBlockAt(int row, int column)
	{
		if (!array || !inBoundsRC(row, column)) return nullptr;
		return &array[indexOf(row, column)];
	}

	const Level::Block* Level::getConstBlockAt(int row, int column) const
	{
		if (!array || !inBoundsRC(row, column)) return nullptr;
		return &array[indexOf(row, column)];
	}

	Level::Block* Level::getBlockAt(SDL_Point point)
	{
		if (!array || !inBoundsPt(point)) return nullptr;
		const int c = point.x / blockSize.w;
		const int r = point.y / blockSize.h;
		return getBlockAt(r, c);
	}

	const Level::Block* Level::getConstBlockAt(SDL_Point point) const
	{
		if (!array || !inBoundsPt(point)) return nullptr;
		const int c = point.x / blockSize.w;
		const int r = point.y / blockSize.h;
		return getConstBlockAt(r, c);
	}

	Level::Block::Collision Level::getCollisionAt(SDL_Point point) const
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

	SDL_Rect Level::calculateLevelSize() const
	{
		SDL_Rect r{0, 0, 0, 0};
		r.w = columns * blockSize.w;
		r.h = rows * blockSize.h;
		return r;
	}

	// Return tile indices overlapped by a rect (clamped to level bounds).
	void Level::rectToTileSpan(const SDL_FRect& r, int& col0, int& col1, int& row0, int& row1) const
	{
		const float eps = 0.0001f;
		const int w = blockSize.w, h = blockSize.h;

		if (r.w <= 0.f || r.h <= 0.f) {
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

		// Clamp to level bounds
		c0 = std::max(0, std::min(columns - 1, c0));
		c1 = std::max(0, std::min(columns - 1, c1));
		r0 = std::max(0, std::min(rows - 1, r0));
		r1 = std::max(0, std::min(rows - 1, r1));

		// Normalize if inverted due to rounding
		if (c1 < c0) std::swap(c0, c1);
		if (r1 < r0) std::swap(r0, r1);

		col0 = c0; col1 = c1; row0 = r0; row1 = r1;
	}

	// Query a tile (with OOB policy)
	Level::BlockQuery Level::queryTile(int col, int row) const
	{
		using Collision = Level::Block::Collision;

		BlockQuery q;
		q.tile = { col,row };

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
		}
		return q;
	}

	// Is any overlapped tile "water"?
	bool Level::rectInWater(const SDL_FRect& r) const
	{
		int c0, c1, r0, r1;
		rectToTileSpan(r, c0, c1, r0, r1);
		for (int rI = r0; rI <= r1; ++rI)
			for (int cI = c0; cI <= c1; ++cI)
				if (queryTile(cI, rI).coll == Level::Block::Collision::Water)
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
		rectToTileSpan(box, col0, col1, row0, row1);

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
					auto q = queryTile(c, rI);
					if (q.coll == Block::Collision::Solid)
					{
						float tileLeft = float(c * w);
						out.hit = true;
						out.tile = { c,rI };
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
					auto q = queryTile(c, rI);
					if (q.coll == Block::Collision::Solid)
					{
						float tileRight = float((c + 1) * w);
						out.hit = true;
						out.tile = { c,rI };
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

		int col0, col1, row0, row1;
		rectToTileSpan(box, col0, col1, row0, row1);

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
					auto q = queryTile(cI, rI);
					if (q.coll == Block::Collision::Solid)
					{
						float tileTop = float(rI * h);
						out.hit = true;
						out.tile = { cI,rI };
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
					auto q = queryTile(cI, rI);
					if (q.coll == Block::Collision::Solid)
					{
						float tileBottom = float((rI + 1) * h);
						out.hit = true;
						out.tile = { cI,rI };
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

	void Level::setTileset(SdlTexture& SdlTexture)
	{
		this->tileset = std::move(SdlTexture);
		tilesMeta.inferColumnsFromTexture(tileset);
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
}
