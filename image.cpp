#include "image.hpp"

namespace may
{
  std::map<std::string, image> image::_unique_images;

  image::image() : _path("")
  {
    this->_surface = nullptr;
    this->_texture = nullptr;
    this->_renderer = nullptr;
  }

  image::image(const char *path) : _path(path)
  {
    this->_surface = nullptr;
    this->_texture = nullptr;
    this->_renderer = nullptr;
  }

  image::~image()
  {
    unload();
  }

  image &image::get_image(const char *path)
  {
    if (_unique_images.count(path) > 0)
    {
      return _unique_images.at(path);
    }
    else
    {
      _unique_images[path] = image(path);
      return _unique_images[path];
    }
  }

  SDL_Surface *image::load_surface()
  {
    if (_surface)
    {
      return _surface;
    }
    else
    {
      _surface = IMG_Load(_path.c_str());
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
      if (_texture)
      {
        SDL_DestroyTexture(_texture);
      }

      _texture = IMG_LoadTexture(renderer, _path.c_str());
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
    return _path.c_str();
  }

  void image::unload()
  {
    bool is_ref_image = &(_unique_images.at(_path)) == this;

    _surface = nullptr;
    _texture = nullptr;

    if (is_ref_image)
    {
      SDL_FreeSurface(_surface);
      SDL_DestroyTexture(_texture);
    }
  }
}
