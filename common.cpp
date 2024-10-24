#include "common.hpp"

namespace may
{
  bool game_state::is_key_pressed(SDL_Keycode key) const
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
      _keys_pressed[key] = false;
    }
  }

  void game_state::tick(uint64_t _tick)
  {
    _delta_time = static_cast<double>(_tick - _last_tick) / 1000.0;
    _last_tick = _tick;
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

  bool colliding(SDL_Rect a, SDL_Rect b)
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
}