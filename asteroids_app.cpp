#include "asteroids_app.hpp"

namespace may
{
  asteroids_app::asteroids_app() : asteroids_app(800, 600)
  {
  }

  asteroids_app::asteroids_app(int width, int height) : asteroids_app("My App", width, height)
  {
  }

  asteroids_app::asteroids_app(const char *title, int width, int height) : app(title, width, height), player("shooter.png", 50, 40)
  {
    background_image("space.png");
  }

  void asteroids_app::game_loop(SDL_Renderer *renderer)
  {
    if (game_state().is_key_set(SDLK_SPACE))
    {
      image_actor bullet("bullet.png", player.position().x + player.width() / 2, player.position().y + player.height() / 2, 10, 10, player.angle());
      bullet.speed(200);

      _bullets.push_back(bullet);

      if (_bullets.size() > 10)
      {
        _bullets.erase(_bullets.begin());
      }

      game_state().key_set(SDLK_SPACE);
    }

    player.input(game_state());
    player.update(game_state());

    loop_on_border(player);

    SDL_Rect fillRect = {
        static_cast<int>(width() / 4),
        static_cast<int>(height() / 4),
        static_cast<int>(width() / 2),
        static_cast<int>(height() / 2)};

    SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0xFF, 0xFF);
    SDL_RenderFillRect(renderer, &fillRect);

    if (may::colliding(fillRect, player.bounding_box()))
    {
      SDL_SetTextureBlendMode(player.image().texture(), SDL_BLENDMODE_MUL);
    }
    else
    {
      SDL_SetTextureBlendMode(player.image().texture(), SDL_BLENDMODE_BLEND);
    }

    player.render(renderer);
    for (auto &bullet : _bullets)
    {
      bullet.move(game_state().delta_time());
      loop_on_border(bullet);
      bullet.render(renderer);
    }
  }
}