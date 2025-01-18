#include "common.hpp"
#include <string>

namespace may
{
  template <>
  inline float random(float min, float max)
  {
    std::uniform_real_distribution<float> distribution(min, max);
    return distribution(_rand_engine);
  }

  template <>
  inline double random(double min, double max)
  {
    std::uniform_real_distribution<double> distribution(min, max);
    return distribution(_rand_engine);
  }

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

    char buffer[32];
    for (size_t i = 0; i < sizeof(buffer) / sizeof(*buffer); ++i)
    {
      buffer[i] = random<char>('0', 'Z'); // which characters are eligible for random selection
    }

    buffer[sizeof(buffer) / sizeof(*buffer) - 1] = 0;

    _tags["id"] = buffer;
  }

  bool actor::has_tag(const char *key) const
  {
    return _tags.count(key) > 0;
  }

  const std::string &actor::operator[](const char *key) const
  {
    return _tags.at(key);
  }

  std::string &actor::operator[](const char *key)
  {
    return _tags[key];
  }

  bool actor::operator==(const may::actor &that) const
  {
    if (this->has_tag("id") && that.has_tag("id"))
    {
      return (*this)["id"] == that["id"];
    }
    else
    {
      return this == &that;
    }
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

  void actor::render(SDL_Renderer *renderer)
  {
  }

  void actor::destroy()
  {
  }

  bool game_state::is_key_pressed(SDL_Keycode key) const
  {
    return _keys_pressed.count(key) > 0;
  }

  bool game_state::is_key_set(SDL_Keycode key) const
  {
    return _keys_pressed.count(key) && _keys_pressed.at(key);
  }

  void game_state::key_down(SDL_Keycode key)
  {
    _keys_pressed[key] = true;
  }

  void game_state::key_up(SDL_Keycode key)
  {
    _keys_pressed.erase(key);
  }

  void game_state::key_set(SDL_Keycode key)
  {
    if (_keys_pressed.count(key))
    {
      _keys_pressed[key] = true;
    }
  }

  void game_state::key_unset(SDL_Keycode key)
  {
    if (_keys_pressed.count(key))
    {
      _keys_pressed[key] = false;
    }
  }

  void game_state::tick(uint64_t _tick)
  {
    _delta_time = static_cast<double>(_tick - _last_tick) / 1000.0;
    _last_tick = _tick;
  }

  bool game_state::is_button_pressed(uint8_t button) const
  {
    return _mouse_buttons.count(button) > 0;
  }

  bool game_state::is_button_set(uint8_t button) const
  {
    return _mouse_buttons.count(button) > 0 && _mouse_buttons.at(button);
  }

  void game_state::button_down(uint8_t button)
  {
    _mouse_buttons[button] = true;
  }

  void game_state::button_up(uint8_t button)
  {
    _mouse_buttons.erase(button);
  }

  void game_state::button_set(uint8_t button)
  {
    if (_mouse_buttons.count(button) > 0)
    {
      _mouse_buttons[button] = true;
    }
  }

  void game_state::button_unset(uint8_t button)
  {
    if (_mouse_buttons.count(button) > 0)
    {
      _mouse_buttons[button] = false;
    }
  }

  void game_state::add_actor(may::actor &__actor)
  {
    // _actors.push_back(__actor);
  }

  void game_state::remove_actor(const may::actor &__actor)
  {
    // for (auto it = _actors.begin(); it != _actors.end(); it++)
    // {
    //   if (*it == __actor)
    //   {
    //   }
    // }
  }

  char *copy_string(const char *string)
  {
    size_t len = 0;
    for (const char *p = string; *p != 0; ++p)
    {
      ++len;
    }

    char *ret = new char[len];
    for (size_t i = 0; i < len; ++i)
    {
      ret[i] = string[i];
    }
    return ret;
  }

  bool colliding(SDL_FRect a, SDL_FRect b)
  {
    bool cx = false, cy = false;
    int dx = b.x - a.x,
        dy = b.y - a.y;

    if (dx >= 0 && dx < a.w)
    {
      cx = true;
    }
    else if (dx < 0 && dx + b.w > 0)
    {
      cx = true;
    }

    if (dy >= 0 && dy < a.h)
    {
      cy = true;
    }
    else if (dy < 0 && dy + b.h > 0)
    {
      cy = true;
    }

    return cx && cy;
  }

  bool is_inside(SDL_FPoint pt, SDL_FRect rect)
  {
    if (rect.x < pt.x && pt.x < rect.x + rect.w)
    {
      return rect.y < pt.y && pt.y < rect.y + rect.h;
    }
    else
    {
      return false;
    }
  }

  SDL_Rect scale_by(SDL_Rect rect, int px)
  {
    return SDL_Rect{rect.x - px, rect.y - px, rect.w + 2 * px, rect.h + 2 * px};
  }

  SDL_FRect scale_by(SDL_FRect rect, float px)
  {
    return SDL_FRect{rect.x - px, rect.y - px, rect.w + 2.0f * px, rect.h + 2.0f * px};
  }

  bool colliding(SDL_Rect a, SDL_Rect b)
  {
    SDL_FRect aa = {a.x, a.y, a.w, a.h};
    SDL_FRect bb = {b.x, b.y, b.w, b.h};

    return colliding(aa, bb);
  }
}