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

  public:
    app();
    ~app();
    may::window &window();
    virtual void main_loop();
    inline may::game_state &game_state() { return _game_state; }
  };
}

#endif /* APP_HPP_ */