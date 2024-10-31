#ifndef ASTEROIDS_APP_HPP_
#define ASTEROIDS_APP_HPP_
#include "app.hpp"
#include <list>

namespace may
{
  class asteroids_app : public app
  {
    floating_actor player;
    std::string _bullet_image;
    std::list<image_actor> _bullets;

  public:
    asteroids_app();
    asteroids_app(int width, int height);
    asteroids_app(const char *title, int width, int height);
    virtual void game_loop(SDL_Renderer *renderer) override;

    inline void player_image(const char *path) { player.image(path); }
    inline void bullet_image(const char *path) { _bullet_image = path; }
  };
}

#endif /* ASTEROIDS_APP_HPP_ */