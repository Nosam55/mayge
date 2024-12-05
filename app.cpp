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
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
      fprintf(stderr, "Error initializing SDL: %s\n", SDL_GetError());
      exit(EXIT_FAILURE);
    }

    int image_settings = IMG_INIT_PNG;
    if (!(IMG_Init(image_settings) & image_settings))
    {
      fprintf(stderr, "Error initializing SDL_image: %s\n", IMG_GetError());
      exit(EXIT_FAILURE);
    }

    if (TTF_Init() != 0)
    {
      fprintf(stderr, "Error initializing SDL_TTF: %s\n", TTF_GetError());
      exit(EXIT_FAILURE);
    }
  }

  app::~app()
  {
    window().destroy();
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
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
    }

    return quit;
  }

  void app::init()
  {
    may::window &window = this->window();

    window.load();

    SDL_Surface *surface = window.surface();

    SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0x55, 0x00, 0x55));
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
      for (bool quit = false; !quit; quit)
      {
        quit = process_events();
        if (game_state().is_key_pressed(SDLK_ESCAPE))
        {
          quit = true;
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

  void app::game_loop(SDL_Renderer *renderer)
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

    position.x = fmodf64(position.x, (double)width());
    position.y = fmodf64(position.y, (double)height());

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