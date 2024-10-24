#ifndef ACTOR_HPP_
#define ACTOR_HPP_
#include "image.hpp"
#include "common.hpp"
#include <string>

namespace may
{
  const SDL_Point DIR_UP = {0.0, 1.0};
  const SDL_Point DIR_LEFT = {-1.0, 0.0};
  const SDL_Point DIR_DOWN = {0.0, -1.0};
  const SDL_Point DIR_RIGHT = {1.0, 0.0};

  class actor
  {
  protected:
    double _x, _y;
    double _angle;
    double _speed, _rot_speed;

  public:
    actor();
    actor(double x, double y, double angle);
    actor(double x, double y, double angle, double speed, double rot_speed);

    SDL_FPoint position() const;
    virtual void position(SDL_FPoint pos);
    virtual void position(double x, double y);

    double angle() const;
    virtual void angle(double __angle);

    double speed() const;
    virtual void speed(double __speed);

    double rot_speed() const;
    virtual void rot_speed(double __rot_speed);

    virtual void move(double delta_time);
    virtual void move(double x, double y);
    virtual void rotate(double angle);
    virtual void input(may::game_state &state);

    virtual void render(SDL_Renderer *renderer) = 0;
    virtual void update(may::game_state &state);
  };

  class image_actor : public actor
  {
  protected:
    may::image _image;
    int _width, _height;
    double _pivot_x, _pivot_y;

  public:
    image_actor(const char *image_path, int width, int height);
    image_actor(const char *image_path, double x, double y, int width, int height, double angle);
    image_actor(const char *image_path, double x, double y, int width, int height, double angle, double speed, double rot_speed);

    may::image &image();
    virtual SDL_Rect bounding_box() const;
    void render(SDL_Renderer *renderer) override;
  };

  class simple_actor : public image_actor
  {
  public:
    simple_actor(const char *image_path, int width, int height);
    simple_actor(const char *image_path, double x, double y, int width, int height, double angle);
    simple_actor(const char *image_path, double x, double y, int width, int height, double angle, double speed, double rot_speed);
    virtual void input(may::game_state &state) override;
  };

  class floating_actor : public image_actor
  {
    double _vx, _vy;
  public:
    floating_actor(const char *image_path, int width, int height);
    floating_actor(const char *image_path, double x, double y, int width, int height, double angle);
    floating_actor(const char *image_path, double x, double y, int width, int height, double angle, double speed, double rot_speed);
    virtual void update(may::game_state &state);
    virtual void input(may::game_state &state) override;
    void render(SDL_Renderer *renderer) override;

    SDL_FPoint velocity() const;
    void velocity(double vx, double vy);
    void accelerate(double vx, double vy);
  };
}
#endif /* ACTOR_HPP_ */