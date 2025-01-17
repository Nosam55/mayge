#include "app.hpp"
#include "asteroids_app.hpp"
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
      _hiragana.front().color({0x00, 0xCC, 0x00, 0xFF});

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
  may::gtext my_text;
  may::font my_font;
  may::audio_device audio_device;
  std::vector<float> _waveform;
  wavedata_t _data;
  may::window _control_panel;
  may::button _sin_button, _saw_button, _tri_button, _sq_button;
  may::button _pure_button, _major_button, _minor_button;

public:
  testapp() : testapp("Title") {}
  testapp(const char *title) : testapp(title, 800, 600) {}
  testapp(const char *title, int w, int h) : may::app(title, w, h), _cursor(nullptr), my_font("fonts/PerfectDOSVGA437.ttf", 24)
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
    // SDL_SetWindowBordered(window().window_ptr(), SDL_FALSE);

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

    int aud = SDL_GetNumAudioDrivers();

    for (int i = 0; i < aud; ++i)
    {
      printf("audio driver %d: '%s'\n", i, SDL_GetAudioDriver(i));
    }

    aud = SDL_GetNumAudioDevices(0);
    for (int i = 0; i < aud; ++i)
    {
      printf("audio device %d: '%s'\n", i, SDL_GetAudioDeviceName(i, 0));
    }

    audio_device.open(AUDIO_F32, 48000, 1, 1024, audio_callback, &_data);

    printf("obtained audio device '%s'\n", audio_device.name().c_str());
    printf("device id: %d\n", audio_device.id());
    printf("device freq: %d\n", audio_device.freq());
    printf("device channels: %d\n", audio_device.channels());
    printf("device frame buffer: %d\n", audio_device.framebuffer_size());

    _control_panel = may::window("Control Panel", 400, 200);
    _control_panel.load();

    SDL_Color off_white = {0xEE, 0xEE, 0xEE, 0xAA};
    _sq_button = may::button(off_white, 0, 0, 100, 50);
    _saw_button = may::button(off_white, 0, 50, 100, 50);
    _sin_button = may::button(off_white, 0, 100, 100, 50);
    _tri_button = may::button(off_white, 0, 150, 100, 50);
    _pure_button = may::button(off_white, 100, 0, 100, 200.0 / 3);
    _major_button = may::button(off_white, 100, 200.0 / 3, 100, 200.0 / 3);
    _minor_button = may::button(off_white, 100, 400.0 / 3, 100, 200.0 / 3);

    _sq_button.text().font(my_font.ttf_font());
    _saw_button.text().font(my_font.ttf_font());
    _sin_button.text().font(my_font.ttf_font());
    _tri_button.text().font(my_font.ttf_font());
    _pure_button.text().font(my_font.ttf_font());
    _major_button.text().font(my_font.ttf_font());
    _minor_button.text().font(my_font.ttf_font());

    _sq_button.fg_color(0x00, 0x00, 0x00, 0xFF);
    _saw_button.fg_color(0x00, 0x00, 0x00, 0xFF);
    _sin_button.fg_color(0x00, 0x00, 0x00, 0xFF);
    _tri_button.fg_color(0x00, 0x00, 0x00, 0xFF);
    _pure_button.fg_color(0x00, 0x00, 0x00, 0xFF);
    _major_button.fg_color(0x00, 0x00, 0x00, 0xFF);
    _minor_button.fg_color(0x00, 0x00, 0x00, 0xFF);

    _sq_button.text().text("Square");
    _saw_button.text().text("Sawtooth");
    _sin_button.text().text("Sine");
    _tri_button.text().text("Triangle");
    _pure_button.text().text("Pure");
    _major_button.text().text("Major");
    _minor_button.text().text("Minor");
  }

  void on_event(SDL_Event &event) override
  {
    if (event.type == SDL_WINDOWEVENT)
    {
      if (event.window.event == SDL_WINDOWEVENT_RESIZED)
      {
        int new_width = event.window.data1;
        int new_height = event.window.data2;

        window().fetch_rect();
      }
      else if (event.window.event == SDL_WINDOWEVENT_CLOSE)
      {
        if (event.window.windowID == _control_panel.id())
        {
          _control_panel.destroy();
        }
        else
        {
          stop();
        }
      }
    }
  }

  void game_loop(SDL_Renderer *renderer) override
  {
    auto &state = game_state();

    for (clicker *button : _buttons)
    {
      _sq_button.update(state);
      _saw_button.update(state);
      _sin_button.update(state);
      _tri_button.update(state);
      _pure_button.update(state);
      _major_button.update(state);
      _minor_button.update(state);

      if (_sq_button.is_clicked() && !_sq_button.is_held())
      {
        SDL_LockAudioDevice(audio_device.id());

        select_oscillator<may::square_osc>(_data, _data.oscillator->frequency());

        SDL_UnlockAudioDevice(audio_device.id());
      }
      else if (_saw_button.is_clicked() && !_saw_button.is_held())
      {
        SDL_LockAudioDevice(audio_device.id());

        select_oscillator<may::sawtooth_osc>(_data, _data.oscillator->frequency());

        SDL_UnlockAudioDevice(audio_device.id());
      }
      else if (_sin_button.is_clicked() && !_sin_button.is_held())
      {
        SDL_LockAudioDevice(audio_device.id());

        select_oscillator<may::sine_osc>(_data, _data.oscillator->frequency());

        SDL_UnlockAudioDevice(audio_device.id());
      }
      else if (_tri_button.is_clicked() && !_tri_button.is_held())
      {
        SDL_LockAudioDevice(audio_device.id());

        select_oscillator<may::triangle_osc>(_data, _data.oscillator->frequency());

        SDL_UnlockAudioDevice(audio_device.id());
      } else if (_pure_button.is_clicked() && !_pure_button.is_held())
      {
        SDL_LockAudioDevice(audio_device.id());

        _data.tones.clear();
        _data.tones.push_back(1.0);

        SDL_UnlockAudioDevice(audio_device.id());
      } else if (_major_button.is_clicked() && !_major_button.is_held())
      {
        SDL_LockAudioDevice(audio_device.id());

        _data.tones.clear();
        _data.tones.push_back(SDL_pow(2.0, 4.0 / 12.0));
        _data.tones.push_back(SDL_pow(2.0, 7.0 / 12.0));
        _data.tones.push_back(2.0);

        SDL_UnlockAudioDevice(audio_device.id());
      } else if (_minor_button.is_clicked() && !_minor_button.is_held())
      {
        SDL_LockAudioDevice(audio_device.id());

        _data.tones.clear();
        _data.tones.push_back(SDL_pow(2.0, 3.0 / 12.0));
        _data.tones.push_back(SDL_pow(2.0, 7.0 / 12.0));
        _data.tones.push_back(2.0);

        SDL_UnlockAudioDevice(audio_device.id());
      }
    }

    // Render control panel
    SDL_Renderer *renderer_cp = _control_panel.accelerated_renderer();
    SDL_RenderClear(renderer_cp);

    _sq_button.center_text();
    _saw_button.center_text();
    _sin_button.center_text();
    _tri_button.center_text();
    _pure_button.center_text();
    _major_button.center_text();
    _minor_button.center_text();

    _sq_button.render(renderer_cp);
    _saw_button.render(renderer_cp);
    _sin_button.render(renderer_cp);
    _tri_button.render(renderer_cp);
    _pure_button.render(renderer_cp);
    _major_button.render(renderer_cp);
    _minor_button.render(renderer_cp);

    SDL_RenderPresent(renderer_cp);

    /* ********************* PROCESS AND RENDER MAIN SCREEN ********************* */
    if (window().flags() & SDL_WINDOW_MOUSE_FOCUS)
    {
      // Handle playing audio on clicks
      if (state.is_button_set(SDL_BUTTON_LEFT))
      {
        audio_device.play();
        state.button_unset(SDL_BUTTON_LEFT);
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

class image_viewer : public may::app
{
  may::pane _filename_view;
  may::gtext _help_text, _error_text;
  may::button _button;
  may::font _font;
  std::list<may::window> _windows;

public:
  image_viewer() : app("Image Viewer", 400, 400, 100, 100), _font("fonts/PerfectDOSVGA437.ttf", 16)
  {
    background_color(0xAA, 0xAA, 0xAA, 0xFF);
  }

  void init() override
  {
    app::init();

    _help_text.text("Enter a path to an image file:");
    _help_text.font(_font.ttf_font());
    _help_text.position(width() / 8 + 2, height() / 3 - _font.pt_size() - 4);

    _error_text.text("");
    _error_text.font(_font.ttf_font());
    _error_text.position(width() / 8 + 2, height() / 3 + _font.pt_size() + 6);
    _error_text.color({0xAA, 0x00, 0x00, 0xFF});

    _filename_view.position(width() / 8, height() / 3);
    _filename_view.width(3 * width() / 4);
    _filename_view.height(_font.pt_size() + 4);

    _filename_view.bg_color(0xFF, 0xFF, 0xFF, 0xFF);
    _filename_view.fg_color(0x11, 0x11, 0x11, 0xFF);

    _filename_view.text().font(_font.ttf_font());
    _filename_view.text().text("some_file.pdf...");

    _filename_view.center_text();
    _filename_view.text().position(_filename_view.position().x + 2, _filename_view.position().y);
    _filename_view.text().wrap_width(_filename_view.width() - 4);

    _button.position(width() / 6, 2 * height() / 3);
    _button.width(2 * width() / 3);
    _button.height(50);

    _button.bg_color(0x77, 0x77, 0x77, 0xFF);
    _button.fg_color(0x11, 0x11, 0x11, 0xFF);

    _button.text().text("View...");
    _button.center_text();
  }

  ~image_viewer()
  {
    for (may::window &win : _windows)
    {
      win.destroy();
    }
  }

  void on_event(SDL_Event &event)
  {
    if (event.type == SDL_WINDOWEVENT)
    {
      if (event.window.event == SDL_WINDOWEVENT_CLOSE)
      {
        SDL_Window *win_ptr = SDL_GetWindowFromID(event.window.windowID);

        auto win_it = _windows.begin();
        while (win_it != _windows.end())
        {
          if (win_it->window_ptr() == win_ptr)
          {
            win_it->destroy();
            win_it = _windows.erase(win_it);
          }
          else
          {
            ++win_it;
          }
        }
      }
    }
  }

  void game_loop(SDL_Renderer *renderer) override
  {
    auto &state = game_state();

    _help_text.render(renderer);
    _error_text.render(renderer);

    _filename_view.update(state);
    _filename_view.render(renderer);

    _button.update(state);
    _button.render(renderer);

    if (state.edited())
    {
      _filename_view.text().text(state.composition() + '|');
    }

    if (_button.is_clicked() && !_button.is_held() || state.is_key_set(SDLK_RETURN))
    {
      const char *filename = state.composition().c_str();

      FILE *test = fopen(filename, "r");
      if (test == nullptr)
      {
        _error_text.text("Error: " + std::string(strerror(errno)));
        return;
      }

      may::image &img = may::image::get_image(filename);
      img.load_surface();

      may::window img_win(img.title(), img.surface()->w, img.surface()->h);
      img_win.load();

      SDL_FillRect(img_win.surface(), nullptr, 0x00000000);
      SDL_BlitSurface(img.surface(), nullptr, img_win.surface(), nullptr);

      img_win.update_surface();

      _windows.push_back(img_win);

      _error_text.text("");

      if (state.is_key_set(SDLK_RETURN))
      {
        state.key_unset(SDLK_RETURN);
      }
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