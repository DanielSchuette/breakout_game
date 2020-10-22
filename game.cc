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

static const Ball   default_ball   = { 80, 320, 35 };
static const Player default_player = { 40, 650, 150, 20 };

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
            blocks.emplace_back(x*block_width + (x+1)*margin,
                                y*block_height + (y+1)*margin,
                                block_width, block_height, 4);
        }
}

void Game::render(void)
{
    if (is_paused) return;

    if (state == Game::PLAYING) {
        context.clear_renderer();

        for (const Block& block: blocks)
            context.draw_rectangle(red, block.get_rect());
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
        if (TTF_SizeText(context.get_font(), msg.c_str(), &w, &h))
            context.quit_on_error(TTF_GetError());
        context.draw_text(msg, black, context.get_width() / 2 - w / 2,
                          context.get_height() / 2 - h / 2, 36);
    } else if (state == Game::START) {
        // TODO
    }

    context.render_present();
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

    // TODO: do updates depending on game state
    ball.update();
    detect_ball_collision();
}

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
        // TODO
        return;
    }

    // collision between ball and bricks

    // collision between ball and left/right/top
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

// NOTE: Should we do bounds-checking in this function?
void Ball::update(void)
{
    circle.update_x(xdir);
    circle.update_y(ydir);
}


bool Ball::collides_with(Player)
{
    return true;
}

bool Ball::collides_with(Block)
{
    return true;
}
