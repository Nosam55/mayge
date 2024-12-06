#include "app.hpp"
#include "asteroids_app.hpp"
#include "cfg_reader.hpp"
#include "gui.hpp"

#include <time.h>

class myactor : public may::animated_actor, public may::floating_actor
{

public:
  myactor() : may::actor(80, 80, 0, 64, M_PI), may::animated_actor(may::spritesheet("orb_sprites.png", 32, 32), 64, 64, 8.0)
  {
  }

  virtual void update(may::game_state &state) override
  {
    floating_actor::update(state);
    animated_actor::update(state);

    double vel = sqrtf(may::sq_mag(velocity()));
    fps(vel / 4);
  }

  virtual void render(SDL_Renderer *renderer) override
  {
    auto tmp = _angle;
    _angle = 0;
    animated_actor::render(renderer);
    _angle = tmp;

    SDL_FPoint v_begin = center() + position();
    SDL_FPoint v_end = v_begin + velocity();

    int head_dist = _width / 2;
    SDL_FPoint head_pos = v_begin + SDL_FPoint{cos(_angle) * head_dist, sin(_angle) * head_dist};

    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderDrawLineF(renderer, v_begin.x, v_begin.y, v_end.x, v_end.y);

    SDL_RenderDrawPointF(renderer, head_pos.x, head_pos.y);
    SDL_RenderDrawPointF(renderer, head_pos.x, head_pos.y + 1);
    SDL_RenderDrawPointF(renderer, head_pos.x, head_pos.y - 1);
    SDL_RenderDrawPointF(renderer, head_pos.x + 1, head_pos.y);
    SDL_RenderDrawPointF(renderer, head_pos.x - 1, head_pos.y);
  }
};

class clicker : virtual public may::button
{
  bool _highlighted;

public:
  clicker(SDL_Color color, double x, double y) : clicker(color, x, y, 100, 100) {}
  clicker(SDL_Color color, double x, double y, int width, int height) : may::actor(x, y, 0, 0, 0), may::pane(width, height), may::button(color, x, y, width, height)
  {
    _highlighted = false;
  }

  inline bool highlighted() const { return _highlighted; }
  inline void highlighted(bool __x) { _highlighted = __x; }

  virtual void render(SDL_Renderer *renderer) override
  {
    int inc = 2;
    if (highlighted())
    {
      inc *= 10;
    }

    if (is_clicked())
    {
      position(_x + inc, _y + inc);
      _width -= 2 * inc;
      _height -= 2 * inc;

      button::render(renderer);

      position(_x - inc, _y - inc);
      _width += 2 * inc;
      _height += 2 * inc;
    }
    else if (highlighted() || is_hovered())
    {
      position(_x - inc / 2, _y - inc / 2);
      _width += inc;
      _height += inc;

      button::render(renderer);

      position(_x + inc / 2, _y + inc / 2);
      _width -= inc;
      _height -= inc;
    }
    else
    {
      button::render(renderer);
    }
  }
};

class testapp : public may::app
{
  static const size_t NUM_ROWS = 5;
  static const size_t NUM_CLICKERS = 25;
  static const size_t CLICKER_SIZE = 100;

  SDL_Cursor *_cursor;
  clicker *_clickers[NUM_CLICKERS];
  std::vector<size_t> _game_order;
  std::vector<size_t> _input_order;

  std::vector<size_t>::iterator _highlighted;
  uint64_t _timer;
  bool _my_turn;
  bool _lose;

public:
  testapp() : testapp("Title") {}
  testapp(const char *title) : testapp(title, 800, 600) {}
  testapp(const char *title, int w, int h) : may::app(title, w, h), _cursor(nullptr)
  {
    background_color(0x22, 0x22, 0x22, 0xFF);
    _lose = false;
    _my_turn = true;
    _highlighted = _game_order.end();
    _timer = 0;
  }

  ~testapp()
  {
    if (_cursor)
      SDL_FreeCursor(_cursor);
    _cursor = nullptr;

    for (size_t i = 0; i < NUM_CLICKERS; ++i)
    {
      if (_clickers[i])
        delete _clickers[i];
    }
  }

  virtual void init() override
  {
    app::init();

    int err = SDL_SetWindowFullscreen(window().window_ptr(), 0);
    if (err)
    {
      fprintf(stderr, "Error going fullscreen: %s\n", SDL_GetError());
      exit(EXIT_FAILURE);
    }

    _cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_CROSSHAIR);
    SDL_SetCursor(_cursor);

    SDL_SetRenderDrawBlendMode(window().renderer(), SDL_BLENDMODE_BLEND);

    int gap_w = (width() - CLICKER_SIZE * NUM_CLICKERS / NUM_ROWS) / (NUM_CLICKERS / NUM_ROWS + 1);
    int gap_h = (height() - CLICKER_SIZE * NUM_ROWS) / (NUM_ROWS + 1);

    SDL_Color RED = {0xFF, 0x00, 0x00, 0xAA};
    SDL_Color GREEN = {0x00, 0xFF, 0x00, 0xAA};
    SDL_Color BLUE = {0x00, 0x00, 0xFF, 0xAA};
    SDL_Color YELLOW = {0xFF, 0xFF, 0x00, 0xAA};

    srand(time(nullptr));
    for (size_t r = 0; r < NUM_ROWS; ++r)
    {
      for (size_t c = 0; c < NUM_CLICKERS / NUM_ROWS; ++c)
      {
        clicker *new_clicker = new clicker(
            SDL_Color{rand() & 0xFF, rand() & 0xFF, rand() & 0xFF, 0xAA},
            gap_w + c * (CLICKER_SIZE + gap_w),
            gap_h + r * (CLICKER_SIZE + gap_h),
            CLICKER_SIZE,
            CLICKER_SIZE);

        _clickers[r * NUM_CLICKERS / NUM_ROWS + c] = new_clicker;
      }
    }

    _timer = SDL_GetTicks64() + 1000;
  }

  void game_loop(SDL_Renderer *renderer) override
  {
    auto &state = game_state();

    for (size_t i = 0; i < NUM_CLICKERS; ++i)
    {
      clicker *c = _clickers[i];
      c->render(renderer);
    }

    if (_lose)
    {
      SDL_SetRenderDrawColor(renderer, 0xFF, 0, 0, 0x7F);
      SDL_RenderFillRect(renderer, nullptr);

      if(state.is_button_set(SDL_BUTTON_LEFT)){
        SDL_QuitEvent evt;
        evt.timestamp = SDL_GetTicks();
        evt.type = SDL_QUIT;
        SDL_PushEvent((SDL_Event*)(&evt));
      }

      return;
    }

    if (_my_turn)
    {
      if (SDL_GetTicks64() >= _timer)
      {
        if (_game_order.size() && _clickers[_game_order.back()]->is_clicked())
        {
          _clickers[_game_order.back()]->unclick(true);
        }

        _game_order.push_back(rand() % NUM_CLICKERS);

        _highlighted = _game_order.begin();
        _clickers[*_highlighted]->highlighted(true);
        _timer = state.tick() + 1000;

        _my_turn = false;
      }
      else{
        if(_game_order.size() && _clickers[_game_order.back()]->is_hovered()){
          _clickers[_game_order.back()]->update(state);
        }
      }
    }
    else if (_highlighted != _game_order.end())
    {
      if (state.tick() >= _timer)
      {
        _timer = state.tick() + 1000;
        _clickers[*_highlighted]->highlighted(false);
        ++_highlighted;

        if (_highlighted != _game_order.end())
        {
          _clickers[*_highlighted]->highlighted(true);
        }
      }
    }
    else
    {
      for (size_t i = 0; i < NUM_CLICKERS; ++i)
      {
        clicker *c = _clickers[i];

        c->update(state);

        if (c->is_clicked() && !c->is_held())
        {
          state.button_unset(SDL_BUTTON_LEFT);

          _input_order.push_back(i);
          if (_game_order.at(_input_order.size() - 1) != i)
          {
            _lose = true;
          }
          else
          {
            if (_game_order.size() == _input_order.size())
            {
              _input_order.clear();
              _my_turn = true;
              _timer = state.tick() + 1000;
            }
          }
        }
      }
    }
    // this->loop_on_border(actor);
  }
};

int main(int argc, char **argv)
{
  // may::cfg_reader config("test.cfg");
  //  may::app &app = config.app();
  testapp app("Simon Says");
  app.start();
  return 0;
}