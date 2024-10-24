#ifndef COMMON_HPP_
#define COMMON_HPP_
#include <SDL2/SDL.h>
#include <SDL2/SDL_rect.h>
#include <map>

namespace may
{
  char *copy_string(const char *string);
  bool colliding(SDL_Rect a, SDL_Rect b);
  class game_state
  {
    std::map<SDL_Keycode, bool> _keys_pressed;
    double _delta_time;
    uint64_t _last_tick;

  public:
    bool is_key_pressed(SDL_Keycode key) const;
    inline double delta_time() const { return _delta_time; }
    void key_down(SDL_Keycode key);
    void key_up(SDL_Keycode key);
    void key_set(SDL_Keycode key);
    void tick(uint64_t _tick);
  };
}

#endif /* COMMON_HPP_ */