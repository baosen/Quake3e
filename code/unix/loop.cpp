#include "fpu.h"

extern "C" void IN_Frame();
void Com_Frame(const bool noDelay) {}
bool CL_NoDelay() { return false; }

void DoGameLoop() {
  for (;;) {
#ifdef __linux__
    Sys_ConfigureFPU();
#endif
    IN_Frame();
    Com_Frame(CL_NoDelay());
  }
}