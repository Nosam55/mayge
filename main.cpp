#include "app.hpp"
#include "asteroids_app.hpp"
#include "cfg_reader.hpp"
#include "gui.hpp"

#include <ctime>

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

class testapp;

class clicker : virtual public may::button
{
  int _res_w, _res_h;

public:
  clicker(int res_w, int res_h, SDL_Color color, double x, double y, int width, int height) : may::actor(x, y, 0, 0, 0), may::pane(color, x, y, width, height), may::button(color, x, y, width, height)
  {
    _res_w = res_w;
    _res_h = res_h;
    _text.text(std::to_string(_res_w) + "x" + std::to_string(_res_h));
    center_text();
  }

  inline SDL_Point res() const { return {_res_w, _res_h}; }
  inline void res(int w, int h)
  {
    _res_w = w;
    _res_h = h;
  }

  virtual void on_click(may::game_state &state) override;
};

class testapp : public may::app
{
  SDL_Cursor *_cursor;
  uint64_t _timer;

  may::font my_font;
  may::gtext my_text;
  may::pane menu_pane;
  std::vector<clicker *> _buttons;

public:
  testapp() : testapp("Title") {}
  testapp(const char *title) : testapp(title, 800, 600) {}
  testapp(const char *title, int w, int h) : may::app(title, w, h), _cursor(nullptr), my_font("fonts/PerfectDOSVGA437.ttf", 24),
                                             menu_pane(SDL_Color{0, 0, 0, 0x3F}, 100, 2 * h / 3, 600, h / 3 - 10)
  {
    background_color(0x22, 0x22, 0x22, 0xFF);
    _timer = 0;
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
    game_state().add_thing(this);
    int err = SDL_SetWindowFullscreen(window().window_ptr(), 0);
    if (err)
    {
      fprintf(stderr, "Error going fullscreen: %s\n", SDL_GetError());
      exit(EXIT_FAILURE);
    }

    _cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_CROSSHAIR);
    SDL_SetCursor(_cursor);

    my_text.font(my_font.ttf_font());
    my_text.color({0xFF, 0xFF, 0xFF, 0xFF});

    _buttons.push_back(new clicker(800, 600, SDL_Color{0, 0, 0xAA, 0xAA}, 100, 100, 100, 100));
    _buttons.push_back(new clicker(1200, 900, SDL_Color{0xAA, 0, 0, 0xAA}, 225, 100, 100, 100));

    SDL_SetRenderDrawBlendMode(window().renderer(), SDL_BLENDMODE_BLEND);
  }

  void game_loop(SDL_Renderer *renderer) override
  {
    auto &state = game_state();

    menu_pane.render(renderer);

    for (clicker *button : _buttons)
    {
      button->update(state);
      button->render(renderer);
    }

    if (state.mouse_moved())
    {
      std::string postxt("(");
      postxt += std::to_string(state.mouse_pos().x);
      postxt += ", ";
      postxt += std::to_string(state.mouse_pos().y);
      postxt += ")";

      my_text.text(postxt);

      SDL_Point mpos = state.mouse_pos();

      mpos.x = may::clamp(0, mpos.x, width() - my_text.width());
      mpos.y = may::clamp(0, mpos.y, height() - my_text.height());

      my_text.position(mpos);
    }

    my_text.render(renderer);
  }
};

void clicker::on_click(may::game_state &state)
{
  button::on_click(state);
  testapp &app = state.add_thing<testapp>(nullptr);

  SDL_Rect win{0, 0, _res_w, _res_h};
  SDL_GetWindowPosition(app.window().window_ptr(), &win.x, &win.y);
  printf("%+d%+d %dx%d\n", win.x, win.y, win.w, win.h);

  app.window().rect(win);
}

int main(int argc, char **argv)
{
  // may::cfg_reader config("test.cfg");
  //  may::app &app = config.app();
  testapp app;
  app.start();
  return 0;
}