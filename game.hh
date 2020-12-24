#ifndef _GAME_H_
#define _GAME_H_

#include <cstdint>
#include <vector>

#include "context.hh"
#include "shapes.hh"
#include "ui.hh"

class Game;
struct Player;
struct Ball;
struct Block;

struct Player {
private:
    SDL_Rect rect;
public:
    const uint8_t num_zones = 10;
    constexpr Player(int32_t x, int32_t y, int32_t w, int32_t h)
        : rect({ x, y, w, h }) {}

    constexpr void     set_ypos(int32_t y)    { rect.y = y; }
    constexpr void     set_xpos(int32_t x)    { rect.x = x; }
    constexpr int32_t  get_xpos(void) const   { return rect.x; }
    constexpr int32_t  get_ypos(void) const   { return rect.y; }
    constexpr int32_t  get_width(void) const  { return rect.w; }
    constexpr int32_t  get_height(void) const { return rect.h; }
    constexpr SDL_Rect get_rect(void) const   { return rect; }
};

struct Ball {
private:
    shapes::Circle circle;
    bool           started = false;
    int32_t        xdir = 2, ydir = 3;
public:
    constexpr Ball(int32_t x, int32_t y, int32_t w) : circle({ x, y, w }) {}

    void update(void);
    bool update_on_collision(Block&);
    bool collides_with(Player);
    bool collides_with(const Block&);
    bool collides_with_wall(int32_t);
    bool collides_with_top(void);

    constexpr void           start(void)             { started = true; }
    constexpr shapes::Circle get_circle(void) const  { return circle; }
    constexpr void           set_xdir(int32_t x)     { xdir = x; }
    constexpr void           set_ydir(int32_t y)     { ydir = y; }
    constexpr int32_t        get_xdir(void)          { return xdir; }
    constexpr int32_t        get_ydir(void)          { return ydir; }
};

struct Block {
private:
    SDL_Rect rect;
    uint8_t  strength;       // how often it needs to be hit to disappear
    bool     indestructable; // this block never disappears
public:
    constexpr Block(int32_t x, int32_t y, int32_t w, int32_t h, int8_t s)
        : rect({ x, y, w, h }), strength(s) {}

    constexpr int8_t   get_strength(void) const      { return strength; }
    constexpr int8_t   decrease_strength(void)       { return --strength; }
    constexpr bool     is_indestructable(void) const { return indestructable; }
    constexpr SDL_Rect get_rect(void) const          { return rect; }
    constexpr int32_t  get_x(void) const             { return rect.x; }
    constexpr int32_t  get_y(void) const             { return rect.y; }
    constexpr int32_t  get_width(void) const         { return rect.w; }
    constexpr int32_t  get_height(void) const        { return rect.h; }
};

class Game {
    Context            context;
    Player             player;
    Ball               ball;
    std::vector<Block> blocks;
    const bool         draw_fps;
    uint32_t           score = 0;
    const uint32_t     winning_score = 15;
    const uint8_t      xoffset = 15;
    uint32_t           current_fps = 0;

    std::vector<ui::Button> ui_buttons;

    enum class GameState { START, HIGHSCORE, PLAYING, PAUSED, WON, LOST };
    GameState state = GameState::START;
public:
    enum class Direction { LEFT, RIGHT };

    Game(bool = false);

    void render(void);
    void start(void);
    void toggle_pause(void);
    void update(void);
    void update_x(Direction);
    void detect_ball_collision(void);
    void key_press(void);
    void left_button_press(int32_t, int32_t);
    bool is_still_running(void);

    GameState get_game_state(void)             { return state; }
    void      update_current_fps(uint32_t fps) { current_fps = fps; }
    void      start_ball(void)                 { ball.start(); }
};

#endif /* _GAME_H_ */
