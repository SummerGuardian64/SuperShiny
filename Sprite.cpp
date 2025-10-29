#include "Sprite.h"

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
	overrideSpeed = Sprite::Animation::OVERRIDE_SPEED_DISABLE;
	highSpeed = 10; // Default value; adjust as needed
	lowSpeed = 5;   // Default value; adjust as needed
	lerp = -1;      // Disabled lerp by default
	finished = false;
}

//===================
// Sprite::Animation
//===================

// Calculates the effective speed.
// If overrideSpeed is not disabled (i.e. not equal to OVERRIDE_SPEED_DISABLE),
// that value is returned. Otherwise, if lerp is enabled (not -1), a linear
// interpolation between lowSpeed and highSpeed is computed.
int Sprite::Animation::calculateEffectiveSpeed() const
{
	if (overrideSpeed != OVERRIDE_SPEED_DISABLE) {
		return overrideSpeed;
	}

	if (lerp != -1) {
		return lowSpeed + static_cast<int>((highSpeed - lowSpeed) * (lerp / 100.0f));
	}

	return highSpeed;
}

//===================
// Sprite Constructor
//===================

Sprite::Sprite(Sprite::Definition& def)
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

int Sprite::getCurrentAnimationFrame() const { return animation.frameIdx; }

int Sprite::getAnimationSpeed() const { return animation.calculateEffectiveSpeed(); }

//==========================
// Animation Control Methods
//==========================

Sprite& Sprite::play()
{
	animation.paused = false;
	return *this;
}

Sprite& Sprite::stop()
{
	animation.paused = true;
	return *this;
}

Sprite& Sprite::rewind()
{
	animation.frameIdx = 0;
	animation.timer = 0;
	animation.finished = false;
	return *this;
}

Sprite& Sprite::nextFrame()
{
	auto& sequence = definition.sequences[animation.seqIdx];

	if (++animation.frameIdx >= sequence.imageIndexes.size()) {
		animation.seqIdx = static_cast<int>(sequence.loopTo);
	}

	animation.timer = 0; // Reset timer on manual frame change
	return *this;
}

Sprite& Sprite::prevFrame()
{
	if (--animation.frameIdx < 0)
	{
		animation.seqIdx = static_cast<int>(
			definition.sequences[animation.seqIdx].imageIndexes.size() - 1);
	}
	animation.timer = 0;
	return *this;
}

Sprite& Sprite::setSequence(int seqIdx)
{
	if (seqIdx < 0 || seqIdx >= static_cast<int>(definition.sequences.size())) {
		seqIdx = 0;
	}

	auto& sequence = definition.sequences[seqIdx];

	animation.finished = false;
	animation.paused = false;
	animation.seqIdx = seqIdx; // Current sequence index
	animation.frameIdx = 0;    // Current frame (used as sequence.imageIndexes[frame])
	animation.timer = 0;
	animation.overrideSpeed =
		Animation::OVERRIDE_SPEED_DISABLE;  // Overrides speed. -2 to disable
	animation.highSpeed = sequence.speed;   // Speed when lerp is 100
	animation.lowSpeed = sequence.altSpeed; // Speed when lerp is 0
	animation.lerp = 100; // For variable speed (0-100). -1 to disable (highSpeed will be used)
	animation.finished = false; // Flag turns on when animation finishes

	return *this;
}

Sprite& Sprite::setFrame(int frame)
{
	if (frame < 0 || frame >= definition.sequences[animation.seqIdx].imageIndexes.size())
	{
		animation.frameIdx = 0;
	}
	else
	{
		animation.frameIdx = frame;
	}
	animation.timer = 0;
	return *this;
}

Sprite& Sprite::setSpeed(int speed)
{
	animation.overrideSpeed = speed;
	return *this;
}

Sprite& Sprite::setLerp(int lerpVal)
{
	animation.lerp = lerpVal;
	return *this;
}

Sprite& Sprite::resetSpeed()
{
	animation.overrideSpeed = Sprite::Animation::OVERRIDE_SPEED_DISABLE;
	return *this;
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

void Sprite::update(float deltaTime)
{
	if (!animation.paused) {
		animation.finished = false;

		int effectiveSpeed = animation.calculateEffectiveSpeed();
		animation.timer += static_cast<int>(deltaTime * effectiveSpeed * 100);

		auto& currentSequence = definition.sequences[animation.seqIdx];
		int nOFrames = currentSequence.imageIndexes.size();

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
	int absW = static_cast<int>(src.w * fabs(xscale));
	int absH = static_cast<int>(src.h * fabs(yscale));

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

	// Adjust destination rectangle so that the anchor lands at offsetFromViewport.
	SDL_Rect dst;
	if (xscale < 0) {
		dst.x = offsetFromViewport.x - (absW - scaledAnchorX);
	}
	else {
		dst.x = offsetFromViewport.x - scaledAnchorX;
	}
	if (yscale < 0) {
		dst.y = offsetFromViewport.y - (absH - scaledAnchorY);
	}
	else {
		dst.y = offsetFromViewport.y - scaledAnchorY;
	}
	dst.w = absW;
	dst.h = absH;

	// Compute the rotation center (pivot) relative to the destination rectangle.
	SDL_Point center;
	center.x = (xscale < 0) ? (absW - scaledAnchorX) : scaledAnchorX;
	center.y = (yscale < 0) ? (absH - scaledAnchorY) : scaledAnchorY;

	SDL_SetTextureAlphaMod(definition.spritesheet, alpha);

	SDL_RenderCopyEx(renderer, definition.spritesheet, &src, &dst, static_cast<double>(angle),
		&center, flip);

	SDL_SetTextureAlphaMod(definition.spritesheet, 255);
}

Sprite::Image::Image(int x, int y, int w, int h, int cx, int cy)
{
	region = { x,y,w,h };
	anchor = { cx,cy };
}

Sprite::Animation::Sequence& Sprite::Definition::addSequence()
{
	Sprite::Animation::Sequence seq;
	sequences.push_back(std::move(seq));
	return sequences.back();
}

Sprite::Animation::Sequence& Sprite::Definition::addSequence(int speed, int altSpeed, int loopTo)
{
	Sprite::Animation::Sequence seq(speed, altSpeed, loopTo);
	sequences.push_back(std::move(seq));
	return sequences.back();
}
