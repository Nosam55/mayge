#include "cfg_reader.hpp"
#include <cstdio>
#include <cstring>
#include <algorithm>
#include <string>

int stricmp(const char *a, const char *b, size_t n = 0xFFFFFFFF)
{
  size_t i;
  int cmp_val = 0;
  for (i = 0; a[i] != 0 && b[i] != 0 && i < n; ++i)
  {
    if (toupper(a[i]) > toupper(b[i]))
    {
      return 1;
    }
    else if (toupper(a[i] < toupper(b[i])))
    {
      return -1;
    }
  }
  if (a[i] > b[i])
  {
    return 1;
  }
  else if (a[i] < b[i])
  {
    return -1;
  }
  else
  {
    return 0;
  }
}

const char *strrms(const char *a)
{
  static std::string _str;

  _str.clear();
  for (a; *a != 0; ++a)
  {
    if (!isspace(*a))
    {
      _str += *a;
    }
  }

  return _str.c_str();
}

const char *strrip(const char *a)
{
  static std::string _str;

  _str.clear();

  for (a; *a && isspace(*a); ++a)
  {
  }

  while (*a)
  {
    for (a; *a && !isspace(*a); ++a)
    {
      _str += *a;
    }

    if (*a && isspace(*a))
    {
      std::string spaces;
      for (a; *a && isspace(*a); ++a)
      {
        spaces += *a;
      }
      if (*a)
      {
        _str += spaces;
      }
    }
  }

  return _str.c_str();
}
enum READ_STATE
{
  INIT,
  APP,
  ASTEROIDS_APP,
  WINDOW,
  GAME_STATE,
  IMAGE,
  IMAGE_ACTOR,
  SIMPLE_ACTOR,
  FLOATING_ACTOR
};
namespace may
{
  void cfg_reader::load_data()
  {
    const size_t BUFFER_SIZE = 512;
    char buffer[BUFFER_SIZE];

    std::vector<std::string> lines;
    READ_STATE state = INIT;

    FILE *file = fopen(_path.c_str(), "r");
    while (!feof(file) && !ferror(file))
    {
      std::string line;
      fgets(buffer, BUFFER_SIZE, file);

      size_t i;
      for (i = 0; buffer[i] != '\n'; ++i)
      {
      }
      buffer[i] = 0;

      switch (state)
      {
      case READ_STATE::INIT:
      {
        if (stricmp("[App]", strrms(buffer)) == 0)
        {
          state = APP;
          if (_app)
          {
            delete _app;
          }

          _app = new may::app();
        }
        else if (stricmp("[AsteroidsApp]", strrms(buffer)) == 0)
        {
          state = ASTEROIDS_APP;
          if (_app)
          {
            delete _app;
          }

          _app = new may::asteroids_app();
        }
        else if (stricmp("[Window]", strrms(buffer)) == 0)
        {
          state = WINDOW;

          _windows.push_back(may::window());
        }
        else if (stricmp("[GameState]", strrms(buffer)) == 0)
        {
          state = GAME_STATE;
        }
        else if (stricmp("[Image]", strrms(buffer)) == 0)
        {
          state = IMAGE;
        }
        else if (stricmp("[ImageActor]", strrms(buffer)) == 0)
        {
          state = IMAGE_ACTOR;
        }
        else if (stricmp("[SimpleActor]", strrms(buffer)) == 0)
        {
          state = SIMPLE_ACTOR;
        }
        else if (stricmp("[FloatingActor]", strrms(buffer)) == 0)
        {
          state = FLOATING_ACTOR;
        }
        break;
      }
      case READ_STATE::ASTEROIDS_APP:
      {
        char * const eq = strchr(buffer, '=');
        *eq = 0;

        const char *key = strrms(buffer);
        const char *val = eq + 1;

        may::asteroids_app &app = asteroids_app();
        if (stricmp("PlayerImage", key) == 0)
        {
          app.player_image(strrip(val));
        }
        else if (stricmp("BulletImage", key) == 0)
        {
          app.bullet_image(strrip(val));
        }

        *eq = '=';
      }
      case READ_STATE::APP:
      {
        char *eq = strchr(buffer, '=');
        *eq = 0;

        const char *key = strrms(buffer);
        const char *val = eq + 1;

        may::app &app = this->app();
        if (stricmp("title", key) == 0)
        {
          app.title(val);
        }
        else if (stricmp("width", key) == 0)
        {
          app.width(atoi(val));
        }
        else if (stricmp("height", key) == 0)
        {
          app.height(atoi(val));
        }
        else if (stricmp("x", key) == 0)
        {
          app.x(atoi(val));
        }
        else if (stricmp("y", key) == 0)
        {
          app.y(atoi(val));
        }
        else if (stricmp("BackgroundImage", key) == 0)
        {
          app.background_image(strrip(val));
        }
        break;
      }
      case READ_STATE::WINDOW:
      {
        char *eq = strchr(buffer, '=');
        *eq = 0;

        const char *key = strrms(buffer);
        const char *val = eq + 1;

        may::window &win = _windows[_windows.size() - 1];
        if (stricmp("title", key) == 0)
        {
          win.title(val);
        }
        else if (stricmp("width", key) == 0)
        {
          win.width(atoi(val));
        }
        else if (stricmp("height", key) == 0)
        {
          win.height(atoi(val));
        }
        else if (stricmp("x", key) == 0)
        {
          win.x(atoi(val));
        }
        else if (stricmp("y", key) == 0)
        {
          win.y(atoi(val));
        }
        break;
      }
      }
    }

    fclose(file);
  }

  cfg_reader::cfg_reader() : cfg_reader("")
  {
  }

  cfg_reader::cfg_reader(const char *path) : _path(path)
  {
    this->_game_state = nullptr;
    this->_app = nullptr;

    if (*path != 0)
    {
      load_data();
    }
  }
}