#include "app.hpp"
#include "asteroids_app.hpp"
#include "cfg_reader.hpp"

class myactor : public may::animated_actor, public may::floating_actor
{

public:
  myactor() : may::animated_actor(may::spritesheet("orb_sprites.png", 32, 32), 0.0, 0.0, 64, 64, 0.0, 8.0, 32, M_PI)
  {
  }

  virtual void update(may::game_state &state) override
  {
    floating_actor::update(state);
    animated_actor::update(state);

    double vel = sqrtf64(may::sq_mag(velocity()));
    fps(vel / 4);
  }

  virtual void render(SDL_Renderer *renderer) override
  {
    auto tmp = _angle;
    _angle = 0;
    animated_actor::render(renderer);
    _angle = tmp;

    SDL_FPoint v_begin = center() + position();
    SDL_FPoint v_end = v_begin + velocity();

    int head_dist = _width / 2;
    SDL_FPoint head_pos = v_begin + SDL_FPoint{cos(_angle) * head_dist, sin(_angle) * head_dist};

    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderDrawLineF(renderer, v_begin.x, v_begin.y, v_end.x, v_end.y);

    SDL_RenderDrawPointF(renderer, head_pos.x, head_pos.y);
    SDL_RenderDrawPointF(renderer, head_pos.x, head_pos.y + 1);
    SDL_RenderDrawPointF(renderer, head_pos.x, head_pos.y - 1);
    SDL_RenderDrawPointF(renderer, head_pos.x + 1, head_pos.y);
    SDL_RenderDrawPointF(renderer, head_pos.x - 1, head_pos.y);
  }
};

class testapp : public may::app
{
  myactor actor;
  may::image_actor portal;
  SDL_Cursor *_cursor;

public:
  testapp() : testapp("Title") {}
  testapp(const char *title) : testapp(title, 800, 600) {}
  testapp(const char *title, int w, int h) : may::app(title, w, h), portal("bullet.png", 100, 100)
  {
    background_color(0xFF, 0x88, 0xFF, 0xFF);
    portal.position(550, 400);
  }

  ~testapp()
  {
    SDL_FreeCursor(_cursor);
    _cursor = nullptr;
  }

  virtual void init() override
  {
    app::init();

    int err = SDL_SetWindowFullscreen(window().window_ptr(), 0);
    if (err)
    {
      fprintf(stderr, "Error going fullscreen: %s\n", SDL_GetError());
      exit(EXIT_FAILURE);
    }

    _cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_CROSSHAIR);
    SDL_SetCursor(_cursor);
  }

  void game_loop(SDL_Renderer *renderer) override
  {
    auto &state = game_state();

    SDL_FPoint pos = actor.position();
    actor.input(state);
    actor.update(state);
    actor.render(renderer);

    portal.render(renderer);

    if (may::colliding(actor.bounding_box(), portal.bounding_box()))
    {
      actor.position(0, 0);
    }

    SDL_Point mpos = state.mouse_pos();
    int size = 75;
    SDL_Rect box{mpos.x - size / 2, mpos.y - size / 2, size, size};
    SDL_RenderDrawRect(renderer, &box);

    if (state.is_button_pressed(SDL_BUTTON_LEFT))
    {
      SDL_SetRenderDrawColor(renderer, 0xFF, 0x88, 0x88, 0xFF);
      SDL_RenderFillRect(renderer, &box);
    }
    else if (state.is_button_pressed(SDL_BUTTON_RIGHT))
    {
      SDL_SetRenderDrawColor(renderer, 0x88, 0x44, 0x44, 0xFF);
      SDL_RenderFillRect(renderer, &box);
    }
  }
};

int main(int argc, char **argv)
{
  // may::cfg_reader config("test.cfg");
  //  may::app &app = config.app();
  testapp app;
  app.start();
  return 0;
}