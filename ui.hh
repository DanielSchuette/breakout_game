#ifndef _UI_H_
#define _UI_H_

#include <cstdint>
#include <SDL2/SDL.h>
#include <string>

#include "context.hh"

namespace ui {
    class Button;
}

class ui::Button {
    SDL_Rect rect;
    std::string text;
public:
    Button(int32_t, int32_t, int32_t, int32_t);
    Button(const Button&);
    Button(Button&&) = default;
    ~Button(void) = default;
    Button& operator=(Button);

    void add_text(const std::string& text) { this->text = text; }
    void render(Context& context);

    friend void swap(Button& fst, Button& snd)
    {
        using std::swap; // enable ADL

        swap(fst.rect, snd.rect);
        swap(fst.text, snd.text);
    }
};

#endif /* _UI_H_ */
