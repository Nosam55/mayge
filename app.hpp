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
    app(int width, int height);
    app(const char *title, int width, int height);
    ~app();

    may::window &window();
    void background_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
    void background_color(uint8_t *r, uint8_t *g, uint8_t *b, uint8_t *a) const;

    void background_image(const char *path);
    image& background_image();
    
    virtual void start();
    virtual void game_loop(SDL_Renderer *renderer);
    
    inline may::game_state &game_state() { return _game_state; }
    inline int width() { return _window.width(); }
    inline int height() { return _window.height(); }
    void clamp_on_border(may::actor& actor);
    void loop_on_border(may::actor& actor);
  };
}

#endif /* APP_HPP_ */