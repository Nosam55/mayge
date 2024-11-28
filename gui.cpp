#include "gui.hpp"

template <typename T>
T clamp(T min, T val, T max)
{
  if (val < min)
    return max;
  if (val > max)
    return max;
  return val;
}

uint8_t of_max(uint8_t min, uint8_t val, uint8_t max)
{
  return clamp<uint8_t>(min, val, max) == val ? val : max;
}

uint8_t uf_min(uint8_t min, uint8_t val, uint8_t max)
{
  return clamp<uint8_t>(min, val, max) == val ? val : min;
}

namespace may
{
  pane::~pane()
  {
    for (may::actor *child : _children)
    {
      delete child;
    }
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

    for (auto child : _children)
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
      _hovered = true;

      if (state.is_button_pressed(SDL_BUTTON_LEFT))
      {
        bool was_clicked = _clicked;
        _clicked = true;

        if (!was_clicked)
        {
          on_click(state);
        }
        else
        {
          _held = true;
        }
      }
      else
      {
        _clicked = false;
        _held = false;
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

  void button::unclick(bool unhover)
  {
    _clicked = false;
    _held = false;
    if (unhover)
    {
      _hovered = false;
    }
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
