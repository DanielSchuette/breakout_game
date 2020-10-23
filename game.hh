#ifndef _GAME_H_
#define _GAME_H_

#include <cstdint>
#include <vector>

#include "context.hh"
#include "shapes.hh"

class Game;
struct Player;
struct Ball;
struct Block;

struct Player {
private:
    SDL_Rect rect;
public:
    const uint8_t num_zones = 10;
    Player(int32_t x, int32_t y, int32_t w, int32_t h)
        : rect({ x, y, w, h }) {}

    void     set_ypos(int32_t y)    { rect.y = y; }
    void     set_xpos(int32_t x)    { rect.x = x; }
    int32_t  get_xpos(void) const   { return rect.x; }
    int32_t  get_ypos(void) const   { return rect.y; }
    int32_t  get_width(void) const  { return rect.w; }
    int32_t  get_height(void) const { return rect.h; }
    SDL_Rect get_rect(void) const   { return rect; }
};

struct Ball {
private:
    shapes::Circle circle;
    bool           started = false;
    int32_t        xdir = 2, ydir = 3;
public:
    Ball(int32_t x, int32_t y, int32_t w) : circle({ x, y, w }) {}

    void update(void);
    bool update_on_collision(Block&);
    bool collides_with(Player);
    bool collides_with(const Block&);
    bool collides_with_wall(int32_t);
    bool collides_with_top(void);

    void           start(void)             { started = true; }
    shapes::Circle get_circle(void) const  { return circle; }
    void           set_xdir(int32_t x)     { xdir = x; }
    void           set_ydir(int32_t y)     { ydir = y; }
    int32_t        get_xdir(void)          { return xdir; }
    int32_t        get_ydir(void)          { return ydir; }
};

struct Block {
private:
    SDL_Rect rect;
    uint8_t  strength;       // how often it needs to be hit to disappear
    bool     indestructable; // this block never disappears
public:
    Block(int32_t x, int32_t y, int32_t w, int32_t h, int8_t s)
        : rect({ x, y, w, h }), strength(s) {}

    int8_t   get_strength(void) const      { return strength; }
    int8_t   decrease_strength(void)       { return --strength; }
    bool     is_indestructable(void) const { return indestructable; }
    SDL_Rect get_rect(void) const          { return rect; }
    int32_t  get_x(void) const             { return rect.x; }
    int32_t  get_y(void) const             { return rect.y; }
    int32_t  get_width(void) const         { return rect.w; }
    int32_t  get_height(void) const        { return rect.h; }
};

class Game {
    Context            context;
    Player             player;
    Ball               ball;
    std::vector<Block> blocks;
    bool               draw_fps;
    const uint8_t      xoffset = 15;
    uint32_t           current_fps = 0;
    bool               is_paused = false;

    enum game_state { START, PLAYING, LOST };
    game_state         state = START;
public:
    enum direction { LEFT, RIGHT };

    Game(bool);

    void render(void);
    void toggle_pause(void);
    void update(void);
    void update_x(direction);
    void detect_ball_collision(void);
    void key_press(void);
    bool is_still_running(void);

    game_state get_game_state(void)             { return state; }
    void       update_current_fps(uint32_t fps) { current_fps = fps; }
    void       start(void)                      { state = Game::PLAYING; }
    void       start_ball(void)                 { ball.start(); }
};

#endif /* _GAME_H_ */
