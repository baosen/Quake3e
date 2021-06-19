#include <cstdio>

extern "C" void ActivateMouseUnlessUserWantsItDeactivated();
extern "C" void Com_Frame(const bool);
extern "C" bool CL_NoDelay();

void DoGameLoop() {
  for (;;) {
    ActivateMouseUnlessUserWantsItDeactivated();
    Com_Frame(CL_NoDelay());
  }
}