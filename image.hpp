#ifndef IMAGE_HPP_
#define IMAGE_HPP_
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>
#include <map>
#include <vector>

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
    ~image();
    image(const image &) = default;
    image &operator=(const image &) = default;

    static image &get_image(const char *path);
    SDL_Surface *load_surface();
    SDL_Texture *load_texture(SDL_Renderer *renderer);
    SDL_Surface *surface() const;
    SDL_Texture *texture() const;
    const char *title() const;
    void unload();

    inline bool operator==(const image &that) const { return this->_path == that._path; }
    inline bool operator!=(const image &that) const { return !operator==(that); }
  };

  class spritesheet
  {
    class sprite
    {
      SDL_Rect _clip_rect;
      SDL_Texture *_sheet_texture;

    public:
      sprite(SDL_Texture *sheet);
      sprite(SDL_Texture *sheet, SDL_Rect clip_rect);
      void render(SDL_Renderer *renderer, SDL_Rect *dest_rect, double angle, SDL_Point *center, SDL_RendererFlip flip) const;
      void renderF(SDL_Renderer *renderer, SDL_FRect *dest_rect, double angle, SDL_FPoint *center, SDL_RendererFlip flip) const;
    };

    class iterator
    {
    protected:
      unsigned int _n;
      const spritesheet *_spritesheet;

    public:
      iterator(const spritesheet &__spritesheet, unsigned int n) : _spritesheet(&__spritesheet), _n(n) {}

//      constexpr iterator& operator=(const iterator &that) { this->_n = that._n; this->_spritesheet = that._spritesheet; }
      inline sprite operator*() const { return (*_spritesheet)[_n]; }
      inline void operator++() { ++_n; }
      inline bool operator!=(const iterator &that) const { return _spritesheet->_sheet != that._spritesheet->_sheet || _n != that._n; }
    };

    class loop_iterator : public iterator
    {
    public:
      loop_iterator(const spritesheet &__spritesheet, unsigned int n) : iterator(__spritesheet, n) {}
      inline void operator++() { _n = (_n + 1) % _spritesheet->length(); }
    };

    image _sheet;
    bool _loaded;
    int _sprite_width, _sprite_height;
    int _sheet_width, _sheet_height;
    int _rows, _columns;

  public:
    typedef iterator iterator;
    typedef loop_iterator loop_iterator;

    spritesheet();
    spritesheet(const char *path, int width, int height);
    void load_sprites(SDL_Renderer *renderer);

    sprite operator[](unsigned int index) const;
    operator bool() const { return _loaded; }

    inline size_t length() const { return _rows * _columns; }

    inline iterator begin() const { return iterator(*this, 0); }

    inline iterator end() const { return iterator(*this, length()); }

    inline loop_iterator looping_iterator() const { return loop_iterator(*this, 0); }
  };

}

#endif /* IMAGE_HPP_ */