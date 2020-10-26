#ifndef _CONTEXT_H_
#define _CONTEXT_H_

#include <cstdint>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <unordered_map>

#include "shapes.hh"

class Context {
    SDL_Window*    window     = nullptr;
    SDL_Renderer*  renderer   = nullptr;
    TTF_Font*      font24     = nullptr;
    TTF_Font*      font36     = nullptr;
    const char*    title      = "Breakout";
    const uint32_t win_x_pos  = SDL_WINDOWPOS_CENTERED;
    const uint32_t win_y_pos  = SDL_WINDOWPOS_CENTERED;
    const uint32_t win_width  = 910;
    const uint32_t win_height = 720;

    std::unordered_map<const char*, SDL_Texture*> texture_map;

    void copy_texture_to_renderer(SDL_Texture*, SDL_Rect*);
    void crop_surface(SDL_Surface**, uint32_t, uint32_t, uint32_t, uint32_t);
public:
    Context(void);
    ~Context(void);

    void draw_line(const SDL_Color&, int32_t, int32_t, int32_t, int32_t);
    void draw_rectangle(const SDL_Color&, const SDL_Rect&, bool = true);
    void draw_circle(const SDL_Color&, const shapes::Circle&);
    void draw_texture(const char*, const SDL_Rect&);
    void draw_texture(const char*, const SDL_Rect&, const SDL_Rect&);
    void draw_text(const std::string&, const SDL_Color&, int32_t, int32_t,
                   uint8_t = 24);
    void draw_text(const std::string&, const SDL_Color&, int32_t, int32_t,
                   TTF_Font*);
    void play_audio(const char*);
    void clear_renderer(SDL_Color = { 180, 180, 180, 255 });
    TTF_Font* get_font(uint8_t = 24) const;

    [[noreturn]] void quit_on_error(const char*) const;
    [[noreturn]] void quit_on_success(int code) const { exit(code); }

    constexpr uint32_t get_height(void) const { return win_height; }
    constexpr uint32_t get_width(void) const  { return win_width; }

    void          render_present(void)     { SDL_RenderPresent(renderer); }
    SDL_Window*   get_window(void) const   { return window; }
    SDL_Renderer* get_renderer(void) const { return renderer; }
};

#endif /* _CONTEXT_H_ */
