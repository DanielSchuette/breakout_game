#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <thread>

#include "context.hh"
#include "shapes.hh"

Context::Context(void)
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) quit_on_error(SDL_GetError());
    if (TTF_Init() != 0)                    quit_on_error(TTF_GetError());

    int img_init_flags = IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF;
    if (IMG_Init(img_init_flags) != img_init_flags)
        quit_on_error(IMG_GetError());

    // X11 usually pings windows to check if they're hung - which we don't need
    bool success = SDL_SetHint(SDL_HINT_VIDEO_X11_NET_WM_PING, "0");
    if (!success) std::cerr << "unable to set hint\n";

    window = SDL_CreateWindow(title, win_x_pos, win_y_pos, win_width,
                              win_height, SDL_WINDOW_SHOWN);
    if (!window) quit_on_error(SDL_GetError());

    /* Enabling vsync should limit the framerate to whatever the video card is
     * capable of. It's still good practice to limit the frame rate in the
     * game's main loop.
     * NOTE: Maybe, enabling alpha blending globally is bad?
     */
    uint32_t render_flags = SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC;
    renderer = SDL_CreateRenderer(window, -1, render_flags);
    if (!renderer) quit_on_error(SDL_GetError());
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    font24 = TTF_OpenFont("./assets/fonts/OpenSans-Bold.ttf", 24);
    if (!font24) quit_on_error(TTF_GetError());
    font36 = TTF_OpenFont("./assets/fonts/OpenSans-Bold.ttf", 36);
    if (!font36) quit_on_error(TTF_GetError());
}

Context::~Context(void)
{
    for (std::pair<const char*, SDL_Texture*> pair: texture_map)
        SDL_DestroyTexture(pair.second);
    if (window)   SDL_DestroyWindow(window);
    if (renderer) SDL_DestroyRenderer(renderer);
    if (font24)   TTF_CloseFont(font24);
    if (font36)   TTF_CloseFont(font36);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

void Context::quit_on_error(const char* msg) const
{
    std::cerr << "error: " << msg << '\n';
    exit(1);
}

/* Clear the background. Currently, the default color is hard-coded in the
 * header (as a default argument, not a class attribute).
 */
void Context::clear_renderer(SDL_Color c)
{
    SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
    SDL_RenderClear(renderer);
}

/* Draw an `SDL_Rect' to the screen. The caller must still invoke
 * `render_present()'.
 */
void Context::draw_rectangle(const SDL_Color& color, const SDL_Rect& rect,
                             bool fill)
{
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    if (fill) SDL_RenderFillRect(renderer, &rect);
    else      SDL_RenderDrawRect(renderer, &rect);
}

void Context::draw_line(const SDL_Color& color, int32_t x0, int32_t y0,
                        int32_t x1, int32_t y1)
{
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderDrawLine(renderer, x0, y0, x1, y1);
}

void Context::draw_circle(const SDL_Color& color, const shapes::Circle& circ)
{
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    circ.render(renderer);
}

/* Copy text to the renderer. The caller must still invoke `render_present()'.
 * The standard font set for this context is used. If x and/or y are ``-1''
 * then the given text will be centered in that direction.
 */
void Context::draw_text(const std::string& text, const SDL_Color& color,
                        int32_t x, int32_t y, uint8_t fontsize)
{
    SDL_Surface* sf;
    if (fontsize == 24)
        sf = TTF_RenderText_Solid(font24, text.c_str(), color);
    else if (fontsize == 36)
        sf = TTF_RenderText_Solid(font36, text.c_str(), color);
    else
        quit_on_error("invalid font size specified");
    if (!sf) quit_on_error(TTF_GetError());

    SDL_Texture* tx = SDL_CreateTextureFromSurface(renderer, sf);
    if (!tx) quit_on_error(SDL_GetError());

    int w, h;
    if (x == -1 || y == -1)
        if (TTF_SizeText(get_font(36), text.c_str(), &w, &h))
            quit_on_error(TTF_GetError());

    SDL_Rect r;
    if (x == -1) r.x = get_width() / 2 - w / 2;
    else         r.x = x;
    if (y == -1) r.y = get_height() / 2 - h / 2;
    else         r.y = y;

    SDL_QueryTexture(tx, NULL, NULL, &r.w, &r.h);
    copy_texture_to_renderer(tx, &r);
    SDL_FreeSurface(sf);
    SDL_DestroyTexture(tx);
}

void Context::draw_text(const std::string& text, const SDL_Color& color,
                        int32_t x, int32_t y, TTF_Font* font)
{
    SDL_Surface* sf = TTF_RenderText_Solid(font, text.c_str(), color);
    if (!sf) quit_on_error(TTF_GetError());
    SDL_Texture* tx = SDL_CreateTextureFromSurface(renderer, sf);
    if (!tx) quit_on_error(SDL_GetError());

    int w, h;
    if (x == -1 || y == -1)
        if (TTF_SizeText(get_font(36), text.c_str(), &w, &h))
            quit_on_error(TTF_GetError());

    SDL_Rect r;
    if (x == -1) r.x = get_width() / 2 - w / 2;
    else         r.x = x;
    if (y == -1) r.y = get_height() / 2 - h / 2;
    else         r.y = y;

    SDL_QueryTexture(tx, NULL, NULL, &r.w, &r.h);
    copy_texture_to_renderer(tx, &r);
    SDL_FreeSurface(sf);
    SDL_DestroyTexture(tx);
}

void Context::draw_texture(const char* path, const SDL_Rect& dest)
{
    SDL_Texture* tex;

    if (texture_map.find(path) == texture_map.end()) {
        SDL_Surface* surf = IMG_Load(path);
        tex = SDL_CreateTextureFromSurface(renderer, surf);
        SDL_FreeSurface(surf);
        texture_map[path] = tex;
    } else {
        tex = texture_map[path];
    }

    SDL_Rect r = dest;
    copy_texture_to_renderer(tex, &r);
}

/* This overload does cropping of the source based on the coordinates supplied
 * via `srcr'. Otherwise, the code is identical to `draw_texture()' above.
 * TODO: instead of just looking up the path in the map, we now need to check
 *       if the texture exists in the correct cropping dimensions.
 */
void Context::draw_texture(const char* path, const SDL_Rect& src,
                           const SDL_Rect& dest)
{
    SDL_Texture* tex;

    if (texture_map.find(path) == texture_map.end()) {
        SDL_Surface* surf = IMG_Load(path);
        crop_surface(&surf, src.x, src.y, src.w, src.h);
        tex = SDL_CreateTextureFromSurface(renderer, surf);
        SDL_FreeSurface(surf);
        texture_map[path] = tex;
    } else {
        tex = texture_map[path];
    }

    SDL_Rect r = dest;
    copy_texture_to_renderer(tex, &r);
}

void Context::crop_surface(SDL_Surface** surface, uint32_t x, uint32_t y,
                           uint32_t width, uint32_t height)
{
    uint32_t rm, gm, bm, am;
    #if SDL_BYTEORDER == SDL_BIG_ENDIAN
        rm = 0xff000000; gm = 0x00ff0000; bm = 0x0000ff00; am = 0x000000ff;
    #else
        rm = 0x000000ff; gm = 0x0000ff00; bm = 0x00ff0000; am = 0xff000000;
    #endif

    SDL_Surface* cropped_surf = SDL_CreateRGBSurface(0, width, height, 32,
                                                     rm, gm, bm, am);
    if (cropped_surf == NULL)
        quit_on_error(SDL_GetError());

    SDL_Rect r;
    r.x = x;
    r.y = y;
    r.w = width;
    r.h = height;
    if (SDL_BlitSurface(*surface, &r, cropped_surf, NULL) != 0)
        quit_on_error(SDL_GetError());

    SDL_FreeSurface(*surface);
    *surface = cropped_surf;
}

/* Copy a texture to the renderer. The caller must still invoke
 * `render_present()'. We use this function for better naming and convenience,
 * since we usually don't specify any copy flags.
 */
void Context::copy_texture_to_renderer(SDL_Texture* texture, SDL_Rect* dest)
{
    SDL_RenderCopy(renderer, texture, NULL, dest);
}


TTF_Font* Context::get_font(uint8_t fontsize) const
{
    if (fontsize == 24)      return font24;
    else if (fontsize == 36) return font36;
    return nullptr;
}

void Context::play_audio(const char* audio_path)
{
    auto fn = [&, audio_path](void) -> void {
        SDL_AudioSpec wav_spec;
        uint32_t      wav_length;
        uint8_t*      wav_buffer;

        if (SDL_LoadWAV(audio_path, &wav_spec,
                        &wav_buffer, &wav_length) == NULL) {
            std::cerr << "error: unable to load .wav file\n";
            quit_on_error(SDL_GetError());
        }

        SDL_AudioDeviceID device_id = SDL_OpenAudioDevice(NULL, 0, &wav_spec,
                                                          NULL, 0);
        SDL_QueueAudio(device_id, wav_buffer, wav_length);
        SDL_PauseAudioDevice(device_id, 0);

        SDL_Delay(3000);   // just a heuristic, no idea how to determine the
                           // length of the sound
        SDL_CloseAudioDevice(device_id);
        SDL_FreeWAV(wav_buffer);
    };
    std::thread audio_thread(fn);
    audio_thread.detach(); // don't block the game
}
