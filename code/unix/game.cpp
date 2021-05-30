#include "init.h"
#include "loop.h"

void RunGame(const int argc, const char *argv[]) {
  InitializeGame(argc, argv);
  DoGameLoop(); // hidden temporal coupling with function above.
}
