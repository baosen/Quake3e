#include "q_shared.h"
#include "qcommon.h"

int CPU_Flags = 0;

#if (idx64 || id386)

#if defined _MSC_VER
#include <intrin.h>
static void CPUID(int func, unsigned int *regs) { __cpuid(regs, func); }

#else // clang/gcc/mingw

static void CPUID(int func, unsigned int *regs) {
  __asm__ __volatile__("cpuid"
                       : "=a"(regs[0]), "=b"(regs[1]), "=c"(regs[2]),
                         "=d"(regs[3])
                       : "a"(func));
}

#endif // clang/gcc/mingw

void Sys_GetProcessorId(char *vendor) {
  unsigned int regs[4];

  // setup initial features
#if idx64
  CPU_Flags |= CPU_SSE | CPU_SSE2 | CPU_FCOM;
#else
  CPU_Flags = 0;
#endif

  // get CPU feature bits
  CPUID(1, regs);

  // bit 15 of EDX denotes CMOV/FCMOV/FCOMI existence
  if (regs[3] & (1 << 15))
    CPU_Flags |= CPU_FCOM;

  // bit 23 of EDX denotes MMX existence
  if (regs[3] & (1 << 23))
    CPU_Flags |= CPU_MMX;

  // bit 25 of EDX denotes SSE existence
  if (regs[3] & (1 << 25))
    CPU_Flags |= CPU_SSE;

  // bit 26 of EDX denotes SSE2 existence
  if (regs[3] & (1 << 26))
    CPU_Flags |= CPU_SSE2;

  // bit 19 of ECX denotes SSE41 existence
  if (regs[2] & (1 << 19))
    CPU_Flags |= CPU_SSE41;

  if (vendor) {
    int print_flags = CPU_Flags;
#if idx64
    strcpy(vendor, "64-bit ");
    vendor += strlen(vendor);
    // do not print default 64-bit features in 32-bit mode
    print_flags &= ~(CPU_FCOM | CPU_MMX | CPU_SSE | CPU_SSE2);
#else
    vendor[0] = '\0';
#endif
    // get CPU vendor string
    CPUID(0, regs);
    memcpy(vendor + 0, (char *)&regs[1], 4);
    memcpy(vendor + 4, (char *)&regs[3], 4);
    memcpy(vendor + 8, (char *)&regs[2], 4);
    vendor[12] = '\0';
    vendor += 12;

    if (print_flags) {
      // print features
      strcat(vendor, " w/");
      if (print_flags & CPU_FCOM)
        strcat(vendor, " CMOV");
      if (print_flags & CPU_MMX)
        strcat(vendor, " MMX");
      if (print_flags & CPU_SSE)
        strcat(vendor, " SSE");
      if (print_flags & CPU_SSE2)
        strcat(vendor, " SSE2");
      if (print_flags & CPU_SSE41)
        strcat(vendor, " SSE4.1");
    }
  }
}

#else // non-x86

#ifdef _WIN32

static void Sys_GetProcessorId(char *vendor) {
  Com_sprintf(vendor, 100, "%s", ARCH_STRING);
}

#else // not _WIN32

#if arm32
#include <asm/hwcap.h>
#include <sys/auxv.h>
#endif

static void Sys_GetProcessorId(char *vendor) {
#if arm32
  const char *platform;
  long hwcaps;
  CPU_Flags = 0;

  platform = (const char *)getauxval(AT_PLATFORM);

  if (!platform || *platform == '\0') {
    platform = "(unknown)";
  }

  if (platform[0] == 'v' || platform[0] == 'V') {
    if (atoi(platform + 1) >= 7) {
      CPU_Flags |= CPU_ARMv7;
    }
  }

  Com_sprintf(vendor, 100, "ARM %s", platform);
  hwcaps = getauxval(AT_HWCAP);
  if (hwcaps & (HWCAP_IDIVA | HWCAP_VFPv3)) {
    strcat(vendor, " /w");

    if (hwcaps & HWCAP_IDIVA) {
      CPU_Flags |= CPU_IDIVA;
      strcat(vendor, " IDIVA");
    }

    if (hwcaps & HWCAP_VFPv3) {
      CPU_Flags |= CPU_VFPv3;
      strcat(vendor, " VFPv3");
    }

    if ((CPU_Flags & (CPU_ARMv7 | CPU_VFPv3)) == (CPU_ARMv7 | CPU_VFPv3)) {
      strcat(vendor, " QVM-bytecode");
    }
  }
#else
  CPU_Flags = 0;
#if arm64
  Com_sprintf(vendor, 100, "ARM %s", ARCH_STRING);
#else
  Com_sprintf(vendor, 128, "%s %s", ARCH_STRING,
              (const char *)getauxval(AT_PLATFORM));
#endif
#endif
}

#endif // !_WIN32

#endif // non-x86
