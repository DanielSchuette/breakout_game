#include <algorithm>
#include <cmath>
#include <cstdio>
#include <SDL2/SDL.h>

#include "shapes.hh"

/* The rendering algorithm scans the x axis in the interval [-radius, +radius]
 * and for every x in the interval, it calculates the intersection points y1 and
 * y2 of an imaginary scanline with the given circle. The scanline is imaginary,
 * because we don't need to do bounds checking - SDL doesn't rendering anything
 * outside of the current window. Before drawing a line between y1 and y2, we
 * translate all points to the actual midpoint of the circle (in the algorithm,
 * we always assume (0,0)) and draw the line.
 */
void shapes::Circle::render(SDL_Renderer* renderer) const
{
    int32_t radius = w / 2;
    for (int32_t scanline = -radius; scanline <= radius; scanline++) {
        int32_t y1 = (int32_t)sqrt((double)(radius*radius - scanline*scanline));
        int32_t translated_x = scanline + x;
        int32_t translated_y1 = y1 + y;
        int32_t translated_y2 = -y1 + y;

        SDL_RenderDrawLine(renderer, translated_x, translated_y1,
                                       translated_x, translated_y2);
    }
}
