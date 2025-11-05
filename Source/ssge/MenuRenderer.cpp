#include "MenuRenderer.h"
#include <iostream>

using namespace ssge;

void MenuRenderer::drawText(const char* s, TTF_Font* font, SDL_Color c, int x, int y, bool center)
{
    SDL_Surface* surf = TTF_RenderUTF8_Blended(font, s, c);
    if (!surf) return;
    SDL_Texture* tex = SDL_CreateTextureFromSurface(r_, surf);
    SDL_Rect dst{ x, y, surf->w, surf->h };
    if (center) dst.x -= surf->w / 2;
    SDL_FreeSurface(surf);
    SDL_RenderCopy(r_, tex, nullptr, &dst);
    SDL_DestroyTexture(tex);
}

void MenuRenderer::draw(const Menu& m, SDL_Point origin, int width)
{
    // title
    drawText(m.title.c_str(), titleFont_, style_.clrTitle, origin.x + width / 2, origin.y, true);
    int y = origin.y + style_.titlePx + 20;

    for (size_t i = 0; i < m.items.size(); ++i)
    {
        const auto& it = m.items[i];
        const bool selected = ((int)i == m.selected);
        SDL_Color col = !it.enabled ? style_.clrDisabled
            : selected ? style_.clrSelected
            : style_.clrItem;
        drawText(it.label.c_str(), itemFont_, col, origin.x + width / 2, y, true);
        y += style_.itemPx + m.itemSpacing;
    }
}
