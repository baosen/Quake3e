/*
===========================================================================
Copyright (C) 1999-2005 Id Software, Inc.

This file is part of Quake III Arena source code.

Quake III Arena source code is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

Quake III Arena source code is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Quake III Arena source code; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================
*/
#include "code/qcommon/q_shared.h"
#include "code/qcommon/qcommon.h"
#include "code/renderercommon/tr_public.h"
#ifndef DEDICATED
#include "code/client/client.h"
#endif
#include <ctype.h>
#include <dlfcn.h>
#include <errno.h>
#include <fcntl.h>
#include <libgen.h> // dirname
#include <limits.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <termios.h>
#include <unistd.h>
#if defined(__linux__) && defined(__GLIBC__)
#include <fpu_control.h> // force dumps on divide by zero.
#endif

extern void HandleEvents(void);

unsigned sys_frame_time;

qboolean stdin_active = qfalse;
int stdin_flags = 0;

// =============================================================
// tty console variables
// =============================================================

typedef enum { TTY_ENABLED, TTY_DISABLED, TTY_ERROR } tty_err;

// general flag to tell about tty console mode
static qboolean ttycon_on = qfalse;

// when printing general stuff to stdout stderr (Sys_Printf)
//   we need to disable the tty console stuff
// this increments so we can recursively disable
static int ttycon_hide = 0;

static field_t tty_con;

qboolean Sys_LowPhysicalMemory(void) { return qfalse; }

void Sys_BeginProfiling(void) {}

// never exit without calling this, or your terminal will be left in a pretty
// bad state.
void Sys_ConsoleInputShutdown(void) {
  // Restore blocking to stdin reads
  if (stdin_active) {
    fcntl(STDIN_FILENO, F_SETFL, stdin_flags);
  }

  Com_Memset(&tty_con, 0, sizeof(tty_con));

  stdin_active = qfalse;
  ttycon_on = qfalse;

  ttycon_hide = 0;
}

// single exit point (regular exit or in case of signal fault)
void Sys_Exit(int code) __attribute((noreturn));
void Sys_Exit(int code) {
  Sys_ConsoleInputShutdown();

#ifdef NDEBUG // regular behavior
  // We can't do this
  //  as long as GL DLL's keep installing with atexit...
  // exit(ex);
  _exit(code);
#else
  // Give me a backtrace on error exits.
  assert(code == 0);
  exit(code);
#endif
}

void Sys_Quit(void) {
#ifndef DEDICATED
  CL_Shutdown("", qtrue);
#endif
  Sys_Exit(0);
}

void Sys_Init(void) { Cvar_Set("arch", OS_STRING " " ARCH_STRING); }

void Sys_Error(const char *format, ...) {
  va_list argptr;
  char text[1024];

  // change stdin to non blocking
  // NOTE TTimo not sure how well that goes with tty console mode
  if (stdin_active) {
    fcntl(STDIN_FILENO, F_SETFL, stdin_flags);
  }

  va_start(argptr, format);
  Q_vsnprintf(text, sizeof(text), format, argptr);
  va_end(argptr);

#ifndef DEDICATED
  CL_Shutdown(text, qtrue);
#endif

  fprintf(stderr, "Sys_Error: %s\n", text);

  Sys_Exit(1);
}

void floating_point_exception_handler(int whatever) {
  signal(SIGFPE, floating_point_exception_handler);
}

// Platform-dependent event handling.
void Sys_SendKeyEvents(void) {
#ifndef DEDICATED
  HandleEvents();
#endif
}

// Block execution for milliseconds or until input is received.
void Sys_Sleep(int msec) {
  struct timeval timeout;
  fd_set fdset;
  int res;

  if (msec == 0)
    return;

  if (msec < 0) {
    // special case: wait for console input or network packet
    if (stdin_active) {
      msec = 300;
      do {
        FD_ZERO(&fdset);
        FD_SET(STDIN_FILENO, &fdset);
        timeout.tv_sec = msec / 1000;
        timeout.tv_usec = (msec % 1000) * 1000;
        res = select(STDIN_FILENO + 1, &fdset, NULL, NULL, &timeout);
      } while (res == 0 && NET_Sleep(10 * 1000));
    } else {
      // can happen only if no map loaded
      // which means we totally stuck as stdin is also disabled :P
      // usleep( 300 * 1000 );
      while (NET_Sleep(3000 * 1000))
        ;
    }
    return;
  }

  if (com_dedicated->integer && stdin_active) {
    FD_ZERO(&fdset);
    FD_SET(STDIN_FILENO, &fdset);
    timeout.tv_sec = msec / 1000;
    timeout.tv_usec = (msec % 1000) * 1000;
    select(STDIN_FILENO + 1, &fdset, NULL, NULL, &timeout);
  } else {
    usleep(msec * 1000);
  }
}

void Sys_Print(const char *msg) { fputs(msg, stderr); }

void QDECL Sys_SetStatus(const char *format, ...) {}
