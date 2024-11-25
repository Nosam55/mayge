#include "app.hpp"
#include "asteroids_app.hpp"
#include "cfg_reader.hpp"
#include "gui.hpp"

class myactor : public may::animated_actor, public may::floating_actor
{

public:
  myactor() : may::actor(80, 80, 0, 64, M_PI), may::animated_actor(may::spritesheet("orb_sprites.png", 32, 32), 64, 64, 8.0)
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

class clicker : virtual public may::button
{
  int _increment;

public:
  clicker(int increment, SDL_Color color, double x, double y, int width, int height) : may::actor(x, y, 0, 0, 0), may::pane(width, height), may::button(color, x, y, width, height)
  {
    this->_increment = increment;
  }
  
  virtual void on_click(may::game_state &state) override
  {
    SDL_Color bg = _bg_color;

    bg.r += _increment;
    if (bg.r <= _bg_color.r)
    {
      bg.g += _increment;
      if (bg.g <= _bg_color.g)
      {
        bg.b += _increment;
      }
    }

    bg_color(bg.r, bg.g, bg.b, bg.a);
    button::on_click(state);
  }

  virtual void render(SDL_Renderer *renderer) override
  {
    int inc = 2;
    if (is_clicked())
    {
      position(_x + inc, _y + inc);
      _width -= 2 * inc;
      _height -= 2 * inc;

      pane::render(renderer);

      position(_x - inc, _y - inc);
      _width += 2 * inc;
      _height += 2 * inc;
    }
    else if (is_hovered())
    {
      position(_x - inc / 2, _y - inc / 2);
      _width += inc;
      _height += inc;

      pane::render(renderer);

      position(_x + inc / 2, _y + inc / 2);
      _width -= inc;
      _height -= inc;
    }
    else
    {
      pane::render(renderer);
    }
  }
};

class testapp : public may::app
{
  SDL_Cursor *_cursor;
  clicker _clicker;

public:
  testapp() : testapp("Title") {}
  testapp(const char *title) : testapp(title, 800, 600) {}
  testapp(const char *title, int w, int h) : may::app(title, w, h), _cursor(nullptr), _clicker(8, SDL_Color{0x00, 0x00, 0x00, 0x88}, 0, 0, 200, 200)
  {
    background_color(0xEE, 0xEE, 0xEE, 0xFF);
    _clicker.position(width() / 2.0 - _clicker.width() / 2.0, height() / 2.0 - _clicker.width() / 2.0);
  }

  ~testapp()
  {
    if (_cursor)
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

    SDL_SetRenderDrawBlendMode(window().renderer(), SDL_BLENDMODE_BLEND);
  }

  void game_loop(SDL_Renderer *renderer) override
  {
    auto &state = game_state();

    // SDL_FPoint pos = actor.position();
    // actor.input(state);
    // actor.update(state);
    // actor.render(renderer);

    _clicker.update(state);
    _clicker.render(renderer);

    // this->loop_on_border(actor);
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