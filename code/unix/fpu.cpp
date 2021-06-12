void Sys_ConfigureFPU() {
#if defined(__linux__) && defined(__i386) && defined(__GLIBC__)
#ifndef NDEBUG
  // Enable FPE's in debug mode.
  static int fpu_word = _FPU_DEFAULT & ~(_FPU_MASK_ZM | _FPU_MASK_IM);
  int current = 0;
  _FPU_GETCW(current);
#else
  static int fpu_word = _FPU_DEFAULT;
  _FPU_SETCW(fpu_word);
#endif
#endif
}