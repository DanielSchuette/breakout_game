#ifndef _SHAPES_H_
#define _SHAPES_H_

#include <cstdint>
#include <SDL2/SDL.h>

namespace shapes {
    // `Circle' is fairly similar to `SDL_Rect', see `render()' for the actual
    // pixel-by-pixel rendering algorithm.
    struct Circle {
    private:
        int32_t x, y; // midpoint coordinates
        int32_t w;    // it's a circle, so width and height are the same
    public:
        Circle(int32_t x, int32_t y, int32_t w) : x(x), y(y), w(w) {}

        void render(SDL_Renderer*) const;
        void render1(SDL_Renderer*) const;

        void update_x(int32_t x) { this->x += x; };
        void update_y(int32_t y) { this->y += y; };
        int32_t get_x(void)      { return this->x; }
        int32_t get_y(void)      { return this->y; }
        int32_t get_width(void)  { return this->w; }
    };
}

#endif /* _SHAPES_H_ */
