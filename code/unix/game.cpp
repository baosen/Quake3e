void Sys_ConfigureFPU() {}
void IN_Frame() {}
void Com_Frame(const bool noDelay) {}
bool CL_NoDelay() { return false; }

void RunGame() {
  for (;;) {
#ifdef __linux__
    Sys_ConfigureFPU();
#endif
    IN_Frame();
    Com_Frame(CL_NoDelay());
  }
}