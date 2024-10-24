#include "app.hpp"

int main(int argc, char **argv)
{
  SDL_Surface *surface = nullptr;

  // Initialize SDL
  may::app app;
  app.main_loop();
  return 0;
}