#include "app.hpp"
#include "asteroids_app.hpp"

int main(int argc, char **argv)
{
  SDL_Surface *surface = nullptr;

  // Initialize SDL
  may::asteroids_app app;
  app.start();
  return 0;
}