#include "app.hpp"
#include "asteroids_app.hpp"
#include "cfg_reader.hpp"
#include "gui.hpp"

#include <ctime>
#include <array>

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

class faller
{
  static const char *random_hira()
  {
    static std::array<const char *, 0x97 - 0x41> glyphs{"\u3041", "\u3042", "\u3043", "\u3044", "\u3045", "\u3046", "\u3047", "\u3048", "\u3049", "\u304a", "\u304b", "\u304c", "\u304d", "\u304e", "\u304f", "\u3050", "\u3051", "\u3052", "\u3053", "\u3054", "\u3055", "\u3056", "\u3057", "\u3058", "\u3059", "\u305a", "\u305b", "\u305c", "\u305d", "\u305e", "\u305f", "\u3060", "\u3061", "\u3062", "\u3063", "\u3064", "\u3065", "\u3066", "\u3067", "\u3068", "\u3069", "\u306a", "\u306b", "\u306c", "\u306d", "\u306e", "\u306f", "\u3070", "\u3071", "\u3072", "\u3073", "\u3074", "\u3075", "\u3076", "\u3077", "\u3078", "\u3079", "\u307a", "\u307b", "\u307c", "\u307d", "\u307e", "\u307f", "\u3080", "\u3081", "\u3082", "\u3083", "\u3084", "\u3085", "\u3086", "\u3087", "\u3088", "\u3089", "\u308a", "\u308b", "\u308c", "\u308d", "\u308e", "\u308f", "\u3090", "\u3091", "\u3092", "\u3093", "\u3094", "\u3095", "\u3096"};
    const char *glyph = glyphs[rand() % glyphs.size()];

    return glyph;
  }

  std::list<may::gtext> _hiragana;

  double _x, _y;
  may::font *_font;
  uint64_t _speed;
  uint64_t _timer;

public:
  faller(double x, double y, may::font &font) : _font(&font)
  {
    _x = x;
    _y = y;
    _timer = 0;
    _speed = rand() % 100 + 50;
  }

  double top() const
  {
    if (_hiragana.size() > 0)
    {
      return _hiragana.back().position().y;
    }
    else
    {
      return _y;
    }
  }

  void update(may::game_state &state)
  {
    if (_hiragana.size() == 0)
    {
      _hiragana.emplace_front(random_hira(), _font->ttf_font(), _x, _y);
      _hiragana.front().color({0x33, 0xFF, 0x33, 0xEE});

      _timer = state.tick() + _speed;
    }
    else if (state.tick() > _timer)
    {
      _y += _hiragana.back().height();

      _hiragana.emplace_front(random_hira(), _font->ttf_font(), _x, _y);
      _hiragana.front().color({0x00, 0xCC, 0x00, 0xFF});

      _timer = state.tick() + _speed;

      if (_hiragana.size() >= 35)
      {
        _hiragana.pop_back();
      }
    }
  }

  void render(SDL_Renderer *renderer)
  {
    for (may::gtext &text : _hiragana)
    {
      text.render(renderer);
    }
  }
};

class testapp : public may::app
{
  SDL_Cursor *_cursor;
  uint64_t _timer;

  std::map<int, may::font> font_map;
  std::vector<clicker *> _buttons;
  std::vector<faller> _fallers;

public:
  testapp() : testapp("Title") {}
  testapp(const char *title) : testapp(title, 800, 600) {}
  testapp(const char *title, int w, int h) : may::app(title, w, h), _cursor(nullptr)
  {
    background_color(0x11, 0x11, 0x11, 0xFF);
    _timer = 0;
  }

  virtual ~testapp()
  {
    for (clicker *c : _buttons)
    {
      delete c;
    }

    if (_cursor)
      SDL_FreeCursor(_cursor);
    _cursor = nullptr;

    for (auto &pair : font_map)
    {
      pair.second.unload();
    }
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

    SDL_SetWindowResizable(window().window_ptr(), SDL_TRUE);

    // err = SDL_SetWindowOpacity(window().window_ptr(), 0.8);
    // if(err){
    //   fprintf(stderr, "cannot set window opacity: %s\n", SDL_GetError());
    // }

    _cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_CROSSHAIR);
    SDL_SetCursor(_cursor);

    _buttons.push_back(new clicker(1920 >> 2, 1080 >> 2, SDL_Color{0, 0, 0xAA, 0xAA}, 100, 100, 100, 100));
    _buttons.push_back(new clicker(1920 >> 1, 1080 >> 1, SDL_Color{0xAA, 0, 0, 0xAA}, 225, 100, 100, 100));
    _buttons.push_back(new clicker(1920, 1080, SDL_Color{0xAA, 0, 0xAA, 0xAA}, 350, 100, 100, 100));

    SDL_SetRenderDrawBlendMode(window().renderer(), SDL_BLENDMODE_BLEND);
  }

  void game_loop(SDL_Renderer *renderer) override
  {
    auto &state = game_state();
    window().fetch_rect();

    for (clicker *button : _buttons)
    {
      button->update(state);
      button->render(renderer);
    }

    if (state.is_button_pressed(SDL_BUTTON_LEFT) && state.tick() > _timer)
    {
      int font_size = rand() % 25 + 4;

      if (font_map.count(font_size) == 0)
      {
        font_map.emplace(font_size, may::font("fonts/Noto_Sans_JP/static/NotoSansJP-Regular.ttf", font_size));
      }

      _fallers.push_back(faller(state.mouse_posF().x, state.mouse_posF().y, font_map.at(font_size)));
      _timer = state.tick() + 50;
    }

    for (auto it = _fallers.begin(); it != _fallers.end(); ++it)
    {
      if (it->top() > height())
      {
        it = _fallers.erase(it) - 1;
      }
      else
      {
        it->update(state);
        it->render(renderer);
      }
    }

    SDL_Color bg;
    background_color(&bg.r, &bg.g, &bg.b, &bg.a);

    SDL_SetRenderDrawColor(renderer, bg.r, bg.g, bg.b, 0x55);
    for (int i = 0; i < height(); i += 2)
    {
      SDL_RenderDrawLine(renderer, 0, i, width(), i);
    }
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