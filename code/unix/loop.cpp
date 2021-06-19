#include <cstdio>

extern "C" void ActivateMouseUnlessUserWantsItDeactivated();
extern "C" void ComputeNextFrame(const bool);
extern "C" bool CL_NoDelay();

void DoGameLoop() {
  for (;;) {
    ActivateMouseUnlessUserWantsItDeactivated();
    ComputeNextFrame(CL_NoDelay());
  }
}