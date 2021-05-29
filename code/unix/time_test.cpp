#include "time.h"
#include <cassert>
#include <iostream>

int main() {
  assert(Sys_Milliseconds() > 0);
  return EXIT_SUCCESS;
}