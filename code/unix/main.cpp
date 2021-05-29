#include "game.h"
#include "init.h"
#include <cstdlib>

int main(const int argc, const char *argv[]) {
  InitializeGame(argc, argv);
  RunGame();
  return EXIT_SUCCESS; // never gets here.
}
