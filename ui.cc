#include <algorithm>
#include <SDL2/SDL.h>

#include "context.hh"
#include "ui.hh"

ui::Button::Button(int32_t x, int32_t y, int32_t w, int32_t h)
    : rect({ x, y, w, h }) {}

ui::Button::Button(const ui::Button& other)
{
    rect = { other.rect.x, other.rect.y, other.rect.w, other.rect.h };
}

ui::Button& ui::Button::operator=(ui::Button other)
{
    // copy & swap (needs a working swap implementation!)
    swap(*this, other);
    return *this;
}

/* Add this button to the renderer of the given context. It's important to note
 * that the caller must still invoke `render_present()' on the context.
 */
void ui::Button::render(Context& context)
{
    static SDL_Color green = { 130, 220, 80, 255 };
    static SDL_Color grey0 = { 40, 40, 40, 255 };
    static SDL_Color grey1 = { 70, 70, 70, 255 };
    static SDL_Color grey2 = { 130, 130, 130, 255 };
    static SDL_Color grey3 = { 190, 190, 190, 255 };
    SDL_Rect shadow1 = { rect.x+1, rect.y+1, rect.w-2, rect.h-2 };
    SDL_Rect shadow2 = { rect.x+2, rect.y+2, rect.w-4, rect.h-4 };
    SDL_Rect shadow3 = { rect.x+3, rect.y+3, rect.w-6, rect.h-6 };
    context.draw_rectangle(green, rect);
    context.draw_rectangle(grey0, rect, false);
    context.draw_rectangle(grey1, shadow1, false);
    context.draw_rectangle(grey2, shadow2, false);
    context.draw_rectangle(grey3, shadow3, false);
}
