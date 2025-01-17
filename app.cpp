#include "app.hpp"
#include <SDL2/SDL_ttf.h>

namespace may
{
  app::app() : app("My App")
  {
  }

  app::app(const char *title) : app(title, 800, 600)
  {
  }

  app::app(const char *title, int width, int height, int x, int y) : _window(title, width, height, x, y), _bg_img()
  {
    background_color(0xFF, 0xFF, 0xFF, 0xFF);
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
    {
      fprintf(stderr, "Error initializing SDL: %s\n", SDL_GetError());
      exit(EXIT_FAILURE);
    }

    int image_settings = IMG_INIT_PNG | IMG_INIT_JPG | IMG_INIT_WEBP;
    if (!(IMG_Init(image_settings) & image_settings))
    {
      fprintf(stderr, "Error initializing SDL_image: %s\n", IMG_GetError());
      exit(EXIT_FAILURE);
    }

    if (TTF_Init() != 0)
    {
      fprintf(stderr, "Error initializing SDL_ttf: %s\n", TTF_GetError());
      exit(EXIT_FAILURE);
    }

    SDL_version compiled;
    SDL_version linked;
    SDL_version const *linked_ptr;

    SDL_VERSION(&compiled);
    SDL_GetVersion(&linked);

    printf("Compiled with SDL %d.%d.%d\n", compiled.major, compiled.minor, compiled.patch);
    printf("Currently running SDL %d.%d.%d\n", linked.major, linked.minor, linked.patch);

    SDL_IMAGE_VERSION(&compiled);
    linked_ptr = IMG_Linked_Version();

    printf("\nCompiled with SDL_image %d.%d.%d\n", compiled.major, compiled.minor, compiled.patch);
    printf("Currently running SDL_image %d.%d.%d\n", linked_ptr->major, linked_ptr->minor, linked_ptr->patch);

    SDL_TTF_VERSION(&compiled);
    linked_ptr = TTF_Linked_Version();

    printf("\nCompiled with SDL_ttf %d.%d.%d\n", compiled.major, compiled.minor, compiled.patch);
    printf("Currently running SDL_ttf %d.%d.%d\n", linked_ptr->major, linked_ptr->minor, linked_ptr->patch);

    _is_running = false;
  }

  app::~app()
  {
    window().destroy();
    image::unload_static();

    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
  }

  may::audio_device &app::audio_out()
  {
    return _audio_out;
  }

  window &app::window()
  {
    return this->_window;
  }

  bool app::process_events()
  {
    bool quit = false;
    auto &state = game_state();

    state.mouse_moved(false);
    state.edited(false);

    SDL_Event event;
    while (SDL_PollEvent(&event) != 0)
    {
      if (event.type == SDL_QUIT)
      {
        quit = true;
      }
      else if (event.type == SDL_KEYDOWN)
      {
        state.key_down(event.key.keysym.sym);
        if (event.key.keysym.sym == SDLK_BACKSPACE && state.composition().length() > 0)
        {
          state.composition().pop_back();
          state.edited(true);
        }
      }
      else if (event.type == SDL_KEYUP)
      {
        state.key_up(event.key.keysym.sym);
      }
      else if (event.type == SDL_MOUSEBUTTONDOWN)
      {
        state.button_down(event.button.button);
        state.mouse_pos(event.button.x, event.button.y);
      }
      else if (event.type == SDL_MOUSEBUTTONUP)
      {
        state.button_up(event.button.button);
        state.mouse_pos(event.button.x, event.button.y);
      }
      else if (event.type == SDL_MOUSEMOTION)
      {
        state.mouse_pos(event.motion.x, event.motion.y);
        state.mouse_moved(true);
      }
      else if (event.type == SDL_MOUSEWHEEL)
      {
        state.scroll_x(event.wheel.preciseX);
        state.scroll_y(event.wheel.preciseY);
      }
      else if (event.type == SDL_TEXTINPUT)
      {
        state.composition() += event.text.text;
        state.edited(true);
      }
      else if (event.type == SDL_TEXTEDITING)
      {
        std::string &comp = state.composition();
        state.composition(comp.substr(0, event.edit.start) + event.edit.text + comp.substr(event.edit.start + event.edit.length));
        state.edited(true);
      }

      on_event(event);
    }

    return quit;
  }

  void app::init()
  {
    may::window &window = this->window();

    window.load();

    SDL_Surface *surface = window.surface();

    SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0x55, 0x00, 0x55));

    may::image &logo = image::get_image("mayge-logo.png");
    SDL_Surface *logo_image = logo.load_surface();

    int logo_size = 64;
    if (surface->w > logo_size && surface->h > logo_size)
    {
      logo_size <<= 1;
      while (surface->w > logo_size && surface->h > logo_size)
      {
        logo_size <<= 1;
      }

      logo_size >>= 2;
    }

    SDL_Rect logo_rect = {
      x : surface->w / 2 - logo_size / 2,
      y : surface->h / 2 - logo_size / 2,
      w : logo_size,
      h : logo_size
    };

    SDL_BlitScaled(logo_image, nullptr, surface, &logo_rect);

    window.update_surface();
  }

  void app::start()
  {
    try
    {
      init();

      may::window &window = this->window();

      SDL_Renderer *renderer = window.accelerated_renderer();

      game_state().tick(SDL_GetTicks64());
      game_state().tick(SDL_GetTicks64());
      
      _is_running = true;
      while(_is_running)
      {
        bool quit = process_events();
        if (game_state().is_key_pressed(SDLK_ESCAPE))
        {
          _is_running = false;
        }

        if (!_is_running || quit)
        {
          break;
        }

        SDL_SetRenderDrawColor(renderer, _bgr, _bgg, _bgb, _bga);
        SDL_RenderClear(renderer);

        if (strcmp(_bg_img.title(), "") != 0)
        {
          SDL_RenderCopy(renderer, _bg_img.load_texture(renderer), NULL, NULL);
        }

        game_loop(renderer);

        SDL_RenderPresent(renderer);

        game_state().tick(SDL_GetTicks64());
      }
    }
    catch (may::sdl_exception &ex)
    {
      fprintf(stderr, "Caught exception: %s\n", ex.what());
    }
  }

  void app::stop()
  {
    _is_running = false;
  }

  void app::game_loop(SDL_Renderer *renderer)
  {
  }

  void app::on_event(SDL_Event &event)
  {
  }

  void app::background_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
  {
    _bgr = r;
    _bgg = g;
    _bgb = b;
    _bga = a;
  }

  void app::background_color(uint8_t *r, uint8_t *g, uint8_t *b, uint8_t *a) const
  {
    *r = _bgr;
    *g = _bgg;
    *b = _bgb;
    *a = _bga;
  }

  void app::background_image(const char *path)
  {
    _bg_img = image::get_image(path);
  }

  image &app::background_image()
  {
    return _bg_img;
  }

  void app::clamp_on_border(may::actor &actor)
  {
    SDL_FPoint position = actor.position();

    if (position.x < 0)
    {
      position.x = 0;
    }
    else if (position.x > width())
    {
      position.x = width();
    }

    if (position.y < 0)
    {
      position.y = 0;
    }
    else if (position.y > height())
    {
      position.y = height();
    }

    actor.position(position);
  }

  void app::loop_on_border(may::actor &actor)
  {
    SDL_FPoint position = actor.position();

    position.x = fmodf(position.x, (double)width());
    position.y = fmodf(position.y, (double)height());

    if (position.x < 0)
    {
      position.x = (double)width() - 5.0;
    }
    if (position.y < 0)
    {
      position.y = (double)height() - 5.0;
    }

    actor.position(position);
  }
}