#ifndef ACTOR_HPP_
#define ACTOR_HPP_
#include "image.hpp"
#include "common.hpp"
#include <string>
#include <map>

namespace may
{
  const SDL_FPoint DIR_UP = {0.0, 1.0};
  const SDL_FPoint DIR_LEFT = {-1.0, 0.0};
  const SDL_FPoint DIR_DOWN = {0.0, -1.0};
  const SDL_FPoint DIR_RIGHT = {1.0, 0.0};

  class playable_actor : virtual public actor
  {
  public:
    playable_actor() : playable_actor(0.0, 0.0, 0.0) {}
    playable_actor(double x, double y, double angle) : playable_actor(x, y, angle, 0.0, 0.0) {}
    playable_actor(double x, double y, double angle, double speed, double rot_speed) : actor(x, y, angle, speed, rot_speed) {}
    virtual void input(may::game_state &state)
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
    };
  };

  class image_actor : virtual public actor
  {
  protected:
    may::image _image;
    int _width, _height;
    double _pivot_x, _pivot_y;

  public:
    image_actor(const char *image_path, int width, int height);
    image_actor(const char *image_path, double x, double y, int width, int height, double angle);
    image_actor(const char *image_path, double x, double y, int width, int height, double angle, double speed, double rot_speed);

    inline int width() const { return _width; }
    inline int height() const { return _height; }
    inline SDL_FPoint center() const { return {_pivot_x, _pivot_y}; }
    inline void center(SDL_FPoint __center)
    {
      _pivot_x = __center.x;
      _pivot_y = __center.y;
    }

    may::image &image();
    void image(const char *path);
    virtual SDL_FRect bounding_box() const;
    virtual void render(SDL_Renderer *renderer) override;
    virtual void destroy() override;
  };

  class animated_actor : virtual public actor
  {

    uint64_t _next_frame_tick;
    spritesheet::loop_iterator _sprite_itr;

  protected:
    may::spritesheet _sheet;
    int _width, _height;
    double _pivot_x, _pivot_y;
    double _fps;

  public:
    animated_actor(const may::spritesheet &sheet, int width, int height, double fps);
    animated_actor(const may::spritesheet &sheet, double x, double y, int width, int height, double angle, double fps);
    animated_actor(const may::spritesheet &sheet, double x, double y, int width, int height, double angle, double fps, double speed, double rot_speed);

    inline double fps() const { return _fps; }
    inline void fps(double __fps)
    {
      if (__fps * __fps > 0.000000000001 && _fps * _fps > 0.000000000001) // if the "calculated next tick" wont be infinite
      {
        _next_frame_tick += (uint64_t)((1000.0 / __fps) - (1000.0 / this->_fps)); // adjust the animation for the changing framerate
      }
      this->_fps = __fps;
    }

    inline may::spritesheet &sheet() { return _sheet; }
    inline void sheet(const may::spritesheet &__sheet) { this->_sheet = __sheet; }
    inline SDL_FPoint center() const { return {_pivot_x, _pivot_y}; }
    inline void center(double x, double y)
    {
      _pivot_x = x;
      _pivot_y = y;
    }
    inline void center(SDL_FPoint __center)
    {
      _pivot_x = __center.x;
      _pivot_y = __center.y;
    }

    virtual SDL_FRect bounding_box() const;

    virtual void update(may::game_state &state) override;
    virtual void render(SDL_Renderer *renderer) override;
    virtual void destroy() override;
  };

  class floating_actor : virtual public playable_actor
  {
    double _vx, _vy;

  public:
    floating_actor();
    floating_actor(const playable_actor &that);
    floating_actor(double x, double y, double angle);
    floating_actor(double x, double y, double angle, double speed, double rot_speed);
    virtual void update(may::game_state &state) override;
    virtual void input(may::game_state &state) override;

    SDL_FPoint velocity() const;
    void velocity(double vx, double vy);
    void accelerate(double vx, double vy);
  };
}
#endif /* ACTOR_HPP_ */