#include "init.h"

void Sys_ConfigureFPU() {}
void IN_Frame() {}
void Com_Frame(const bool noDelay) {}
bool CL_NoDelay() { return false; }

void RunGame(const int argc, const char *argv[]) {
  InitializeGame(argc, argv);
  extern void DoGameLoop();
  DoGameLoop(); // hidden temporal coupling with function above.
}

void DoGameLoop() {
  for (;;) {
#ifdef __linux__
    Sys_ConfigureFPU();
#endif
    IN_Frame();
    Com_Frame(CL_NoDelay());
  }
}