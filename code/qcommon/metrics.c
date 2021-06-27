#include "q_shared.h"
#include "qcommon.h"

extern cvar_t *com_speeds;
extern cvar_t *com_showtrace;

void DisplayMetricsIfEnabled() {
  if (com_speeds->integer) {
    extern void ReportTimingInformation();
    ReportTimingInformation();
  }
  if (com_showtrace->integer) {
    extern void TraceOptimizationTracking();
    TraceOptimizationTracking();
  }
}

static int com_frameNumber = 0;

void CountFrameNumber() {
  com_frameNumber++;
}

// These variables are used to profile the code:
int timeBeforeFirstEvents = 0;
int timeBeforeServer = 0;
int timeBeforeEvents = 0;
int timeBeforeClient = 0;
int timeAfter = 0;

void ReportTimingInformation() {
  int all = timeAfter - timeBeforeServer;
  int sv = timeBeforeEvents - timeBeforeServer;
  int ev = timeBeforeServer - timeBeforeFirstEvents + timeBeforeClient -
           timeBeforeEvents;
  int cl = timeAfter - timeBeforeClient;

  sv -= time_game;
  cl -= time_frontend + time_backend;

  Com_Printf("frame:%i all:%3i sv:%3i ev:%3i cl:%3i gm:%3i rf:%3i bk:%3i\n",
             com_frameNumber, all, sv, ev, cl, time_game, time_frontend,
             time_backend);
}

void TraceOptimizationTracking() {
  extern int c_traces, c_brush_traces, c_patch_traces, c_pointcontents;
  Com_Printf("%4i traces  (%ib %ip) %4i points\n", c_traces, c_brush_traces,
             c_patch_traces, c_pointcontents);
  c_traces = 0;
  c_brush_traces = 0;
  c_patch_traces = 0;
  c_pointcontents = 0;
}
