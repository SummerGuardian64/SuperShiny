#pragma once
#include "SdlTexture.h"
#include "SDL.h"
#include <vector>
#include <memory>
#include "DrawContext.h"

namespace ssge
{
	class Sprite
	{
	public:
		class Image // An image is just a to-be-cropped part of the spritesheet texture
		{
		public: //TODO: Encapsulate after everything properly works
			SDL_Point anchor = { 0,0 };
			SDL_Rect region = { 0,0,0,0 }; // Region of the sprite's texture that contains the sprite image
			Image(int x, int y, int w, int h, int cx, int cy);
		};

		class Animation // Active animation with all of its active features (playback, sequence
			// selection, etc.)
		{
		public: //TODO: Encapsulate after everything properly works
			class Sequence // Definition of an animation
			{
			public: //TODO: Encapsulate after everything properly works
				std::vector<int>
					imageIndexes; // Indexes of sprite images belonging to this animation sequence
				int speed;        // Animation speed
				// 0 means standstill
				// 10 means 1 frame per second
				// 20 means 2 frames per second
				// etc.
				// However, -1 means 1 frame (image-wise) per 1 frame (tick-wise)
				// regardless of the refresh rate or draw-wise FPS
				int altSpeed;     // Alternative speed (means low speed if used. 255 - disabled)
				int loopTo; // Index of image that follows the last image (if looped. 255 means no
				// loop)

				Sequence();
				Sequence(int speed, int altSpeed, int loopTo);
			};

			Animation();

			// Setting for overrideSpeed to stop overriding.
			static const int OVERRIDE_SPEED_DISABLE = -2;

			// NOTE: This is not like pausing an animation!
			// It's more like "This is a slideshow" or "We wanna manually change frames of this"
			static const int SPEED_STANDSTILL = 0;

			// Setting for speed: 1 frame (image-wise) per 1 frame (tick-wise) regardless of the
			// refresh rate or draw-wise FPS
			static const int SPEED_FRAME_PER_FRAME = -1;

			// Setting for lerp. Disables linear interpolation.
			static const int LERP_DISABLE = -1;

		private:
			friend class Sprite; // Only Sprite may access these

			bool paused;
			int seqIdx;   // Current sequence index
			int frameIdx; // Current frame (used as sequence.imageIndexes[frame])
			int timer;
			int overrideSpeed; // Overrides speed. -2 to disable
			int highSpeed;     // Speed when lerp is 100
			int lowSpeed;      // Speed when lerp is 0
			int lerp; // For variable speed (0-100). -1 to disable (highSpeed will be used)
			bool finished; // Flag turns on when animation finishes

			int calculateEffectiveSpeed() const; // Returns overrideSpeed if enabled, otherwise
			// calculates linear interpolation
		};

		class Definition {
		public: //TODO: Encapsulate after everything properly works
			Definition() = default;
			Definition(std::string spritesheetPath);;
			std::string spritesheetPath;
			bool load(SDL_Renderer* renderer);
			void unload();
			bool isLoaded() const;
			SdlTexture spritesheet;
			std::vector<Sprite::Animation::Sequence> sequences;
			std::vector<Sprite::Image> images; // Contains the frames of the sprite
			Sprite::Animation::Sequence& addSequence();
			Sprite::Animation::Sequence& addSequence(int speed, int altSpeed, int loopTo);
		};

		// A Sprite can only made according to its definition.
		// Without the definition, the sprite cannot be made.
		Sprite(const Sprite::Definition& definition);

		bool isPlaying() const; // Calls animation.IsPlaying()

		int getSeqIdx() const;                // Calls animation.GetSeqIdx()
		int getCurrentAnimationFrame() const; // Calls animation.GetAnimFrame()
		int getAnimationSpeed() const;        // Calls animation.GetSpeed()
		//todo: bool IsFinished() const;

		// These will stay public
		Uint8 alpha;
		int angle;
		int blend; //todo: rgb variable for SDL2???
		float xscale;
		float yscale;

		Sprite& play();
		Sprite& stop();
		Sprite& rewind();
		Sprite& nextFrame();
		Sprite& prevFrame();
		Sprite& setSequence(int seqIdx);
		Sprite& setFrame(int frame);
		Sprite& setSpeed(int speed);
		Sprite& setLerp(int lerp);
		Sprite& resetSpeed();

		SDL_Rect getBounds() const;

		// CLARIFICATIONS: This is how rendering a Sprite works
		// Calculating frame index is needed to fetch the current image which is needed by Render
		// function to know how to tell SDL2's rendering functions to properly crop the wanted
		// image for the sprite out of the spritesheet and put it onto the render target (which is
		// set for the renderer and none of Sprite class's responsibility)
		// Update function updates all necessary timers by calling animation.Update and passing the
		// provided deltatime.
		//
		// IMPORTANT TIMING NOTE!
		// Update is tick-based while Render is FPS-based.

		int calculateImageIndex()
			const; // definition.sequences[animation.animSeq].imageIndexes[animation.animFrame]
		const Sprite::Image& fetchCurrentImage() const; // definition.images[fetchCurrentImageIndex]

		//TODO: New step context???
		void update(float deltaTime);
		void draw(DrawContext context) const;
		void render(SDL_Renderer* renderer, SDL_Point offsetFromViewport) const;

		Sprite::Animation animation;
		const Sprite::Definition& definition;
	};
};