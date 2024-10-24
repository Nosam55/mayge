#ifndef APP_HPP_
#define APP_HPP_
#include "window.hpp"
#include "image.hpp"
#include "actor.hpp"
#include "common.hpp"

#include <map>

namespace may
{
  class app
  {
    may::window _window;
    may::image _shooter;
    may::game_state _game_state;
    bool process_events();
    uint8_t _bgr, _bgg, _bgb, _bga;
  public:
    app();
    ~app();
    may::window &window();
    virtual void main_loop();
    virtual void game_loop(SDL_Renderer *renderer);
    inline may::game_state &game_state() { return _game_state; }
    void background_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
  };
}

#endif /* APP_HPP_ */