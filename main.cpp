#include "app.hpp"
#include "asteroids_app.hpp"
#include "cfg_reader.hpp"

int main(int argc, char **argv)
{
  may::cfg_reader config("test.cfg");

  may::app &app = config.app();
  app.start();
  return 0;
}