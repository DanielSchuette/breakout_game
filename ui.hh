#ifndef _UI_H_
#define _UI_H_

#include <cstdint>
#include <functional>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>

#include "context.hh"

namespace ui {
    class Button;
    struct Text {
        std::string text = "";
        int32_t     x = 0, y = 0;
        int32_t     w = 0, h = 0;   // determined based on the used font
        TTF_Font*   font = nullptr; // take care with fonts we don't posses
        SDL_Color   color = { 40, 40, 40, 255 };

        Text(void) = default;
        Text(const std::string& t, int32_t x, int32_t y, int32_t w, int32_t h)
            : text(t), x(x), y(y), w(w), h(h) {}
        Text(const Text&);
        Text(Text&&) = delete;
        ~Text(void) = default;
        Text& operator=(Text);

        bool is_empty(void)             { return text.empty(); }
        void set_color(SDL_Color color) { this->color = color; }

        friend void swap(Text& fst, Text& snd)
        {
            using std::swap;
            swap(fst.x, snd.x);
            swap(fst.y, snd.y);
            swap(fst.w, snd.w);
            swap(fst.h, snd.h);
            swap(fst.text, snd.text);
            swap(fst.color, snd.color);
            TTF_Font* f = fst.font;
            fst.font = snd.font;
            snd.font = f;
        }
    };
}

class ui::Button {
    SDL_Rect rect;
    Text     text = Text();

    std::function<void(void*)> callback;

    constexpr static SDL_Color black0 = { 40, 40, 40, 255 };
    constexpr static SDL_Color green0 = { 160, 220, 100, 255 };
    constexpr static SDL_Color green1 = { 140, 170, 80, 255 };
    constexpr static SDL_Color grey0  = { 40, 40, 40, 255 };
    constexpr static SDL_Color grey1  = { 70, 70, 70, 255 };
    constexpr static SDL_Color grey2  = { 130, 130, 130, 255 };
    constexpr static SDL_Color grey3  = { 190, 190, 190, 255 };
public:
    Button(int32_t x, int32_t y, int32_t w, int32_t h)
        : rect({ x, y, w, h }) {}
    Button(const Button&);
    Button(Button&& other) { rect = other.rect; text = other.text; }
    ~Button(void) {}
    Button& operator=(Button);

    void add_text(const std::string& text, TTF_Font*);
    bool render(Context& context, int32_t, int32_t, bool = false);

    void add_callback(std::function<void(void*)> fn) { callback = fn; }
    std::string get_text(void)                       { return text.text; }

    friend void swap(Button& fst, Button& snd)
    {
        using std::swap; // enable ADL
        swap(fst.rect, snd.rect);
        swap(fst.text, snd.text);
        swap(fst.callback, snd.callback);
    }
};

#endif /* _UI_H_ */
