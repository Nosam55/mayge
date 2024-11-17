#include "asteroids_app.hpp"

namespace may
{
  asteroids_app::asteroids_app() : asteroids_app(800, 600)
  {
  }

  asteroids_app::asteroids_app(int width, int height) : asteroids_app("My App", width, height)
  {
  }

  asteroids_app::asteroids_app(const char *title, int width, int height) : app(title, width, height)
  {
    background_color(0, 0, 0, 1);
    bullet_image("bullet.png");
  }

  void asteroids_app::game_loop(SDL_Renderer *renderer)
  {
    if (game_state().is_key_set(SDLK_SPACE))
    {
      image_actor bullet(_bullet_image.c_str(), player.position().x + player.width() / 2, player.position().y + player.height() / 2, 10, 10, player.angle());
      bullet.speed(200);

      _bullets.push_back(bullet);

      if (_bullets.size() > 10)
      {
        _bullets.erase(_bullets.begin());
      }

      game_state().key_unset(SDLK_SPACE);
    }

    player.input(game_state());
    player.update(game_state());

    loop_on_border(player);

    player.render(renderer);
    for (auto &bullet : _bullets)
    {
      bullet.move(game_state().delta_time());
      loop_on_border(bullet);
      bullet.render(renderer);
    }
  }
}