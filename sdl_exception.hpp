#ifndef SDL_EXCEPTION_HPP_
#define SDL_EXCEPTION_HPP_
#include <SDL2/SDL.h>
#include <exception>
#include <cstring>

namespace may {
  class sdl_exception : std::exception {
    char *msg;
  public:
    sdl_exception(){
      const char *msg = SDL_GetError();
      size_t len = std::strlen(msg);
      this->msg = new char[len];
      std::strncpy(this->msg, msg, len);
    }
    sdl_exception(const sdl_exception &that){
      delete[] this->msg;
      size_t len = std::strlen(that.msg);
      this->msg = new char[len];
      std::strncpy(this->msg, that.msg, len);
    }
    sdl_exception& operator=(const sdl_exception& that){
      delete[] this->msg;
      size_t len = std::strlen(that.msg);
      this->msg = new char[len];
      std::strncpy(this->msg, that.msg, len);
    }
    const char *what() {
      return this->msg;
    }
    ~sdl_exception(){
      delete[] this->msg;
    }
  };
}

#endif