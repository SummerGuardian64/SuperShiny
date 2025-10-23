#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include "Menu.h"

namespace ssge
{
    struct MenuStyle
    {
        SDL_Color clrTitle{ 255,64,32,255 };
        SDL_Color clrItem{ 255,192,64,255 };
        SDL_Color clrDisabled{ 128,128,128,255 };
        SDL_Color clrSelected{ 255,255,128,255 };
        int titlePx = 28;
        int itemPx = 22;
    };

    class MenuRenderer
    {
    public:
        MenuRenderer(SDL_Renderer* r, TTF_Font* titleFont, TTF_Font* itemFont, MenuStyle style)
            : r_(r), titleFont_(titleFont), itemFont_(itemFont), style_(style)
        {
        }

        void draw(const Menu& m, SDL_Point origin, int width);

    private:
        SDL_Renderer* r_ = nullptr;
        TTF_Font* titleFont_ = nullptr;
        TTF_Font* itemFont_ = nullptr;
        MenuStyle style_;
        void drawText(const char* s, TTF_Font* font, SDL_Color c, int x, int y, bool center);
    };
} // namespace ssge
