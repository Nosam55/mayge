#ifndef CFG_READER_HPP_
#define CFG_READER_HPP_
#include <string>
#include <vector>
#include "app.hpp"
#include "asteroids_app.hpp"

namespace may
{
  class cfg_reader
  {
    std::string _path;

    std::vector<may::window> _windows;
    may::game_state *_game_state;
    may::app *_app;
    std::vector<may::image> _images;
    std::vector<may::image_actor> _image_actors;
    std::vector<may::floating_actor> _floating_actors;
    void load_data();
  public:
    cfg_reader();
    cfg_reader(const char *path);
    inline may::app& app() { return *_app; }
    inline may::asteroids_app& asteroids_app() { return (may::asteroids_app&)(*_app); }
  };
}

#endif /* CFG_READER_HPP_ */