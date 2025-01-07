#include "gui.hpp"
#include <map>
#include <typeinfo>

uint8_t of_max(uint8_t min, uint8_t val, uint8_t max)
{
  return may::clamp<uint8_t>(min, val, max) == val ? val : max;
}

uint8_t uf_min(uint8_t min, uint8_t val, uint8_t max)
{
  return may::clamp<uint8_t>(min, val, max) == val ? val : min;
}

// may::font may::DEFAULT_FONT = may::font();

namespace may
{
  const layout_opts layout_opts::DEFAULT_TEXT(JUSTIFY_LEFT, ALIGN_TOP, 5, SPACING_AROUND, 1, DIRECTION_HORIZONTAL);

  font::font() : _path("")
  {
    this->_source = nullptr;
    this->_pt_size = 0;
  }

  font::font(const char *__path, int __pt_size) : _path(__path)
  {
    this->_source = nullptr;
    this->_pt_size = __pt_size;
  }

  font::font(const font &that)
  {
    this->_path = that._path;
    this->_pt_size = that._pt_size;
    this->_source = nullptr;
  }

  font &font::operator=(const font &that)
  {
    if (_source)
    {
      unload();
    }

    this->_path = that._path;
    this->_pt_size = that._pt_size;
    this->_source = nullptr;
  }

  font::~font()
  {
    unload();
  }

  void font::unload()
  {
    if (_source)
    {
      TTF_CloseFont(_source);
      _source = nullptr;
    }
  }

  TTF_Font *font::ttf_font()
  {
    if (_source == nullptr)
    {
      std::string full_path = SDL_GetBasePath() + _path;
      _source = TTF_OpenFont(full_path.c_str(), _pt_size);
      if (_source == nullptr)
      {
        fprintf(stderr, "unable to load font '%s': %s\n", _path.c_str(), TTF_GetError());
        exit(EXIT_FAILURE);
      }
    }

    return _source;
  }

  int gtext::min_wrap_width(int wwidth) const
  {
    int read_len = 0;
    int max_width = 0;
    while (read_len < _text.length())
    {
      int width, read;
      TTF_MeasureUTF8(_font, _text.c_str(), wwidth, &width, &read);

      read_len += read;
      if (width > max_width)
      {
        max_width = width;
      }
    }

    return max_width;
  }

  gtext::gtext() : gtext("", nullptr)
  {
  }

  gtext::gtext(const std::string &__text, TTF_Font *__font) : gtext(__text, __font, 0, 0)
  {
  }

  gtext::gtext(const std::string &__text, TTF_Font *__font, int __x, int __y) : actor(__x, __y, 0, 0, 0.0, 0.0, 0.0), _text(__text), _font(__font)
  {
    this->_texture = nullptr;
    this->_renderer = nullptr;
    this->_wrap_width = 0;
    this->_color = SDL_Color{0x00, 0x00, 0x00, 0xFF};
    this->_redraw = true;
    this->_render_length = _text.length();
    this->_play_delay = 50;
    this->_timer = 0;
    this->load(nullptr);
  }

  gtext::~gtext()
  {
    unload();
  }

  void gtext::load(SDL_Renderer *renderer)
  {
    if (renderer && (_texture || renderer != _renderer))
    {
      SDL_DestroyTexture(_texture);
      _width = 0;
      _height = 0;
    }

    SDL_Surface *rasterized;
    if (_text.length() > 0)
    {
      rasterized = TTF_RenderUTF8_Solid_Wrapped(_font, _text.substr(0, _render_length).c_str(), _color, _wrap_width);
      if (rasterized == nullptr)
      {
        fprintf(stderr, "unable to rasterize text '%s': %s\n", _text.c_str(), TTF_GetError());
      }

      _width = rasterized->w;
      _height = rasterized->h;

      if (renderer != nullptr)
      {
        _renderer = renderer;

        _texture = SDL_CreateTextureFromSurface(renderer, rasterized);
        if (_texture == nullptr)
        {
          fprintf(stderr, "unable to load rasterized text to graphics memory: %s\n", SDL_GetError());
          exit(EXIT_FAILURE);
        }
      }

      SDL_FreeSurface(rasterized);
    }
    else
    {
      _width = 0;
      _height = 0;
    }
    _redraw = false;
  }

  void gtext::unload()
  {
    if (_texture)
    {
      SDL_DestroyTexture(_texture);
    }
  }

  double gtext::width() const
  {
    if (!_texture && _font)
    {
      int width;
      if (!TTF_MeasureUTF8(_font, _text.c_str(), _wrap_width, &width, nullptr))
      {
        fprintf(stderr, "unable to measure gtext width: %s\n", TTF_GetError());
        SDL_Surface *raster = TTF_RenderUTF8_Solid_Wrapped(_font, _text.c_str(), _color, _wrap_width);

        width = raster->w;

        SDL_FreeSurface(raster);
      }

      return width;
    }
    else
    {
      return _width;
    }
  }

  double gtext::height() const
  {
    if (!_texture && _font)
    {
      double height = 0.0;
      int total = 0;
      while (total < _text.length())
      {
        int count;
        if (!TTF_MeasureUTF8(_font, _text.c_str(), _wrap_width, nullptr, &count))
        {
          fprintf(stderr, "unable to measure gtext height: %s\n", TTF_GetError());

          SDL_Surface *raster = TTF_RenderUTF8_Solid_Wrapped(_font, _text.c_str(), _color, _wrap_width);
          height = raster->h;

          SDL_FreeSurface(raster);

          break;
        }

        total += count;
        height += TTF_FontHeight(_font);
      }

      return height;
    }
    else
    {
      return _height;
    }
  }

  void gtext::render(SDL_Renderer *renderer)
  {
    if (_redraw || _texture == nullptr || renderer != _renderer)
    {
      load(renderer);
    }
    SDL_Rect dest_rect = bounding_box();

    SDL_RenderCopyEx(renderer, _texture, nullptr, &dest_rect, 0.0, nullptr, SDL_FLIP_NONE);
  }

  void gtext::update(game_state &state)
  {
    if (_render_length < _text.length() && state.tick() > _timer)
    {
      ++_render_length;
      _redraw = true;
      _timer = state.tick() + _play_delay;
    }
  }

  void gtext::play(unsigned delay)
  {
    if (delay != 0)
    {
      _play_delay = delay;
    }

    _render_length = 0;
  }

  void pane::destroy()
  {
    for (may::actor *child : _children)
    {
      child->destroy();
      delete child;
    }
  }

  void pane::layout()
  {
    SDL_FRect clip_rect = {positionF().x + _layout.margin(), positionF().y + _layout.margin(), width() - _layout.margin() * 2, height() - _layout.margin() * 2};
    SDL_FPoint content_corner = {0, 0};

    double max_height = 0.0;
    for (actor *child : _children)
    {
      if (content_corner.x + child->width() < clip_rect.w)
      {
        if (child->height() > max_height)
        {
          max_height = child->height();
        }
      }
      else
      {
        content_corner.x = 0.0;
        content_corner.y += max_height + _layout.spacing_size();

        max_height = 0.0;
      }

      child->position(content_corner.x + clip_rect.x, content_corner.y + clip_rect.y);
      content_corner.x += child->width() + _layout.spacing_size();
    }

    for (actor *child : _children)
    {
      pane *child_pane = dynamic_cast<pane *>(child);
      if (child_pane)
      {
        child_pane->layout();
      }
    }
  }

  void pane::fg_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
  {
    _fg_color = {r, g, b, a};
  }

  void pane::bg_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
  {
    _bg_color = {r, g, b, a};
  }

  void pane::remove(const actor &child)
  {
    for (auto iter = _children.begin(); iter != _children.end(); ++iter)
    {
      if (**iter == child)
      {
        _children.erase(iter);
      }
    }
  }

  void pane::remove(size_t i)
  {
    _children.erase(_children.begin() + i);
  }

  void pane::render(SDL_Renderer *renderer)
  {
    SDL_FRect dest = bounding_box();

    SDL_Color orig;
    SDL_GetRenderDrawColor(renderer, &orig.r, &orig.g, &orig.b, &orig.a);

    SDL_SetRenderDrawColor(renderer, _bg_color.r, _bg_color.g, _bg_color.b, _bg_color.a ? 0xFF : 0); // full opacity for border
    SDL_RenderDrawRectF(renderer, &dest);

    SDL_SetRenderDrawColor(renderer, _bg_color.r, _bg_color.g, _bg_color.b, _bg_color.a);
    SDL_RenderFillRectF(renderer, &dest);

    for (actor *child : _children)
    {
      child->render(renderer);
    }
    SDL_SetRenderDrawColor(renderer, orig.r, orig.g, orig.b, orig.a);
  }

  void pane::update(game_state &state)
  {
    for (may::actor *child : _children)
    {
      child->update(state);
    }
  }

  void button::bg_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
  {
    pane::bg_color(r, g, b, a);

    _base_color = {r, g, b, a};
    _hover_color = {of_max(r, r + 0xF, 0xFF), of_max(g, g + 0xF, 0xFF), of_max(b, b + 0xF, 0xFF), a};
    _click_color = {uf_min(0, r - 0xF, r), uf_min(0, g - 0xF, g), uf_min(0, b - 0xF, b), a};
  }

  void button::update(game_state &state)
  {
    if (is_inside(state.mouse_posF(), bounding_box()))
    {
      if (state.is_button_pressed(SDL_BUTTON_LEFT))
      {
        bool was_clicked = _clicked;
        _clicked = _hovered; // "= _hovered" prevents dragging onto the button to click

        if (_clicked && !was_clicked)
        {
          on_click(state);
        }
        else if (_clicked)
        {
          _held = true;
        }
      }
      else
      {
        _clicked = false;
        _held = false;
        _hovered = true;
      }
    }
    else
    {
      _clicked = false;
      _held = false;
      _hovered = false;
    }
  }

  void button::render(SDL_Renderer *renderer)
  {
    if (is_clicked())
    {
      _bg_color = _click_color;
    }
    else if (is_hovered())
    {
      _bg_color = _hover_color;
    }
    else
    {
      _bg_color = _base_color;
    }

    pane::render(renderer);
  }

  void button::unclick()
  {
    _clicked = false;
    _held = false;
    _hovered = false;
  }

  void button::on_click(void (*func)(game_state &))
  {
    _on_click = func;
  }

  void button::on_click(game_state &state)
  {
    if (_on_click)
    {
      _on_click(state);
    }
  }
}
