#include <sys/time.h>

static timeval GetTimeOfDay();

/* base time in seconds, that's our origin
   timeval:tv_sec is an int:
   assuming this wraps every 0x7fffffff - ~68 years since the Epoch (1970) -
   we're safe till 2038 using unsigned long data type to work right with
   Sys_XTimeToSysTime */
static unsigned long sys_timeBase = 0;
/* current time in ms, using sys_timeBase as origin
   NOTE: sys_timeBase*1000 + curtime -> ms since the Epoch
     0x7fffffff ms - ~24 days
   although timeval:tv_usec is an int, I'm not sure wether it is actually used
   as an unsigned int (which would affect the wrap period) */

void InitTimeBase() {
  auto tp = GetTimeOfDay();
  sys_timeBase = tp.tv_sec;
}

// Returns the time elapsed in milliseconds.
int Sys_Milliseconds() {
  auto tp = GetTimeOfDay();
  if (!sys_timeBase) { // set time base on first call.
    sys_timeBase = tp.tv_sec;
    return tp.tv_usec / 1000;
  }
  return (tp.tv_sec - sys_timeBase) * 1000 + tp.tv_usec / 1000;
}

static timeval GetTimeOfDay() {
  timeval tp;
  gettimeofday(&tp, nullptr);
  return tp;
}