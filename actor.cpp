#include "actor.hpp"

namespace may
{
  image_actor::image_actor(const char *image_path, int width, int height) : image_actor(image_path, 0.0, 0.0, width, height, 0.0)
  {
  }

  image_actor::image_actor(const char *image_path, double x, double y, double width, double height, double angle) : image_actor(image_path, x, y, width, height, angle, 100, M_PI)
  {
  }

  image_actor::image_actor(const char *image_path, double x, double y, double width, double height, double angle, double speed, double rot_speed) : actor(x, y, width, height, angle, speed, rot_speed), _image()
  {
    this->_pivot_x = width / 2.0;
    this->_pivot_y = height / 2.0;

    this->_image = image::get_image(image_path);
  }

  may::image &image_actor::image()
  {
    return _image;
  }

  void image_actor::image(const char *path)
  {
    _image = image::get_image(path);
  }

  SDL_FRect image_actor::bounding_box() const
  {
    return {_x, _y, static_cast<double>(_width), static_cast<double>(_height)};
  }

  void image_actor::render(SDL_Renderer *renderer)
  {
    SDL_Texture *texture = _image.load_texture(renderer);
    SDL_FRect dest = bounding_box();
    SDL_FPoint pivot = {_pivot_x, _pivot_y};

    int err = SDL_RenderCopyExF(renderer, texture, nullptr, &dest, _angle * 180 * M_1_PI, &pivot, SDL_FLIP_NONE);
    if (err)
    {
      fprintf(stderr, "Could not render %s: %s\n", image().title(), SDL_GetError());
      exit(EXIT_FAILURE);
    }
  }

  void image_actor::destroy()
  {
    this->_image.unload();
    this->_angle = 0.0;
    this->_height = 0;
    this->_pivot_x = 0.0;
    this->_pivot_y = 0.0;
    this->_rot_speed = 0.0;
    this->_speed = 0.0;
    this->_width = 0;
    this->_x = 0.0;
    this->_y = 0.0;
  }

  animated_actor::animated_actor(const may::spritesheet &sheet, double width, double height, double fps) : animated_actor(sheet, 0.0, 0.0, width, height, 0.0, fps)
  {
  }

  animated_actor::animated_actor(const may::spritesheet &sheet, double x, double y, double width, double height, double angle, double fps) : animated_actor(sheet, x, y, width, height, angle, fps, 0.0, 0.0)
  {
  }

  animated_actor::animated_actor(const may::spritesheet &sheet, double x, double y, double width, double height, double angle, double fps, double speed, double rot_speed) : actor(x, y, width, height, angle, speed, rot_speed), _sheet(sheet), _sprite_itr(sheet.looping_iterator())
  {
    this->_fps = fps;
    this->_next_frame_tick = 0;

    this->_pivot_x = width / 2.0;
    this->_pivot_y = height / 2.0;
  }

  SDL_FRect animated_actor::bounding_box() const
  {
    return {this->_x, this->_y, static_cast<double>(this->_width), static_cast<double>(this->_height)};
  }

  void animated_actor::update(may::game_state &state)
  {
    if (_fps != 0.0 && state.tick() >= _next_frame_tick)
    {
      if (_next_frame_tick != 0)
      {
        ++_sprite_itr;
      }
      _next_frame_tick = state.tick() + static_cast<uint64_t>(1000 / _fps);
    }
  }

  void animated_actor::render(SDL_Renderer *renderer)
  {
    if (!_sheet)
    {
      _sheet.load_sprites(renderer);
      _sprite_itr = _sheet.looping_iterator();
    }

    SDL_FRect dest = bounding_box();
    SDL_FPoint center = {_pivot_x, _pivot_y};

    auto sprite = *_sprite_itr;
    sprite.renderF(renderer, &dest, _angle * 180 / M_PI, &center, SDL_FLIP_NONE);
  }

  void animated_actor::destroy()
  {
  }

  floating_actor::floating_actor() : floating_actor(0.0, 0.0, 0.0, 0.0)
  {
  }

  floating_actor::floating_actor(double x, double y, double width, double height) : floating_actor(x, y, width, height, 0.0, 100, M_PI)
  {
  }

  floating_actor::floating_actor(double x, double y, double width, double height, double angle, double speed, double rot_speed) : playable_actor(x, y, width, height, angle, speed, rot_speed)
  {
    this->_vx = 0.0;
    this->_vy = 0.0;
  }

  void floating_actor::update(may::game_state &state)
  {
    move(_vx * state.delta_time(), _vy * state.delta_time());
  }

  void floating_actor::input(may::game_state &state)
  {
    if (state.is_key_pressed(SDLK_UP))
    {
      accelerate(_speed * state.delta_time() * cos(_angle),
                 _speed * state.delta_time() * sin(_angle));
    }

    if (state.is_key_pressed(SDLK_DOWN))
    {
      accelerate(-_speed * state.delta_time() * cos(_angle),
                 -_speed * state.delta_time() * sin(_angle));
    }

    if (state.is_key_pressed(SDLK_LEFT))
    {
      rotate(-_rot_speed * state.delta_time());
    }

    if (state.is_key_pressed(SDLK_RIGHT))
    {
      rotate(_rot_speed * state.delta_time());
    }
  }

  SDL_FPoint floating_actor::velocity() const
  {
    return {_vx, _vy};
  }

  void floating_actor::velocity(double vx, double vy)
  {
    _vx = vx;
    _vy = vy;
  }

  void floating_actor::accelerate(double vx, double vy)
  {
    _vx += vx;
    _vy += vy;
  }
}
