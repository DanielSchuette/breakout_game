#include "game.hh"

static const SDL_Color blue  = { 37, 26, 239, 255 };
static const SDL_Color red   = { 170, 10, 20, 255 };
static const SDL_Color black = { 15, 15, 15, 255 };

template <typename T>
static T min(const T& a, const T& b) { return (a <= b) ? a : b; }

template <typename T>
static T max(const T& a, const T& b) { return (a >= b) ? a : b; }

Game::Game(bool draw_fps)
    : player({ 40, 650, 150, 20 }), ball({ 120, 120, 30 }), draw_fps(draw_fps)
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

    context.clear_renderer();

    for (const Block& block: blocks)
        context.draw_rectangle(red, block.get_rect());
    context.draw_rectangle(blue, player.get_rect());
    context.draw_circle(black, ball.get_circle());

    if (draw_fps) {
        std::string msg = "FPS: " + std::to_string(current_fps);
        context.draw_text(msg, black, context.get_width()-100,
                          context.get_height()-50);
    }

    context.render_present();
}

void Game::toggle_pause(void)
{
    is_paused = !is_paused;
    if (is_paused) {
        std::string msg = "The game is paused!";
        // the centering is hardcoded and approximated - ugly!
        context.draw_text(msg, black, context.get_width() / 2 - 120,
                          context.get_height() / 2 - 120);
        context.render_present();
    }
}

void Game::update(void)
{
    if (is_paused) return;

    ball.update();
    detect_ball_collision();
}

void Game::detect_ball_collision(void)
{
    // collision between ball and player

    // collision between ball and bricks
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
