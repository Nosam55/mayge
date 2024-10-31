#include "window.hpp"
#include <cstring>

namespace may
{
  window::window()
  {
    this->_window_ptr = nullptr;
  }

  window::window(const window &that)
  {
    this->_window_ptr = that._window_ptr;
  }

  window::window(const char *title, int width, int height, int x, int y) : _title(title)
  {
    this->_width = width;
    this->_height = height;
    this->_x = x;
    this->_y = y;
  }

  void window::load()
  {
    this->_window_ptr = SDL_CreateWindow(this->_title.c_str(), this->_x, this->_y, this->_width, this->_height, SDL_WINDOW_SHOWN);
    if (this->_window_ptr == nullptr)
    {
      throw sdl_exception();
    }
  }

  void window::destroy()
  {
    SDL_DestroyWindow(window_ptr());
  }

  void window::x(int __x)
  {
    this->_x = __x;
    if (_window_ptr)
    {
      SDL_SetWindowPosition(_window_ptr, _x, _y);
    }
  }

  void window::y(int __y)
  {
    this->_y = __y;
    if (_window_ptr)
    {
      SDL_SetWindowPosition(_window_ptr, _x, _y);
    }
  }

  void window::width(int width)
  {
    this->_width = width;
    if (_window_ptr)
    {
      SDL_SetWindowSize(_window_ptr, _width, _height);
    }
  }

  void window::height(int height)
  {
    this->_height = height;
    if (_window_ptr)
    {
      SDL_SetWindowSize(_window_ptr, _width, _height);
    }
  }

  void window::rect(SDL_Rect new_rect)
  {
    this->_x = new_rect.x;
    this->_y = new_rect.y;
    this->_width = new_rect.w;
    this->_height = new_rect.h;

    if (_window_ptr)
    {
      SDL_SetWindowPosition(_window_ptr, _x, _y);
      SDL_SetWindowSize(_window_ptr, _width, _height);
    }
  }

  void window::position(SDL_Point pos)
  {
    this->_x = pos.x;
    this->_y = pos.y;

    if (_window_ptr)
    {
      SDL_SetWindowPosition(_window_ptr, _x, _y);
    }
  }

  SDL_Window *window::window_ptr() const
  {
    return this->_window_ptr;
  }

  SDL_Surface *window::surface() const
  {
    return SDL_GetWindowSurface(window_ptr());
  }

  SDL_Renderer *window::renderer() const
  {
    return SDL_GetRenderer(window_ptr());
  }

  SDL_Renderer *window::accelerated_renderer() const
  {
    SDL_Renderer *renderer = SDL_CreateRenderer(window_ptr(), -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr)
    {
      renderer = this->renderer();
      if (renderer == nullptr)
      {
        throw sdl_exception();
      }
    }
    return renderer;
  }

  void window::title(const char *__title)
  {
    this->_title = __title;
    SDL_SetWindowTitle(this->_window_ptr, this->_title.c_str());
  }

  void window::update_surface()
  {
    SDL_UpdateWindowSurface(window_ptr());
  }

  window &window::operator=(const window &that)
  {
    this->_window_ptr = that._window_ptr;
    return *this;
  }
}