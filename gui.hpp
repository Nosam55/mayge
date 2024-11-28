#ifndef GUI_HPP_
#define GUI_HPP_

#include "actor.hpp"
#include <vector>

namespace may
{
  class pane : virtual public actor
  {
  protected:
    int _width, _height;
    std::vector<actor *> _children;
    SDL_Color _bg_color;

  public:
    pane() : pane(0, 0) {}
    pane(int width, int height) : pane(0, 0, width, height) {}
    pane(double x, double y, int width, int height) : pane(SDL_Color{0, 0, 0, 0}, x, y, width, height) {}
    pane(SDL_Color bg_color, double x, double y, int width, int height) : actor(x, y, 0, 0, 0)
    {
      this->_width = width;
      this->_height = height;
      this->bg_color(bg_color.r, bg_color.g, bg_color.b, bg_color.a);
    }

    virtual ~pane();
    virtual inline int width() const { return _width; }
    virtual inline int height() const { return _height; }

    virtual inline void width(int __width) { _width = __width; }
    virtual inline void height(int __height) { _height = __height; }

    virtual SDL_Color bg_color() const { return _bg_color; }
    virtual void bg_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

    template <class WidgetType>
    void add(const WidgetType &child) { _children.push_back(new WidgetType(child)); }
    virtual void remove(const actor &child);
    virtual void remove(size_t i);

    virtual inline SDL_FRect bounding_box() const { return {_x, _y, (double)_width, (double)_height}; }
    virtual void render(SDL_Renderer *renderer) override;
    virtual void update(game_state &state) override;
  };

  class button : virtual public pane
  {
  protected:
    SDL_Color _hover_color;
    SDL_Color _click_color;
    SDL_Color _base_color;
    bool _clicked;
    bool _held;
    bool _hovered;
    void (*_on_click)(game_state &);

  public:
    button() : button(0, 0) {}
    button(int width, int height) : button(0, 0, width, height) {}
    button(double x, double y, int width, int height) : button(SDL_Color{0, 0, 0, 0}, x, y, width, height) {}
    button(SDL_Color bg_color, double x, double y, int width, int height) : actor(x, y, 0), pane(width, height)
    {
      this->bg_color(bg_color.r, bg_color.g, bg_color.b, bg_color.a);
      this->_clicked = false;
      this->_hovered = false;
      this->_held = false;
      this->_on_click = nullptr;
    }

    button(const button &that) = default;

    virtual void bg_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
    virtual void update(game_state &state) override;
    virtual void render(SDL_Renderer *renderer) override;

    void unclick(bool unhover = false);
    inline bool is_clicked() const { return _clicked; }
    inline bool is_held() const { return _held; }
    inline bool is_hovered() const { return _hovered; }

    void on_click(void (*func)(game_state &));
    virtual void on_click(game_state &state);
  };
}
#endif /* GUI_HPP_ */
