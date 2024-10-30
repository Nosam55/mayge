#ifndef IMAGE_HPP_
#define IMAGE_HPP_
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>
#include <map>

namespace may
{
  class image
  {
    static std::map<std::string, image> _unique_images;
    
    std::string _path;
    SDL_Surface *_surface;
    SDL_Texture *_texture;
    SDL_Renderer *_renderer;
    image(const char *path);

  public:
    image();
    image(const image&) = default;
    image &operator=(const image&) = default;
    
    static image &get_image(const char *path);
    SDL_Surface *load_surface();
    SDL_Texture *load_texture(SDL_Renderer *renderer);
    SDL_Surface *surface() const;
    SDL_Texture *texture() const;
    const char *title() const;
    void unload();
  };
}

#endif /* IMAGE_HPP_ */