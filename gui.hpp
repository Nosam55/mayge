#ifndef GUI_HPP_
#define GUI_HPP_

#include "actor.hpp"
#include <vector>
#include <SDL2/SDL_ttf.h>

namespace may
{
  class font
  {
    std::string _path;
    TTF_Font *_source;
    int _pt_size;

  public:
    font();
    font(const char *__path, int __pt_size);
    explicit font(const font &that);
    font &operator=(const font &that);
    ~font();
    void unload();
    TTF_Font *ttf_font();
    inline int pt_size() const { return _pt_size; }
  };

  class gtext
  {
    int min_wrap_width(int wwidth) const;

    std::string _text;
    TTF_Font *_font;
    SDL_Texture *_texture;
    SDL_Color _color;
    SDL_Renderer *_renderer;

    bool _redraw;

    int _x, _y;
    int _width, _height;
    int _wrap_width;

    size_t _render_length;
    unsigned _play_delay;
    uint64_t _timer;

  public:
    gtext();
    gtext(const std::string &__text, TTF_Font *__font);
    gtext(const std::string &__text, TTF_Font *__font, int __x, int __y);
    ~gtext();

    virtual void load(SDL_Renderer *renderer);
    virtual void unload();

    virtual inline SDL_Point position() const { return {_x, _y}; }
    virtual inline void position(SDL_Point __point)
    {
      _x = __point.x;
      _y = __point.y;
    }

    virtual inline void position(int __x, int __y)
    {
      _x = __x;
      _y = __y;
    }

    virtual inline SDL_Color color() const { return _color; }
    virtual inline void color(SDL_Color __color)
    {
      _color = __color;
      _redraw = true;
    }

    virtual inline int wrap_width() const { return _wrap_width; }
    virtual inline void wrap_width(int __wrap_width)
    {
      _wrap_width = __wrap_width;
      _redraw = true;
    }

    virtual inline int width() const { return _width; }
    virtual inline void width(int __width) { _width = __width; }

    virtual inline int height() const { return _height; }
    virtual inline void height(int __height) { _height = __height; }

    virtual inline SDL_Rect bounding_box() const { return {_x, _y, _width, _height}; }

    virtual inline SDL_Texture *texture() const { return _texture; }

    virtual inline TTF_Font *font() const { return _font; }
    virtual inline void font(TTF_Font *__font)
    {
      _font = __font;
      _redraw = true;
    }

    virtual inline const char *text() const { return _text.c_str(); }
    virtual inline void text(const std::string &__text)
    {
      _text = __text;
      _render_length = _text.length();
      _redraw = true;
    }

    inline unsigned play_delay() const { return _play_delay; }
    inline void play_delay(unsigned delay) { _play_delay = delay; }

    inline size_t render_length() const { return _render_length; }
    inline void render_length(size_t __render_length) { _render_length = __render_length; }

    virtual void render(SDL_Renderer *renderer);
    void update(game_state &state);
    void play(unsigned delay = 0);
  };

  class pane : virtual public actor
  {
  protected:
    int _width, _height;
    std::vector<actor *> _children;
    SDL_Color _fg_color;
    SDL_Color _bg_color;

    may::gtext _text;

  public:
    pane() : pane(0, 0) {}
    pane(int width, int height) : pane(0, 0, width, height) {}
    pane(double x, double y, int width, int height) : pane(SDL_Color{0, 0, 0, 0}, x, y, width, height) {}
    pane(SDL_Color bg_color, double x, double y, int width, int height) : actor(x, y, 0, 0, 0)
    {
      this->_width = width;
      this->_height = height;

      this->bg_color(bg_color.r, bg_color.g, bg_color.b, bg_color.a);
      this->fg_color(~bg_color.r, ~bg_color.g, ~bg_color.b, bg_color.a);

      _text.wrap_width(_width);
    }

    virtual ~pane();
    virtual inline int width() const { return _width; }
    virtual inline int height() const { return _height; }

    virtual inline void width(int __width) { _width = __width; }
    virtual inline void height(int __height) { _height = __height; }

    virtual inline const may::gtext &text() const { return _text; }
    virtual inline may::gtext &text() { return _text; }

    virtual void center_text();

    virtual inline SDL_Color fg_color() const { return _fg_color; }
    virtual void fg_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

    virtual inline SDL_Color bg_color() const { return _bg_color; }
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
    inline bool is_clicked() const { return _clicked; }
    inline bool is_held() const { return _held; }
    inline bool is_hovered() const { return _hovered; }

    void unclick();

    void on_click(void (*func)(game_state &));
    virtual void on_click(game_state &state);
  };
}
#endif /* GUI_HPP_ */
