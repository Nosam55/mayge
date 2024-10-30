#ifndef WINDOW_HPP_
#define WINDOW_HPP_
#include <SDL2/SDL.h>
#include "sdl_exception.hpp"

namespace may
{
  class window
  {
    SDL_Window *_window_ptr;
    const char *_title;
    int _width;
    int _height;
    int _x;
    int _y;

  public:
    window();
    window(const window &that);
    window(const char *title, int width, int height, int x = SDL_WINDOWPOS_UNDEFINED, int y = SDL_WINDOWPOS_UNDEFINED);
    
    void load();
    void destroy();

    inline int width() const { return _width; }
    inline int height() const { return _height; }
    
    SDL_Window *window_ptr() const;
    SDL_Surface *surface() const;
    SDL_Renderer *renderer() const;
    SDL_Renderer *accelerated_renderer() const;

    const char *title() const;
    void title(const char *__title);
    void update_surface();

    // Operators
    window &operator=(const window &that);
  };
}

#endif /* WINDOW_HPP_ */