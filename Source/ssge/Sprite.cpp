#include "Sprite.h"
#include <cmath>

using namespace ssge;

Sprite::Animation::Sequence::Sequence()
{
	speed = 0;
	altSpeed = 0;
	loopTo = 0;
}

Sprite::Animation::Sequence::Sequence(int speed, int altSpeed, int loopTo)
{
	this->speed = speed;
	this->altSpeed = altSpeed;
	this->loopTo = loopTo;
}

Sprite::Animation::Animation()
{
	paused = true; // Initially paused
	seqIdx = 0;
	frameIdx = 0;
	timer = 0;
	overrideSpeed = OVERRIDE_SPEED_DISABLE;
	highSpeed = 100; // Default value; adjust as needed
	lowSpeed = 100;  // Default value; adjust as needed
	lerp = LERP_DISABLE; // Disabled lerp by default
	finished = false;
}

//===================
// Sprite::Animation
//===================

// Calculates the effective speed.
// If overrideSpeed is not disabled (i.e. not equal to OVERRIDE_SPEED_DISABLE),
// it is returned. Otherwise, if lerp is enabled (not LERP_DISABLE), a linear
// interpolation between lowSpeed and highSpeed is computed.
int Sprite::Animation::calculateEffectiveSpeed() const
{
	if (overrideSpeed != OVERRIDE_SPEED_DISABLE)
	{
		return overrideSpeed;
	}

	if (lerp != LERP_DISABLE)
	{
		return lowSpeed + static_cast<int>(
			(highSpeed - lowSpeed) * (lerp / 100.0f));
	}

	return highSpeed;
}

//===================
// Sprite Constructor
//===================

Sprite::Sprite(const Sprite::Definition& def)
	: definition(def)
{
	// Initialize rendering properties
	alpha = 255;
	angle = 0;
	blend = 0;
	xscale = 1.0f;
	yscale = 1.0f;
}

//======================
// Accessor Functions
//======================

bool Sprite::isPlaying() const
{
	// Returns true if the animation is not paused.
	return !animation.paused;
}

int Sprite::getSeqIdx() const { return animation.seqIdx; }

int Sprite::getCurrentAnimationFrame() const
{ return animation.frameIdx; }

int Sprite::getAnimationSpeed() const
{ return animation.calculateEffectiveSpeed(); }

bool ssge::Sprite::isFinished() const
{
	return animation.finished;
}

//==========================
// Animation Control Methods
//==========================

Sprite& Sprite::play()
{
	animation.paused = false;
	return *this; // Setters return object by reference (U++ standard)
}

Sprite& Sprite::stop()
{
	animation.paused = true;
	return *this; // Setters return object by reference (U++ standard)
}

Sprite& Sprite::rewind()
{
	animation.frameIdx = 0;
	animation.timer = 0;
	animation.finished = false;
	return *this; // Setters return object by reference (U++ standard)
}

Sprite& Sprite::nextFrame()
{
	auto& sequence = definition.sequences[animation.seqIdx];

	int nOFrames = static_cast<int>(sequence.imageIndexes.size());
	int& frame = animation.frameIdx;

	// Advance frame with loop wrap
	if (++frame >= nOFrames)
	{
		frame = sequence.loopTo;

		// Handle faulty loop
		if (frame >= nOFrames)
		{
			// Fallback to first frame
			frame = 0;
			//TODO: Log error
		}
	}

	animation.timer = 0; // Reset timer on manual frame change

	return *this; // Setters return object by reference (U++ standard)
}

Sprite& Sprite::prevFrame()
{
	if (--animation.frameIdx < 0)
	{
		animation.seqIdx = static_cast<int>(
			definition.sequences[animation.seqIdx].imageIndexes.size() - 1);
	}
	animation.timer = 0;

	return *this; // Setters return object by reference (U++ standard)
}

Sprite& Sprite::setSequence(int seqIdx)
{
	int nOSequences = static_cast<int>(definition.sequences.size());

	// Fallback to first sequence
	if (seqIdx < 0 || seqIdx >= nOSequences)
	{
		seqIdx = 0;
		//TODO: Log error
	}

	// Fetch sequence definition
	auto& sequence = definition.sequences[seqIdx];

	// Initialize animation parameters to play this new sequence
	animation.finished = false; // We're just getting started
	animation.paused = false;   // Play on start
	animation.seqIdx = seqIdx;  // Current sequence index
	animation.frameIdx = 0;     // Current frame
								// (used as sequence.imageIndexes[frame])
	animation.timer = 0;
	animation.overrideSpeed =               // Overrides speed
		Animation::OVERRIDE_SPEED_DISABLE;  // (disabled by default)
	animation.highSpeed = sequence.speed;   // Speed when lerp is 100
	animation.lowSpeed = sequence.altSpeed; // Speed when lerp is 0
	animation.lerp =             // For variable speed (0-100).
		Animation::LERP_DISABLE; // If disabled, highSpeed will be used.

	return *this; // Setters return object by reference (U++ standard)
}

Sprite& Sprite::setFrame(int frame)
{
	auto& sequence = definition.sequences[animation.seqIdx];
	int nOFrames = (int)sequence.imageIndexes.size();

	// Handle out-of-bounds parameter
	if (frame < 0 || frame >= nOFrames)
	{
		// Fallback to first frame
		animation.frameIdx = 0;
		//TODO: Log error
	}
	else
	{
		animation.frameIdx = frame;
	}

	// Restart timer
	animation.timer = 0;

	return *this; // Setters return object by reference (U++ standard)
}

Sprite& Sprite::setSpeed(int speed)
{
	animation.overrideSpeed = speed;
	return *this; // Setters return object by reference (U++ standard)
}

Sprite& Sprite::setLerp(int lerpVal)
{
	animation.lerp = lerpVal;
	return *this; // Setters return object by reference (U++ standard)
}

Sprite& Sprite::resetSpeed()
{
	animation.overrideSpeed = Sprite::Animation::OVERRIDE_SPEED_DISABLE;
	return *this; // Setters return object by reference (U++ standard)
}

SDL_Rect Sprite::getBounds() const
{
	int imgIdx = calculateImageIndex();
	if (imgIdx == -1)
		return SDL_Rect{ 0,0 };

	const auto& img = definition.images[imgIdx];

	SDL_Point offset = img.anchor;
	offset.x *= -1;
	offset.y *= -1;

	SDL_Rect offseted = img.region;
	offseted.x += offset.x;
	offseted.y += offset.y;

	return offseted;
}

//=============================
// Image Index and Fetching
//=============================

int Sprite::calculateImageIndex() const
{
	auto& sequences = definition.sequences;

	// Ensure that this definition has any sequence
	if (auto seqCount = static_cast<int>(sequences.size()))
	{
		auto seqIdx = animation.seqIdx;

		// Ensure that the sequence index is valid.
		if (seqIdx < 0 || seqIdx >= seqCount)
			return -1; // Invalid sequence - Invalid index

		const auto& seq = definition.sequences[animation.seqIdx];

		// Ensure that there is at least one image in the sequence
		if (auto imgCount = static_cast<int>(seq.imageIndexes.size()))
		{
			// Ensure that the frame index is valid.
			if (animation.frameIdx < 0 || animation.frameIdx >= imgCount)
				return 0; // Fallback to first image in the sequence

			return seq.imageIndexes[animation.frameIdx];
		}
		else return -1; // No images in sequence - Invalid index
	}
	else return -1; // No sequences in definition - Invalid index
}

const Sprite::Image& Sprite::fetchCurrentImage() const
{
	int imgIndex = calculateImageIndex();

	if (imgIndex == -1)
		throw "TODO: Handle error where image index is invalid!";

	// Return the current image from the definition.
	return definition.images[imgIndex];
}

//======================
// Update and Rendering
//======================

void Sprite::update(double deltaTime)
{
	if (!animation.paused)
	{
		animation.finished = false;

		int effectiveSpeed = animation.calculateEffectiveSpeed();
		animation.timer += static_cast<int>(deltaTime * effectiveSpeed * 100);

		auto& currentSequence = definition.sequences[animation.seqIdx];
		int nOFrames = (int)currentSequence.imageIndexes.size();

		// When timer exceeds 100, advance the frame.
		while (animation.timer > 100)
		{
			// Deplete time(r)
			animation.timer -= 100;

			// Advance with reroll
			if (++animation.frameIdx >= nOFrames)
			{
				animation.frameIdx = currentSequence.loopTo;

				// Don't allow broken reroll to go out of bounds
				if (animation.frameIdx >= nOFrames)
					animation.frameIdx = 0;
				//TODO: Log this!

				animation.finished = true;
			}
		}
	}
}

void Sprite::draw(DrawContext context) const
{
	render(context.getRenderer(), context.calculateAnchorPoint());
}

void Sprite::render(SDL_Renderer* renderer, SDL_Point offsetFromViewport) const
{
	int imgIndex = calculateImageIndex();
	if (imgIndex == -1)
		return;

	const Sprite::Image& img = definition.images[imgIndex];

	// Construct the source rectangle.
	SDL_Rect src = img.region;

	// Compute the absolute scaled dimensions.
	int absW = static_cast<int>(src.w * std::fabs(xscale));
	int absH = static_cast<int>(src.h * std::fabs(yscale));

	// Determine flip flags based on negative scales.
	SDL_RendererFlip flip = SDL_FLIP_NONE;
	if (xscale < 0) {
		flip = (SDL_RendererFlip)(flip | SDL_FLIP_HORIZONTAL);
	}
	if (yscale < 0) {
		flip = (SDL_RendererFlip)(flip | SDL_FLIP_VERTICAL);
	}

	// Compute the scaled anchor coordinates.
	int scaledAnchorX = static_cast<int>(img.anchor.x * fabs(xscale));
	int scaledAnchorY = static_cast<int>(img.anchor.y * fabs(yscale));

	// Adjust destination rectangle
	// so that the anchor lands at offsetFromViewport.
	SDL_Rect dst{
		(xscale < 0) ? // dst.x
			offsetFromViewport.x - (absW - scaledAnchorX)
		  : offsetFromViewport.x - scaledAnchorX,
		(yscale < 0) ? // dst.y
			offsetFromViewport.y - (absH - scaledAnchorY)
		 : offsetFromViewport.y - scaledAnchorY,
		absW, // dst.w
		absH  // dst.h
	};

	// Compute the rotation center (pivot)
	// relative to the destination rectangle.
	SDL_Point center{
		(xscale < 0) ? (absW - scaledAnchorX) : scaledAnchorX, // center.x
		(yscale < 0) ? (absH - scaledAnchorY) : scaledAnchorY // center.y
	};

	// Update TextureAlphaMod, but only if we changed it
	Uint8 alphaInSDL;
	auto& spritesheet = definition.spritesheet;
	SDL_GetTextureAlphaMod(spritesheet, &alphaInSDL);
	if(alpha!=alphaInSDL)
		SDL_SetTextureAlphaMod(spritesheet, alpha);

	SDL_RenderCopyEx(
		renderer, spritesheet, &src, &dst, static_cast<double>(angle),
		&center, flip);

	// Redundant
	//SDL_SetTextureAlphaMod(definition.spritesheet, 255);
}

Sprite::Image::Image(int x, int y, int w, int h, int cx, int cy)
{
	region = { x,y,w,h };
	anchor = { cx,cy };
}

Sprite::Definition::Definition(std::string spritesheetPath)
	: spritesheetPath(spritesheetPath) {}

bool Sprite::Definition::load(SDL_Renderer* renderer)
{
	spritesheet = SdlTexture(spritesheetPath, renderer);
	return isLoaded();
}

void Sprite::Definition::unload()
{
	spritesheet.free();
}

bool Sprite::Definition::isLoaded() const
{
	return spritesheet.isValid();
}

Sprite::Animation::Sequence& Sprite::Definition::addSequence()
{
	Sprite::Animation::Sequence seq;
	sequences.push_back(std::move(seq));
	return sequences.back();
}

Sprite::Animation::Sequence& Sprite::Definition::addSequence(
	int speed, int altSpeed, int loopTo)
{
	Sprite::Animation::Sequence seq(speed, altSpeed, loopTo);
	sequences.push_back(std::move(seq));
	return sequences.back();
}
