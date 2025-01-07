#ifndef GUI_HPP_
#define GUI_HPP_

#include "actor.hpp"
#include <vector>
#include <SDL2/SDL_ttf.h>

namespace may
{
  enum justify_t
  {
    JUSTIFY_NONE,
    JUSTIFY_LEFT,
    JUSTIFY_CENTER,
    JUSTIFY_RIGHT
  };

  enum align_t
  {
    ALIGN_NONE,
    ALIGN_TOP,
    ALIGN_CENTER,
    ALIGN_BOTTOM
  };

  enum spacing_t
  {
    SPACING_NONE,
    SPACING_BETWEEN,
    SPACING_AROUND
  };

  enum direction_t
  {
    DIRECTION_HORIZONTAL,
    DIRECTION_VERTICAL
  };

  class layout_opts
  {
    justify_t _justify;
    align_t _align;
    spacing_t _spacing;
    direction_t _direction;
    int _margin;
    int _spacing_size;

  public:
    const static layout_opts DEFAULT_TEXT;
    layout_opts(justify_t __justify, align_t __align, int __margin, spacing_t __spacing, int __spacing_size, direction_t __direction)
    {
      _justify = __justify;
      _align = __align;
      _margin = __margin;
      _spacing = __spacing;
      _spacing_size = __spacing_size;
      _direction = __direction;
    }

    inline justify_t justify() const { return _justify; }
    inline align_t align() const { return _align; }
    inline int margin() const { return _margin; }
    inline spacing_t spacing() const { return _spacing; }
    inline int spacing_size() const { return _spacing_size; }
    inline direction_t direction() const { return _direction; }

    inline void justify(justify_t __justify) { _justify = __justify; }
    inline void align(align_t __align) { _align = __align; }
    inline void margin(int __margin) { _margin = __margin; }
    inline void spacing(spacing_t __spacing) { _spacing = __spacing; }
    inline void spacing_size(int __spacing_size) { _spacing_size = __spacing_size; }
    inline void direction(direction_t __direction) { _direction = __direction; }
  };

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

  class gtext : virtual public actor
  {
    int min_wrap_width(int wwidth) const;

    std::string _text;
    TTF_Font *_font;
    SDL_Texture *_texture;
    SDL_Color _color;
    SDL_Renderer *_renderer;

    bool _redraw;

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

    virtual double width() const override;
    virtual double height() const override;

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
    virtual void update(game_state &state) override;
    void play(unsigned delay = 0);
  };

  class pane : virtual public actor
  {
  protected:
    std::vector<actor *> _children;
    SDL_Color _fg_color;
    SDL_Color _bg_color;

    layout_opts _layout;

  public:
    pane() : pane(0, 0) {}
    pane(int width, int height) : pane(0, 0, width, height) {}
    pane(double x, double y, int width, int height) : pane(SDL_Color{0, 0, 0, 0}, x, y, width, height) {}
    pane(SDL_Color bg_color, double x, double y, int width, int height) : actor(x, y, width, height, 0, 0, 0), _layout(layout_opts::DEFAULT_TEXT)
    {
      this->bg_color(bg_color.r, bg_color.g, bg_color.b, bg_color.a);
      this->fg_color(~bg_color.r, ~bg_color.g, ~bg_color.b, bg_color.a);
    }

    virtual void destroy() override;
    virtual inline layout_opts layout_options() const { return _layout; }
    virtual void layout_options(layout_opts __layout) { _layout = __layout; }
    virtual void layout();

    virtual inline SDL_Color fg_color() const { return _fg_color; }
    virtual void fg_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

    virtual inline SDL_Color bg_color() const { return _bg_color; }
    virtual void bg_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

    template <class WidgetType>
    void add(const WidgetType &__child)
    {
      WidgetType *child_ptr = new WidgetType(__child);
      _children.push_back(child_ptr);
    }

    virtual void remove(const actor &child);
    virtual void remove(size_t i);

    inline std::vector<actor *> &get() { return _children; }
    inline const std::vector<actor *> &get() const { return _children; }

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
    button(SDL_Color bg_color, double x, double y, int width, int height) : actor(x, y, width, height, 0, 0, 0), pane()
    {
      this->bg_color(bg_color.r, bg_color.g, bg_color.b, bg_color.a);
      this->_clicked = false;
      this->_hovered = false;
      this->_held = false;
      this->_on_click = nullptr;
      this->layout_options(layout_opts(JUSTIFY_CENTER, ALIGN_CENTER, 2, SPACING_BETWEEN, 4, DIRECTION_HORIZONTAL));
    }

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
