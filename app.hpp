#ifndef APP_HPP_
#define APP_HPP_
#include "window.hpp"
#include "image.hpp"
#include "actor.hpp"
#include "common.hpp"

#include <map>
#include <vector>

namespace may
{
  class app
  {
    may::window _window;
    may::game_state _game_state;
    uint8_t _bgr, _bgg, _bgb, _bga;
    image _bg_img;
    bool process_events();

  public:
    app();
    app(const char *title);
    app(const char *title, int width, int height, int x = SDL_WINDOWPOS_CENTERED, int y = SDL_WINDOWPOS_CENTERED);
    virtual ~app();

    may::window &window();
    void background_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
    void background_color(uint8_t *r, uint8_t *g, uint8_t *b, uint8_t *a) const;

    void background_image(const char *path);
    image& background_image();
    
    virtual void init();
    virtual void start();
    virtual void game_loop(SDL_Renderer *renderer);
    virtual void on_event(SDL_Event &event);

    inline may::game_state &game_state() { return _game_state; }
    inline void add_actor(may::actor &actor) { _game_state.add_actor(actor); }

    void clamp_on_border(may::actor& actor);
    void loop_on_border(may::actor& actor);
  };
}

#endif /* APP_HPP_ */