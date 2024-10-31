#ifndef WINDOW_HPP_
#define WINDOW_HPP_
#include <SDL2/SDL.h>
#include "sdl_exception.hpp"
#include <string>

namespace may
{
  class window
  {
    SDL_Window *_window_ptr;
    std::string _title;
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

    inline int x() const { return _x; }
    inline int y() const { return _y; }
    inline int width() const { return _width; }
    inline int height() const { return _height; }
    inline SDL_Rect rect() const { return {_x, _y, _width, _height}; }
    inline SDL_Point position() const { return {_x, _y}; }

    void x(int __x);
    void y(int __y);
    void width(int width);
    void height(int height);
    void rect(SDL_Rect new_rect);
    void position(SDL_Point pos);

    SDL_Window *window_ptr() const;
    SDL_Surface *surface() const;
    SDL_Renderer *renderer() const;
    SDL_Renderer *accelerated_renderer() const;

    inline const char *title() const { return _title.c_str(); }
    void title(const char *__title);
    void update_surface();

    // Operators
    window &operator=(const window &that);
  };
}

#endif /* WINDOW_HPP_ */