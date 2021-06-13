#include "time_of_day.h"
#include <cstdlib>

timeval GetCurrentTimeOfDay() {
  timeval time_value;
  if (gettimeofday(&time_value, nullptr) != 0) {
    exit(EXIT_FAILURE);
  }
  return time_value;
}
