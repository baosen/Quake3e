#include "fpu.h"

extern "C" void IN_Frame();
extern "C" void Com_Frame(const bool noDelay);
extern "C" bool CL_NoDelay();

void DoGameLoop() {
  for (;;) {
#ifdef __linux__
    Sys_ConfigureFPU();
#endif
    IN_Frame();
    Com_Frame(CL_NoDelay());
  }
}