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
            if (strength > 6)      alpha = 255;
            else if (strength > 4) alpha = 210;
            else if (strength > 2) alpha = 140;
            else                   alpha = 50;
            SDL_Color color = { 170, 10, 20, alpha };
            context.draw_rectangle(color, block.get_rect());
        }
        context.draw_rectangle(blue, player.get_rect());
        context.draw_circle(black, ball.get_circle());

        if (draw_fps) {
            std::string msg = "FPS: " + std::to_string(current_fps);
            context.draw_text(msg, black, context.get_width()-130,
                              context.get_height()-50);
        }
    } else if (state == Game::LOST) {
        context.clear_renderer(red);
        std::string msg = "You lost!";
        int w, h;
        if (TTF_SizeText(context.get_font(36), msg.c_str(), &w, &h))
            context.quit_on_error(TTF_GetError());
        context.draw_text(msg, black, context.get_width() / 2 - w / 2,
                          context.get_height() / 2 - h / 2, 36);
    } else if (state == Game::START) {
        context.clear_renderer(blue);
        std::string msg = "Press SPACE to start!";
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
    int32_t screen_height = context.get_height();
    int32_t ball_radius = ball_circ.get_width() / 2;

    // collision/exit at bottom of screen
    if (ball_circ.get_y() - ball_radius > screen_height) {
        state = Game::LOST;
        return;
    }

    // collision between ball and player
    if (ball.collides_with(player)) {
        // NOTE: vectors aren't normalized, so the ball will change its speed
        // | -- | -- | -- | -- | -- |
        //  4/1  3/2  2/2  3/2  4/1
        // Like this, but with 10 zones and 10/2 max vector at edge.
        //int32_t old_ydir = ball.get_ydir();
        ball.set_xdir(10);
        ball.set_ydir(-2);
        return;
    }

    // collision between ball and bricks
    for (const Block& block: blocks) {
        if (ball.collides_with(block)) {
            // TODO
            return;
        }
    }

    // collision between ball and left/right/top
    if (ball.collides_with_wall()) {
        // TODO
        return;
    }
}

void Game::update_x(Game::direction dir)
{
    if (is_paused) return;

    int32_t new_xpos = player.get_xpos();
    if (dir == Game::LEFT)       new_xpos -= xoffset;
    else if (dir == Game::RIGHT) new_xpos += xoffset;
    else context.quit_on_error("unknown direction on x axis");

    int32_t win_width = static_cast<int32_t>(context.get_width());
    if ((new_xpos >= 0) && (new_xpos + player.get_width() <= win_width))
        player.set_xpos(new_xpos);
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
    int32_t bx = circle.get_x();
    int32_t by = circle.get_y() + (circle.get_width() / 2);
    int32_t px1 = player.get_xpos();
    int32_t px2 = player.get_xpos() + player.get_width();
    int32_t py = player.get_ypos();

    // NOTE: on the y axis, we don't check for an exact match
    if (bx >= px1 && bx <= px2 && by >= py) return true;
    return false;
}

bool Ball::collides_with_wall(void)
{
    return false;
}

bool Ball::collides_with(Block)
{
    return false;
}
