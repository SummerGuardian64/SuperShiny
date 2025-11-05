#pragma once
#include "SDL.h"
#include "SDL_image.h"
#include <string>
#include <iostream>
#include <cstddef>

class SdlTexture {
public:
    // Default constructor (creates an empty texture)
    SdlTexture() noexcept : texture(nullptr) {}

    // Constructor from an existing SDL_Texture*
    explicit SdlTexture(SDL_Texture* tex) noexcept : texture(tex) {}

    // Constructor from file path and renderer
    SdlTexture(const std::string& path, SDL_Renderer* renderer) noexcept : texture(nullptr)
    {
        SDL_Surface* loadedSurface = IMG_Load(path.c_str());
        if (!loadedSurface)
        {
            std::cout << "Unable to load image %s! SDL_image Error: %s\n" << path.c_str() << IMG_GetError();
        }
        else
        {
            texture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
            if (!texture)
            {
                std::cout << "Unable to create texture from %s! SDL Error: %s\n" << path.c_str() << SDL_GetError();
            }
            else
            {
                SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
            }
            SDL_FreeSurface(loadedSurface);
        }
    }

    // Assignment operator from SDL_Texture* (lazy initialization/ownership transfer)
    SdlTexture& operator=(SDL_Texture* tex) noexcept {
        if (texture != tex) {
            free();
            texture = tex;
        }
        return *this;
    }

    // Move constructor (transfers ownership)
    SdlTexture(SdlTexture&& other) noexcept {
        texture = other.texture;
        other.texture = nullptr;
    }

    // Move assignment (cleans up old resource and transfers ownership)
    SdlTexture& operator=(SdlTexture&& other) noexcept {
        if (this != &other) {
            free();
            texture = other.texture;
            other.texture = nullptr;
        }
        return *this;
    }

    // Deleted copy constructor & assignment (to prevent duplication of SDL_Texture)
    SdlTexture(const SdlTexture&) = delete;
    SdlTexture& operator=(const SdlTexture&) = delete;

    // Destructor (ensures proper cleanup)
    ~SdlTexture() { free(); }

    // Returns true if texture is valid
    bool isValid() const noexcept { return texture != nullptr; }

    // Returns the raw SDL_Texture* for use with SDL2 functions
    SDL_Texture* get() const noexcept { return texture; }

    // Releases the texture manually (optional)
    void free() noexcept {
        if (texture) {
            SDL_DestroyTexture(texture);
            texture = nullptr;
        }
    }

    // Release ownership of the texture without destroying it
    SDL_Texture* release() noexcept {
        SDL_Texture* tmp = texture;
        texture = nullptr;
        return tmp;
    }

    // Clone into a new SdlTexture (deep copy)
    SdlTexture clone(SDL_Renderer* renderer) const {
        SDL_Texture* newTex = cloneRaw(renderer);
        return SdlTexture(newTex);
    }

    // Clone into a raw SDL_Texture* (deep copy)
    SDL_Texture* cloneRaw(SDL_Renderer* renderer) const {
        if (!texture) return nullptr;

        Uint32 format;
        int access, w, h;
        if (SDL_QueryTexture(texture, &format, &access, &w, &h) != 0) {
            return nullptr;
        }

        SDL_Texture* newTex = SDL_CreateTexture(renderer, format, SDL_TEXTUREACCESS_TARGET, w, h);
        if (!newTex) return nullptr;

        SDL_Texture* oldTarget = SDL_GetRenderTarget(renderer);
        SDL_SetRenderTarget(renderer, newTex);
        SDL_RenderCopy(renderer, texture, nullptr, nullptr);
        SDL_SetRenderTarget(renderer, oldTarget);

        return newTex;
    }

    // Get texture dimensions
    SDL_Rect getDimensions() const
    {
        SDL_Rect dimensions{ 0,0,0,0 };
        SDL_QueryTexture(texture, nullptr, nullptr, &dimensions.w, &dimensions.h);
        return dimensions;
    }

    // Query texture via SDL_QueryTexture
    void query(int& w, int& h) const
    {
        SDL_QueryTexture(texture, nullptr, nullptr, &w, &h);
    }

    // Query texture via SDL_QueryTexture
    void query(int* w, int* h) const
    {
        SDL_QueryTexture(texture, nullptr, nullptr, w, h);
    }

    // Implicit conversion to SDL_Texture*
    operator SDL_Texture* () const noexcept { return texture; }

private:
    SDL_Texture* texture;
};
