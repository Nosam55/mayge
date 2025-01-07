#include "app.hpp"
#include "asteroids_app.hpp"
#include "cfg_reader.hpp"
#include "gui.hpp"

#include <ctime>
#include <array>

class myactor : public may::animated_actor, public may::floating_actor
{

public:
  myactor() : may::actor(80, 80, 64, 64, 0, 64, M_PI), may::animated_actor(may::spritesheet("orb_sprites.png", 32, 32), 64, 64, 8.0)
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

    SDL_FPoint v_begin = center() + positionF();
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

class testapp : public may::app
{
  SDL_Cursor *_cursor;
  uint64_t _timer;

  may::font my_font;
  may::pane _menu;

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
    if (_cursor)
      SDL_FreeCursor(_cursor);
    _cursor = nullptr;

    _menu.destroy();
    my_font.unload();
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

    SDL_SetRenderDrawBlendMode(window().renderer(), SDL_BLENDMODE_BLEND);

    may::gtext menu_text("Menu Screen!", my_font.ttf_font());
    may::gtext play_text("Play!", my_font.ttf_font());

    printf("menu_text size (%.2fx%.2f)\n", menu_text.width(), menu_text.height());

    _menu.rect({300, 100, 200, 400});
    _menu.bg_color(0xFF, 0xFF, 0xFF, 0xCC);
    _menu.add(menu_text);

    may::button _play_button(150, 100);
    _play_button.bg_color(0xCC, 0x00, 0xCC, 0xCC);
    _play_button.add(play_text);

    _menu.add(_play_button);

    _menu.layout();
  }

  void on_event(SDL_Event &event) override
  {
    if (event.type == SDL_WINDOWEVENT)
    {
      if (event.window.event == SDL_WINDOWEVENT_RESIZED)
      {
        int new_width = event.window.data1;
        int new_height = event.window.data2;

        _menu.layout();

        printf("resize\n");
      }
    }
  }

  void game_loop(SDL_Renderer *renderer) override
  {
    auto &state = game_state();

    _menu.update(state);
    _menu.render(renderer);

    // Scanlines //
    ////////////////////////////////////////////////
    SDL_Color bg;
    background_color(&bg.r, &bg.g, &bg.b, &bg.a);

    SDL_SetRenderDrawColor(renderer, bg.r, bg.g, bg.b, 0x55);
    for (int i = 0; i < window().height(); i += 2)
    {
      SDL_RenderDrawLine(renderer, 0, i, window().width(), i);
    }
  }
};

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
    int width = window().width();
    int height = window().height();

    _help_text.text("Enter a path to an image file:");
    _help_text.font(_font.ttf_font());
    _help_text.position(width / 8 + 2, height / 3 - _font.pt_size() - 4);

    _error_text.text("");
    _error_text.font(_font.ttf_font());
    _error_text.position(width / 8 + 2, height / 3 + _font.pt_size() + 6);
    _error_text.color({0xAA, 0x00, 0x00, 0xFF});

    _filename_view.position(width / 8, height / 3);
    _filename_view.width(3 * width / 4);
    _filename_view.height(_font.pt_size() + 4);

    _filename_view.bg_color(0xFF, 0xFF, 0xFF, 0xFF);
    _filename_view.fg_color(0x11, 0x11, 0x11, 0xFF);

    may::gtext text("some_file.jpg...", _font.ttf_font());
    _filename_view.add(text);

    _button.position(width / 6, 2 * height / 3);
    _button.width(2 * width / 3);
    _button.height(50);

    _button.bg_color(0x77, 0x77, 0x77, 0xFF);
    _button.fg_color(0x11, 0x11, 0x11, 0xFF);

    text.text("View...");
    _button.add(text);
  }

  ~image_viewer()
  {
    for (may::window &win : _windows)
    {
      win.destroy();
    }
    _font.unload();
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
      dynamic_cast<may::gtext*>(_filename_view.get()[0])->text(state.composition() + '|');
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
  // may::app &app = config.app();
  testapp app;
  app.start();
  return 0;
}