#include "image.hpp"
#include <stdexcept>

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
      if (path == nullptr)
      {
        _unique_images[path] = empty_image();
      }
      else
      {
        _unique_images[path] = image(path);
      }

      return _unique_images[path];
    }
  }

  image image::empty_image()
  {
    image null_img("MAY NULL IMAGE");
    null_img._surface = SDL_CreateRGBSurface(0, 1, 1, 32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);
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
        fprintf(stderr, "Unable to load image %s: %s\n", _path.c_str(), IMG_GetError());
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

      if (_surface)
      {
        _texture = SDL_CreateTextureFromSurface(renderer, _surface);
        if (_texture == nullptr)
        {
          fprintf(stderr, "Could not load image texture %s from existing surface: %s\n", _path.c_str(), SDL_GetError());
          exit(EXIT_FAILURE);
        }
      }
      else
      {
        _texture = IMG_LoadTexture(renderer, _path.c_str());
        if (_texture == nullptr)
        {
          fprintf(stderr, "Could not load image texture %s: %s\n", _path.c_str(), IMG_GetError());
          exit(EXIT_FAILURE);
        }
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

    if (is_ref_image)
    {
      SDL_FreeSurface(_surface);
      SDL_DestroyTexture(_texture);
    }

    _surface = nullptr;
    _texture = nullptr;
  }

  spritesheet::spritesheet() : spritesheet("", 0, 0)
  {
  }

  spritesheet::spritesheet(const char *path, int sprite_width, int sprite_height)
  {
    this->_sheet = image::get_image(path);
    this->_sprite_width = sprite_width;
    this->_sprite_height = sprite_height;
    this->_columns = 0;
    this->_rows = 0;
    this->_sheet_height = 0;
    this->_sheet_width = 0;
  }

  void spritesheet::load_sprites(SDL_Renderer *renderer)
  {
    SDL_Texture *sheet_texture = _sheet.load_texture(renderer);

    SDL_QueryTexture(sheet_texture, nullptr, nullptr, &_sheet_width, &_sheet_height);

    // Boundary Checking
    if (_sheet_width % _sprite_width != 0)
    {
      throw std::length_error("Spritesheet width is not aligned to sprite width");
    }

    if (_sheet_height % _sprite_height != 0)
    {
      throw std::length_error("Spritesheet height is not aligned to sprite height");
    }
    ////////////////////////

    this->_rows = _sheet_height / _sprite_height;
    this->_columns = _sheet_width / _sprite_width;

    _loaded = true;
  }

  spritesheet::sprite spritesheet::operator[](unsigned int index) const
  {
    SDL_Texture *sheet_texture = _sheet.texture();

    int x = (index % _columns) * _sprite_width;
    int y = (index / _columns) * _sprite_height;

    if (x > _sheet_width || x < 0)
    {
      throw std::out_of_range("Requested sprite was out of range of width of spritesheet");
    }

    if (y > _sheet_height || y < 0)
    {
      throw std::out_of_range("Requested sprite was out of range of width of spritesheet");
    }

    SDL_Rect clip_rect = {x, y, _sprite_width - 1, _sprite_height - 1};

    return sprite(sheet_texture, clip_rect);
  }

  spritesheet::sprite::sprite(SDL_Texture *sheet) : sprite(sheet, {0, 0, 0, 0})
  {
  }

  spritesheet::sprite::sprite(SDL_Texture *sheet, SDL_Rect clip_rect)
  {
    this->_sheet_texture = sheet;
    this->_clip_rect = clip_rect;
  }

  void spritesheet::sprite::render(SDL_Renderer *renderer, SDL_Rect *dest_rect, double angle, SDL_Point *center, SDL_RendererFlip flip) const
  {
    int err = SDL_RenderCopyEx(renderer, _sheet_texture, &_clip_rect, dest_rect, angle, center, flip);
    if (err)
    {
      fprintf(stderr, "Could not render sprite: %s\n", SDL_GetError());
      exit(EXIT_FAILURE);
    }
  }
  void spritesheet::sprite::renderF(SDL_Renderer *renderer, SDL_FRect *dest_rect, double angle, SDL_FPoint *center, SDL_RendererFlip flip) const
  {
    int err = SDL_RenderCopyExF(renderer, _sheet_texture, &_clip_rect, dest_rect, angle, center, flip);
    if (err)
    {
      fprintf(stderr, "Could not render sprite: %s\n", SDL_GetError());
      exit(EXIT_FAILURE);
    }
  }
}
