#ifndef COMMON_HPP_
#define COMMON_HPP_
#include <SDL2/SDL.h>
#include <SDL2/SDL_rect.h>
#include <map>
#include <vector>
#include <string>
#include <random>

#include "mayge_config.h"

inline SDL_Point operator+(const SDL_Point &a, SDL_Point b) { return {a.x + b.x, a.y + b.y}; }
inline SDL_FPoint operator+(const SDL_FPoint &a, SDL_FPoint b) { return {a.x + b.x, a.y + b.y}; }
inline SDL_Rect operator+(const SDL_Rect &a, SDL_Point b) { return {a.x + b.x, a.y + b.y, a.w, a.h}; }

namespace may
{
  static std::default_random_engine _rand_engine;
  template <typename T>
  T random(T min, T max)
  {
    std::uniform_int_distribution<T> distribution(min, max);
    return distribution(_rand_engine);
  }

  template <typename T>
  T clamp(T min, T val, T max)
  {
    if (val < min)
      return min;
    if (val > max)
      return max;
    return val;
  }

  class color_t
  {
    uint16_t _r, _g, _b, _a, _h, _s, _l;
    void rgb2hsl();
    void hsl2rgb();

  public:
    color_t();
    color_t(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

    inline SDL_Color rgba() const { return {_r, _g, _b, _a}; }
    inline uint8_t r() const { return _r; }
    inline uint8_t g() const { return _g; }
    inline uint8_t b() const { return _b; }
    inline uint8_t a() const { return _a; }

    inline SDL_Color hsla() const { return {_h, _s, _l, _a}; }
    inline uint8_t h() const { return _h; }
    inline uint8_t s() const { return _s; }
    inline uint8_t l() const { return _l; }

    inline void rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a) { rgba({r, g, b, a}); }
    void rgba(SDL_Color color);
    inline void r(uint8_t __r) { rgba(__r, _g, _b, _a); }
    inline void g(uint8_t __g) { rgba(_r, __g, _b, _a); }
    inline void b(uint8_t __b) { rgba(_r, _g, __b, _a); }
    inline void a(uint8_t __a) { rgba(_r, _g, _b, __a); }

    inline void hsla(uint8_t h, uint8_t s, uint8_t v, uint8_t a) { hsla({h, s, v, a}); }
    void hsla(SDL_Color color);
    inline void h(uint8_t __h) { hsla(__h, _s, _l, _a); }
    inline void s(uint8_t __s) { hsla(_h, __s, _l, _a); }
    inline void l(uint8_t __l) { hsla(_h, _s, __l, _a); }
  };

  inline double sq_mag(SDL_Point vec) { return vec.x * vec.x + vec.y * vec.y; }
  inline double sq_mag(SDL_FPoint vec) { return vec.x * vec.x + vec.y * vec.y; }
  char *copy_string(const char *string);
  bool colliding(SDL_Rect a, SDL_Rect b);
  bool colliding(SDL_FRect a, SDL_FRect b);
  bool is_inside(SDL_FPoint pt, SDL_FRect rect);
  SDL_Rect scale_by(SDL_Rect rect, int px);
  SDL_FRect scale_by(SDL_FRect rect, float px);
  inline SDL_Point center_of(SDL_Rect rect) { return {rect.x + rect.w / 2, rect.y + rect.h / 2}; }

  class game_state; // Forward declaration of may::game_state for actor::update method
  class actor
  {
    std::map<std::string, std::string> _tags;

  protected:
    double _x, _y;
    double _angle;
    double _speed, _rot_speed;

  public:
    actor();
    actor(double x, double y, double angle);
    actor(double x, double y, double angle, double speed, double rot_speed);
    actor &operator=(const actor &) = default;
    virtual ~actor() = default;

    bool has_tag(const char *key) const;
    const std::string &operator[](const char *key) const;
    std::string &operator[](const char *key);

    bool operator==(const may::actor &that) const;

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

    virtual void update(may::game_state &state) {};
    virtual void render(SDL_Renderer *renderer);
    virtual void destroy();
  };

  class game_state
  {
    std::map<SDL_Keycode, bool> _keys_pressed;
    std::map<uint8_t, bool> _mouse_buttons;
    SDL_Point _mouse_pos = {-999, -999};
    bool _mouse_moved = false;

    double _delta_time = 0.0;
    uint64_t _last_tick = 0;
    float _scroll_x = 0.0f, _scroll_y = 0.0f;

    std::string _composition;
    bool _edited;

  public:
    bool is_key_pressed(SDL_Keycode key) const;
    bool is_key_set(SDL_Keycode key) const;

    void key_down(SDL_Keycode key);
    void key_up(SDL_Keycode key);
    void key_set(SDL_Keycode key);
    void key_unset(SDL_Keycode key);

    inline double delta_time() const { return _delta_time; }

    inline uint64_t tick() const { return _last_tick; }
    void tick(uint64_t _tick);

    inline SDL_Point mouse_pos() const { return _mouse_pos; }
    inline SDL_FPoint mouse_posF() const { return {_mouse_pos.x, _mouse_pos.y}; }
    inline void mouse_pos(int x, int y) { _mouse_pos = {x, y}; }
    inline bool mouse_moved() const { return _mouse_moved; }
    inline void mouse_moved(bool moved) { _mouse_moved = moved; }

    inline float scroll_x() const { return _scroll_x; }
    inline float scroll_y() const { return _scroll_y; }
    inline void scroll_x(float __scroll_x) { _scroll_x = __scroll_x; }
    inline void scroll_y(float __scroll_y) { _scroll_y = __scroll_y; }

    bool is_button_pressed(uint8_t button) const;
    bool is_button_set(uint8_t button) const;

    void button_down(uint8_t button);
    void button_up(uint8_t button);
    void button_set(uint8_t button);
    void button_unset(uint8_t button);

    void add_actor(may::actor &__actor);
    void remove_actor(const may::actor &__actor);

    inline std::string &composition() { return _composition; }
    inline void composition(const std::string &__composition) { _composition = __composition; }
    inline bool edited() const { return _edited; }
    inline void edited(bool __edited) { _edited = __edited; }

    template <typename T>
    T &add_thing(T *thing)
    {
      static T *my_thing = nullptr;
      if (my_thing == nullptr)
      {
        my_thing = thing;
      }
      return *my_thing;
    }
  };
}

#endif /* COMMON_HPP_ */