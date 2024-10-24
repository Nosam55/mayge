#include "actor.hpp"
namespace may
{
  actor::actor() : actor(0.0, 0.0, 0.0)
  {
  }

  actor::actor(double x, double y, double angle) : actor(x, y, angle, 100, M_PI)
  {
  }

  actor::actor(double x, double y, double angle, double speed, double rot_speed)
  {
    this->_x = x;
    this->_y = y;
    this->_angle = angle;
    this->_speed = speed;
    this->_rot_speed = rot_speed;
  }

  SDL_FPoint actor::position() const
  {
    return {_x, _y};
  }

  void actor::position(SDL_FPoint pos)
  {
    _x = pos.x;
    _y = pos.y;
  }

  void actor::position(double x, double y)
  {
    _x = x;
    _y = y;
  }

  double actor::angle() const
  {
    return _angle;
  }

  void actor::angle(double __angle)
  {
    _angle = __angle;
  }

  double actor::speed() const
  {
    return _speed;
  }

  void actor::speed(double __speed)
  {
    _speed = __speed;
  }

  double actor::rot_speed() const
  {
    return _rot_speed;
  }

  void actor::rot_speed(double __rot_speed)
  {
    _rot_speed = __rot_speed;
  }

  void actor::move(double delta_time)
  {
    _x += _speed * cos(_angle) * delta_time;
    _y += _speed * sin(_angle) * delta_time;
  }

  void actor::move(double x, double y)
  {
    _x += x;
    _y += y;
  }

  void actor::rotate(double angle)
  {
    _angle += angle;
  }

  void actor::input(may::game_state &state)
  {
  }

  void actor::update(may::game_state &state)
  {
  }

  image_actor::image_actor(const char *image_path, int width, int height) : image_actor(image_path, 0.0, 0.0, width, height, 0.0)
  {
  }

  image_actor::image_actor(const char *image_path, double x, double y, int width, int height, double angle) : image_actor(image_path, x, y, width, height, angle, 100, M_PI)
  {
  }

  image_actor::image_actor(const char *image_path, double x, double y, int width, int height, double angle, double speed, double rot_speed) : _image(image_path), actor(x, y, angle, speed, rot_speed)
  {
    this->_width = width;
    this->_height = height;
    this->_pivot_x = width / 2.0;
    this->_pivot_y = height / 2.0;
  }

  may::image &image_actor::image()
  {
    return _image;
  }

  SDL_Rect image_actor::bounding_box() const
  {
    return {_x, _y, _width, _height};
  }

  void image_actor::render(SDL_Renderer *renderer)
  {
    SDL_Texture *texture = _image.load_texture(renderer);
    SDL_FRect dest = {_x, _y, static_cast<double>(_width), static_cast<double>(_height)};
    SDL_FPoint pivot = {_pivot_x, _pivot_y};

    int err = SDL_RenderCopyExF(renderer, texture, nullptr, &dest, _angle * 180 * M_1_PI, &pivot, SDL_FLIP_NONE);
    if (err)
    {
      fprintf(stderr, "Could not render %s: %s\n", image().title(), SDL_GetError());
      exit(EXIT_FAILURE);
    }
  }

  simple_actor::simple_actor(const char *image_path, int width, int height) : simple_actor(image_path, 0.0, 0.0, width, height, 0.0)
  {
  }

  simple_actor::simple_actor(const char *image_path, double x, double y, int width, int height, double angle) : simple_actor(image_path, x, y, width, height, angle, 100, M_PI)
  {
  }

  simple_actor::simple_actor(const char *image_path, double x, double y, int width, int height, double angle, double speed, double rot_speed) : image_actor(image_path, x, y, width, height, angle, speed, rot_speed)
  {
  }

  void simple_actor::input(may::game_state &state)
  {
    if (state.is_key_pressed(SDLK_UP))
    {
      _y -= _speed * state.delta_time();
    }
    if (state.is_key_pressed(SDLK_DOWN))
    {
      _y += _speed * state.delta_time();
    }
    if (state.is_key_pressed(SDLK_LEFT))
    {
      _x -= _speed * state.delta_time();
    }
    if (state.is_key_pressed(SDLK_RIGHT))
    {
      _x += _speed * state.delta_time();
    }
  }

  floating_actor::floating_actor(const char *image_path, int width, int height) : floating_actor(image_path, 0.0, 0.0, width, height, 0.0)
  {
  }

  floating_actor::floating_actor(const char *image_path, double x, double y, int width, int height, double angle) : floating_actor(image_path, x, y, width, height, angle, 100, M_PI)
  {
  }

  floating_actor::floating_actor(const char *image_path, double x, double y, int width, int height, double angle, double speed, double rot_speed) : image_actor(image_path, x, y, width, height, angle, speed, rot_speed)
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

  void floating_actor::render(SDL_Renderer *renderer)
  {
    this->image_actor::render(renderer);
    SDL_SetRenderDrawColor(renderer, 0x00, 0xFF, 0x00, 0xFF);
    SDL_RenderDrawLineF(renderer, static_cast<float>(_x + _width / 2.0), static_cast<float>(_y + _height / 2.0), static_cast<float>(_x + _vx / 4.0 + _width / 2.0), static_cast<float>(_y + _vy / 4.0 + _height / 2.0));
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
