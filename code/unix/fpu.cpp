void Sys_ConfigureFPU(void) // bk001213 - divide by zero
{
#ifdef __linux__
#ifdef __i386
#ifdef __GLIBC__
#ifndef NDEBUG
  // Enable FPE's in debug mode.
  static int fpu_word = _FPU_DEFAULT & ~(_FPU_MASK_ZM | _FPU_MASK_IM);
  int current = 0;
  _FPU_GETCW(current);
#else  // NDEBUG
  static int fpu_word = _FPU_DEFAULT;
  _FPU_SETCW(fpu_word);
#endif // NDEBUG
#endif // __GLIBC__
#endif // __i386
#endif // __linux
}