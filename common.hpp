#ifndef COMMON_HPP_
#define COMMON_HPP_
#include <SDL2/SDL.h>
#include <SDL2/SDL_rect.h>
#include <map>
#include <vector>
#include <string>

inline SDL_Point operator+(const SDL_Point &a, const SDL_Point &b) { return {a.x + b.x, a.y + b.y}; }
inline SDL_FPoint operator+(const SDL_FPoint &a, const SDL_FPoint &b) { return {a.x + b.x, a.y + b.y}; }

namespace may
{
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
    bool _mouse_moved;

    double _delta_time;
    uint64_t _last_tick;

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

    bool is_button_pressed(uint8_t button) const;
    bool is_button_set(uint8_t button) const;

    void button_down(uint8_t button);
    void button_up(uint8_t button);
    void button_set(uint8_t button);
    void button_unset(uint8_t button);

    void add_actor(may::actor &__actor);
    void remove_actor(const may::actor &__actor);
  };
}

#endif /* COMMON_HPP_ */