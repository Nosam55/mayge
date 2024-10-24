#include "image.hpp"

namespace may
{
  image::image(const char *path)
  {
    this->_path = path;
    this->_surface = nullptr;
    this->_texture = nullptr;
    this->_renderer = nullptr;
  }

  SDL_Surface *image::load_surface()
  {
    if (_surface)
    {
      return _surface;
    }
    else
    {
      _surface = IMG_Load(_path);
      if (_surface == nullptr)
      {
        fprintf(stderr, "Unable to load image %s: %s\n", _path, IMG_GetError());
        exit(EXIT_FAILURE);
      }

      return _surface;
    }
  }

  SDL_Texture *image::load_texture(SDL_Renderer *renderer)
  {
    if (_texture && _renderer == renderer)
    {
      return _texture;
    }
    else
    {
      if(_texture)
      {
        SDL_DestroyTexture(_texture);
      }

      _texture = IMG_LoadTexture(renderer, _path);
      if (_texture == nullptr)
      {
        fprintf(stderr, "Could not load image texture %s: %s\n", _path, IMG_GetError());
        exit(EXIT_FAILURE);
      }

      _renderer = renderer;
      return _texture;
    }
  }

  SDL_Surface *image::surface() const
  {
    return _surface;
  }

  SDL_Texture *image::texture() const
  {
    return _texture;
  }

  const char *image::title() const
  {
    return _path;
  }

  void image::unload()
  {
    if (_surface)
    {
      SDL_FreeSurface(_surface);
      _surface = nullptr;
    }

    if (_texture)
    {
      SDL_DestroyTexture(_texture);
      _texture = nullptr;
    }
  }
}
