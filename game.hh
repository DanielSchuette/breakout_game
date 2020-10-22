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
    Player(int32_t x, int32_t y, int32_t w, int32_t h)
        : rect({ x, y, w, h }) {}

    int32_t  get_xpos(void) const  { return rect.x; }
    void     set_xpos(int32_t x)   { rect.x = x; }
    int32_t  get_width(void) const { return rect.w; }
    SDL_Rect get_rect(void) const  { return rect; }
};

struct Ball {
private:
    shapes::Circle circle;
    int32_t        xdir = 2, ydir = 3;
public:
    Ball(int32_t x, int32_t y, int32_t w) : circle({ x, y, w }) {}

    void update(void);
    bool collides_with(Player);
    bool collides_with(Block);

    shapes::Circle get_circle(void) const  { return circle; }
};

struct Block {
private:
    SDL_Rect rect;
    uint8_t strength; // how often we need to hit this block to for it disappear
public:
    Block(int32_t x, int32_t y, int32_t w, int32_t h, int8_t s)
        : rect({ x, y, w, h }), strength(s) {}

    int8_t   get_strength(void) const { return strength; }
    SDL_Rect get_rect(void) const     { return rect; }
};

class Game {
    Context            context;
    Player             player;
    Ball               ball;
    std::vector<Block> blocks;
    bool               draw_fps;
    const uint8_t      xoffset = 10;
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

    void update_current_fps(uint32_t fps) { current_fps = fps; }
};

#endif /* _GAME_H_ */
