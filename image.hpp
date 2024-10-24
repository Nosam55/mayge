#ifndef IMAGE_HPP_
#define IMAGE_HPP_
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

namespace may
{
  class image
  {
    const char *_path;
    SDL_Surface *_surface;
    SDL_Texture *_texture;
    SDL_Renderer *_renderer;

  public:
    image(const char *path);
    SDL_Surface *load_surface();
    SDL_Texture *load_texture(SDL_Renderer *renderer);
    SDL_Surface *surface() const;
    SDL_Texture *texture() const;
    const char *title() const;
    void unload();
  };
}

#endif /* IMAGE_HPP_ */