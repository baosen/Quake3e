extern "C" void ActivateMouseUnlessUserWantsItDeactivated();
extern "C" void ComputeNextFrame();

void DoGameLoop() {
  for (;;) {
    ActivateMouseUnlessUserWantsItDeactivated();
    ComputeNextFrame();
  }
}