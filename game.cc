#include <cassert>
#include <cstdio>
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "context.hh"
#include "game.hh"

template <typename T>
static T min(const T& a, const T& b) { return (a <= b) ? a : b; }

template <typename T>
static T max(const T& a, const T& b) { return (a >= b) ? a : b; }

static const SDL_Color blue  = { 37, 26, 239, 255 };
static const SDL_Color red   = { 170, 10, 20, 255 };
static const SDL_Color green = { 15, 222, 47, 255 };
static const SDL_Color black = { 15, 15, 15, 255 };

static const Player default_player = { 40, 650, 150, 20 };

static const int32_t wball = 35;
static const int32_t xball = default_player.get_xpos() +
                             default_player.get_width()/2;
static const int32_t yball = default_player.get_ypos() - wball/2 - 5;
static const Ball default_ball = { xball, yball, wball };

Game::Game(bool draw_fps)
    : player(default_player), ball(default_ball), draw_fps(draw_fps)
{
    const uint32_t block_width  = 80;
    const uint32_t block_height = 20;
    const uint32_t margin       = 10;
    const uint32_t num_blocks_x = (context.get_width()-margin) / block_width;
    const uint32_t num_blocks_y = 6;

    for (uint32_t x = 0; x < num_blocks_x; x++)
        for (uint32_t y = 0; y < num_blocks_y; y++) {
            blocks.emplace_back(Block(
                    x*block_width + (x+1)*margin,
                    y*block_height + (y+1)*margin,
                    block_width,
                    block_height,
                    num_blocks_y - y
                    ));
        }
}

void Game::render(void)
{
    if (is_paused) return;

    if (state == Game::PLAYING) {
        context.clear_renderer();

        for (const Block& block: blocks) {
            uint8_t strength = block.get_strength();
            uint8_t alpha;
            if (strength > 7)      alpha = 255;
            else if (strength > 6) alpha = 220;
            else if (strength > 5) alpha = 200;
            else if (strength > 4) alpha = 175;
            else if (strength > 3) alpha = 155;
            else if (strength > 2) alpha = 130;
            else if (strength > 1) alpha = 90;
            else                   alpha = 40;
            SDL_Color color = { 170, 10, 20, alpha };
            context.draw_rectangle(color, block.get_rect());
        }
        context.draw_rectangle(blue, player.get_rect());
        context.draw_circle(black, ball.get_circle());

        std::string msg = "Score: " + std::to_string(score);
        context.draw_text(msg, black, 10, context.get_height()-50);
        if (draw_fps) {
            std::string msg = "FPS: " + std::to_string(current_fps);
            context.draw_text(msg, black, context.get_width()-130,
                              context.get_height()-50);
        }
    } else if (state == Game::START) {
        context.clear_renderer(blue);
        std::string msg = "Press SPACE to start!";
        int w, h;
        if (TTF_SizeText(context.get_font(36), msg.c_str(), &w, &h))
            context.quit_on_error(TTF_GetError());
        context.draw_text(msg, black, context.get_width() / 2 - w / 2,
                          context.get_height() / 2 - h / 2, 36);
        msg = "BREAKOUT";
        if (TTF_SizeText(context.get_font(36), msg.c_str(), &w, &h))
            context.quit_on_error(TTF_GetError());
        context.draw_text(msg, black, context.get_width() / 2 - w / 2,
                          context.get_height() / 2 - 150, 36);
    } else if (state == Game::LOST) {
        context.clear_renderer(red);
        std::string msg = "You lost!";
        int w, h;
        if (TTF_SizeText(context.get_font(36), msg.c_str(), &w, &h))
            context.quit_on_error(TTF_GetError());
        context.draw_text(msg, black, context.get_width() / 2 - w / 2,
                          context.get_height() / 2 - h / 2, 36);
    } else if (state == Game::WON) {
        context.clear_renderer(green);
        std::string msg = "You won!";
        int w, h;
        if (TTF_SizeText(context.get_font(36), msg.c_str(), &w, &h))
            context.quit_on_error(TTF_GetError());
        context.draw_text(msg, black, context.get_width() / 2 - w / 2,
                          context.get_height() / 2 - h / 2, 36);
    }

    context.render_present();
}

void Game::key_press(void)
{
    if (state == Game::START) {
        state = Game::PLAYING;
    } else if (state == Game::LOST) {
        context.quit_on_success(0);
    }
}

void Game::toggle_pause(void)
{
    is_paused = !is_paused;
    if (is_paused) {
        std::string msg = "The game is paused!";
        int w, h;
        if (TTF_SizeText(context.get_font(), msg.c_str(), &w, &h))
            context.quit_on_error(TTF_GetError());
        context.draw_text(msg, black, context.get_width() / 2 - w / 2,
                          context.get_height() / 2 - h / 2);
        context.render_present();
    }
}

void Game::update(void)
{
    if (is_paused) return;

    if (state == Game::START) {
        // nothing to update
    } else if (state == Game::PLAYING) {
        ball.update();
        detect_ball_collision();
    } else if (state == Game::LOST) {
        // nothing to update
    }
}

/* Ball reflection heuristic (is this really correct?):
 * The player reflects the ball based on where it was hit (midpoint = straight
 * up, left side = angle to left and proportional to distance from midpoint,
 * right side same as left side). The "walls" (top, left, right) as well as the
 * bricks reflect according to the classic laws of reflection.
 */
void Game::detect_ball_collision(void)
{
    shapes::Circle ball_circ = ball.get_circle();
    int32_t screen_height    = context.get_height();
    int32_t ball_radius      = ball_circ.get_width() / 2;

    // collision/exit at bottom of screen
    if (ball_circ.get_y() + ball_radius >= screen_height) {
        state = Game::LOST;
        return;
    }

    // collision between ball and player
    if (ball.collides_with(player)) {
        // NOTE: Vectors aren't normalized, so the ball will change its speed.
        int32_t wzone = player.get_width() / player.num_zones;
        int32_t diff  = max(1, ball_circ.get_x() - player.get_xpos());
        int32_t zone  = min(10, diff / wzone + 1);

        assert(zone > 0 && zone <= player.num_zones);
        switch (zone) {
        case 1:  ball.set_xdir(-9); ball.set_ydir(-4); break;
        case 2:  ball.set_xdir(-7); ball.set_ydir(-5); break;
        case 3:  ball.set_xdir(-7); ball.set_ydir(-4); break;
        case 4:  ball.set_xdir(-6); ball.set_ydir(-5); break;
        case 5:  ball.set_xdir(-4); ball.set_ydir(-7); break;
        case 6:  ball.set_xdir(4);  ball.set_ydir(-7); break;
        case 7:  ball.set_xdir(6);  ball.set_ydir(-5); break;
        case 8:  ball.set_xdir(7);  ball.set_ydir(-4); break;
        case 9:  ball.set_xdir(7);  ball.set_ydir(-5); break;
        case 10: ball.set_xdir(9);  ball.set_ydir(-4); break;
        }
        return;
    }

    // collision between ball and bricks
    size_t position = 0;
    for (Block& block: blocks) {
        if (ball.collides_with(block)) {
            if (!ball.update_on_collision(block)) {
                blocks.erase(blocks.begin()+position);
                if (++score >= winning_score) state = Game::WON;
            }
            return;
        }
        position++;
    }

    // collision between ball and left/right walls
    if (ball.collides_with_wall(context.get_width())) {
        ball.set_xdir(ball.get_xdir() * -1);
        return;
    }

    // collision between ball and top wall
    if (ball.collides_with_top()) {
        ball.set_ydir(ball.get_ydir() * -1);
        return;
    }
}

void Game::update_x(Game::direction dir)
{
    if (is_paused) return;

    int32_t new_xpos  = player.get_xpos();
    if (dir == Game::LEFT)       new_xpos -= xoffset;
    else if (dir == Game::RIGHT) new_xpos += xoffset;
    else context.quit_on_error("unknown direction on x axis");

    int32_t win_width    = static_cast<int32_t>(context.get_width());
    int32_t player_width = player.get_width();
    if ((new_xpos >= 0) && (new_xpos + player_width <= win_width))
        player.set_xpos(new_xpos);
    else if (new_xpos < 0)
        player.set_xpos(0);
    else if (new_xpos + player_width > win_width)
        player.set_xpos(win_width - player_width);
}

bool Game::is_still_running(void)
{
    if (state != Game::LOST) return true;
    return false;
}

// NOTE: Should we do bounds-checking in this function?
void Ball::update(void)
{
    if (started) {
        circle.update_x(xdir);
        circle.update_y(ydir);
    }
}

bool Ball::collides_with(Player player)
{
    // use a squared hit box
    int32_t b_left = circle.get_x() - circle.get_width()/2;
    int32_t b_right = b_left + circle.get_width()/2;
    int32_t b_bottom = circle.get_y() + circle.get_width()/2;
    int32_t p_left = player.get_xpos();
    int32_t p_right = p_left + player.get_width();
    int32_t p_top = player.get_ypos();

    if (b_right >= p_left && b_bottom >= p_top && b_left <= p_right)
        return true;
    return false;
}

bool Ball::collides_with_wall(int32_t win_width)
{
    int32_t b_left = circle.get_x() - circle.get_width()/2;
    int32_t b_right = circle.get_x() + circle.get_width()/2;

    if (b_left <= 0 && xdir < 0)
        return true;
    else if ( b_right >= win_width && xdir > 0)
        return true;
    return false;
}

bool Ball::collides_with_top(void)
{
    int32_t b_top = circle.get_y() - circle.get_width()/2;

    if (b_top <= 0 && ydir < 0)
        return true;
    return false;
}

bool Ball::collides_with(const Block& block)
{
    int32_t ball_left = circle.get_x() - circle.get_width()/2;
    int32_t ball_right = circle.get_x() + circle.get_width()/2;
    int32_t ball_bottom = circle.get_y() + circle.get_width()/2;
    int32_t ball_top = circle.get_y() - circle.get_width()/2;
    int32_t block_left = block.get_x();
    int32_t block_right = block.get_x() + block.get_width();
    int32_t block_top = block.get_y();
    int32_t block_bottom = block.get_y() + block.get_height();

    if (ball_right >= block_left && ball_bottom >= block_top &&
        ball_left <= block_right && ball_top <= block_bottom)
        return true;
    return false;
}

bool Ball::update_on_collision(Block& block)
{
    bool keep_block = true;
    int8_t new_strength = block.decrease_strength();
    if (new_strength <= 0) keep_block = false;

    int32_t ball_y   = circle.get_y();
    int32_t top_y    = block.get_y();
    int32_t bottom_y = top_y + block.get_height();

    // check for all four possible ball directions which face was hit
    assert(xdir != 0 && ydir != 0);
    if (xdir > 0 && ydir > 0) {           // moving down and right
        if (ball_y < top_y) ydir = -ydir; // top face collision
        else                xdir = -xdir; // left face collision
    } else if (xdir < 0 && ydir > 0) {    // moving down and left
        if (ball_y < top_y) ydir = -ydir;
        else         xdir = -xdir;
    } else if (xdir > 0 && ydir < 0) {    // moving up and right
        if (ball_y < bottom_y) xdir = -xdir;
        else                   ydir = -ydir;
    } else if (xdir < 0 && ydir < 0) {    // moving up and left
        if (ball_y < bottom_y) xdir = -xdir;
        else                   ydir = -ydir;
    }

    return keep_block;
}

void Game::start(void) { if (state == Game::START) state = Game::PLAYING; }
