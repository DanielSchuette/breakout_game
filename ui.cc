#include <algorithm>
#include <cassert>
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "context.hh"
#include "ui.hh"
#include "utils.hh"

ui::Button::Button(const ui::Button& other)
{
    rect = { other.rect.x, other.rect.y, other.rect.w, other.rect.h };
    text = other.text;
    callback = other.callback;
    user_data = other.user_data;
}

ui::Button& ui::Button::operator=(ui::Button other)
{
    // copy & swap (needs a working swap implementation!)
    swap(*this, other);
    return *this;
}

ui::Text& ui::Text::operator=(ui::Text other)
{
    swap(*this, other);
    return *this;
}

ui::Text::Text(const ui::Text& other)
{
    text = other.text;
    x = other.x;
    y = other.y;
    w = other.w;
    h = other.h;
    font = other.font;
    color = other.color;
}

/* Add this button to the renderer of the given context. It's important to note
 * that the caller must still invoke `render_present()' on the context.
 * TODO:
 * improve button press event cascading
 * improve ui button storage in game class
 * center button text automatically (what to do about long text?)
 * pass main color via parameter
 * determine "pressed" color from color parameter (-50, -20, -20)
 * make "shadow colors" private static class attributes
 * experiment with different shadows for pressed and non-pressed states
 */
bool ui::Button::render(Context& context, int32_t x, int32_t y, bool pressed)
{
    SDL_Rect shadow1 = { rect.x+1, rect.y+1, rect.w-2, rect.h-2 };
    SDL_Rect shadow2 = { rect.x+2, rect.y+2, rect.w-4, rect.h-4 };
    SDL_Rect shadow3 = { rect.x+3, rect.y+3, rect.w-6, rect.h-6 };

    /* To determine if the button needs to be rendered, we must check the (x,y)
     * boundaries. That's only relevant if the button was pressed, though.
     */
    if (pressed) {
        if (x < rect.x || x > (rect.x + rect.w) ||
            y < rect.y || y > (rect.y + rect.h))
            return false;
        context.draw_rectangle(green1, rect);
        context.draw_rectangle(grey1-70, shadow1, false);
        context.draw_rectangle(grey2-70, shadow2, false);
        context.draw_rectangle(grey3-70, shadow3, false);
        callback(user_data);
    } else {
        context.draw_rectangle(green0, rect);
        context.draw_rectangle(grey1, shadow1, false);
        context.draw_rectangle(grey2, shadow2, false);
        context.draw_rectangle(grey3, shadow3, false);
    }
    context.draw_rectangle(grey0, rect, false);
    if (!text.is_empty())
        context.draw_text(text.text, text.color, text.x, text.y, text.font);

    return true;
}

/* Since button text is always centered, the (x,y) coordinates of the text are
 * determined such that the rendering routine doesn't need to care about
 * calculating anything.
 */
void ui::Button::add_text(const std::string& text, TTF_Font* font)
{
    assert(font);
    int w, h;
    TTF_SizeText(font, text.c_str(), &w, &h);
    if (w > rect.w || h > rect.h) {
        std::cerr << "error: button text is too long\n";
        exit(1);
    }

    int32_t xoff = (rect.w - w) / 2;
    int32_t yoff = (rect.h - h) / 2;
    this->text = Text(text, rect.x+xoff, rect.y+yoff, w, h);
    this->text.font = font;
}
