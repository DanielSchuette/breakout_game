#include <iostream>
#include <SDL2/SDL.h>

#include "utils.hh"

std::ostream& operator<<(std::ostream& os, const SDL_Rect& r)
{
    os << "x=" << r.x << " y=" << r.y << " w=" << r.w << " h=" << r.h;
    return os;
}
