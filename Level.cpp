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

	inline ssge::Level::Block::Collision toCollision(uint8_t v) {
		using C = ssge::Level::Block::Collision;
		return (v < (uint8_t)C::TOTAL) ? (C)v : C::Air;
	}
	inline ssge::Level::Block::Type toType(uint8_t v) {
		using T = ssge::Level::Block::Type;
		return (v < (uint8_t)T::TOTAL) ? (T)v : T::EMPTY;
	}


	std::unique_ptr<ssge::Level> ssge::Level::loadFromBytes(
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
				read_u8(p, end, co);
				Block* b = lvl->getBlockAt((int)r, (int)c);
				if (b) {
					b->type = toType(t);
					b->collision = toCollision(co);
				}
			}
		}

		// If you later want Level to load tileset here, call your asset loader with tilesetPath.
		// Otherwise, return Level and let GameWorld/Engine handle tileset based on outTilesetPath.

		return lvl;
	}


	// ---------------- Block ----------------
	// Transforms Level::Block::Type of this block into a 0-based index.
	// Use this for tileset layout indexing.
	// -1 means empty block or out-of-scope block
	int Level::Block::getTypeIndex() const
	{
		// Map enum Type to a 0-based tileset index. Adjust to your tileset layout.
		switch (type)
		{
			case Type::EMPTY:         return -1; // nothing to draw
			case Type::Grass:         return 0;
			case Type::Ground:        return 1;
			case Type::RedBricks:     return 2;
			case Type::StoneFloor:    return 3;
			case Type::PuzzlePavement:return 4;
			default:                   return -1;
		}
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
	}

	Level& Level::operator=(Level&& other) noexcept
	{
		if (this != &other)
		{
			delete[] array;
			// const members (columns/rows/blockSize) cannot be assigned — but
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
		return b ? b->collision : Block::Collision::Air;
	}

	SDL_Rect Level::calculateLevelSize() const
	{
		SDL_Rect r{0, 0, 0, 0};
		r.w = columns * blockSize.w;
		r.h = rows * blockSize.h;
		return r;
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

		if (tileset)
		{
			for (int r = 0; r < rows; ++r)
			{
				for (int c = 0; c < columns; ++c)
				{
					const Block& b = array[indexOf(r, c)];
					if (b.type == Block::Type::EMPTY) continue;

					SDL_Rect src = tilesMeta.makeRectForTile(b.getTypeIndex());

					SDL_Rect dst{
						c * blockSize.w,
						r * blockSize.h,
						blockSize.w,
						blockSize.h
					};

					SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
					SDL_RenderCopy(renderer, tileset, &src, &dst);
				}
			}
		}
		else
		{
			for (int r = 0; r < rows; ++r)
			{
				for (int c = 0; c < columns; ++c)
				{
					const Block& b = array[indexOf(r, c)];
					if (b.type == Block::Type::EMPTY) continue;

					SDL_Rect dst{
						c * blockSize.w,
						r * blockSize.h,
						blockSize.w,
						blockSize.h
					};

					// Simple color coding for now; replace with tileset blit when integrated
					Uint8 R = 200, G = 200, B = 200;
					switch (b.type)
					{
					case Block::Type::Grass: R = 80; G = 180; B = 80; break;
					case Block::Type::Ground: R = 150; G = 120; B = 70; break;
					case Block::Type::RedBricks: R = 180; G = 60; B = 60; break;
					case Block::Type::StoneFloor: R = 120; G = 120; B = 130; break;
					case Block::Type::PuzzlePavement: R = 100; G = 80; B = 150; break;
					default: break;
					}

					SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
					SDL_SetRenderDrawColor(renderer, R, G, B, 255);
					SDL_RenderFillRect(renderer, &dst);
				}
			}
		}
	}
}
