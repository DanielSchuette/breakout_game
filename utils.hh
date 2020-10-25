#ifndef _UTILS_H_
#define _UTILS_H_

#include <iostream>
#include <SDL2/SDL.h>

std::ostream& operator<<(std::ostream&, const SDL_Rect&);

template <typename T>
SDL_Color operator-(SDL_Color color, T val)
{
    SDL_Color new_color;
    new_color.r = color.r - val;
    new_color.g = color.g - val;
    new_color.b = color.b - val;
    new_color.a = color.a - val;
    return new_color;
}

#endif /* _UTILS_H_ */
