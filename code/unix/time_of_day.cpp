#include "time_of_day.h"

timeval GetCurrentTimeOfDay() {
  timeval tp;
  gettimeofday(&tp, nullptr);
  return tp;
}
