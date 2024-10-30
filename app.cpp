#include "app.hpp"

const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 900;

double shooter_x = 0.0, shooter_y = 0.0;
double v_x = 0.0, v_y = 0.0;
double speed = 175.0;
double angle = 0.0f;

namespace may
{
  app::app() : _window("My App", SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED)
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
  }

  app::~app()
  {
    for (may::actor &actor : _actors)
    {
      actor.destroy();
    }
    window().destroy();
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

    SDL_Event event;
    while (SDL_PollEvent(&event) != 0)
    {
      if (event.type == SDL_QUIT)
      {
        quit = true;
      }
      else if (event.type == SDL_KEYDOWN)
      {
        game_state().key_down(event.key.keysym.sym);
      }
      else if (event.type == SDL_KEYUP)
      {
        game_state().key_up(event.key.keysym.sym);
      }
    }

    return quit;
  }

  void app::start()
  {
    try
    {
      may::window &window = this->window();
      window.load();

      SDL_Surface *surface = window.surface();

      SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0x55, 0x00, 0x55));
      window.update_surface();

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

        game_loop(renderer);

        SDL_RenderPresent(renderer);

        game_state().tick(SDL_GetTicks64());
      }
    }
    catch (may::sdl_exception ex)
    {
      fprintf(stderr, "Caught exception: %s\n", ex.what());
    }
  }

  void app::game_loop(SDL_Renderer *renderer)
  {
    static may::floating_actor player("shooter.png", 100.0, 100.0, 50, 40, 0.0);

    player.input(game_state());
    player.update(game_state());

    // Window border checking
    SDL_FPoint player_pos = player.position();
    player_pos.x = fmodf64(player_pos.x, (double)SCREEN_WIDTH);
    player_pos.y = fmodf64(player_pos.y, (double)SCREEN_HEIGHT);
    if (player_pos.x < 0)
    {
      player_pos.x = (double)SCREEN_WIDTH - 5.0;
    }
    if (player_pos.y < 0)
    {
      player_pos.y = (double)SCREEN_HEIGHT - 5.0;
    }
    player.position(player_pos);
    ////////////////////////

    SDL_Rect fillRect = {
        static_cast<int>(SCREEN_WIDTH / 4),
        static_cast<int>(SCREEN_HEIGHT / 4),
        static_cast<int>(SCREEN_WIDTH / 2),
        static_cast<int>(SCREEN_HEIGHT / 2)};

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
  }

  void app::background_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
  {
    _bgr = r;
    _bgg = g;
    _bgb = b;
    _bga = a;
  }
}