#include "command_line.h"
#include "time.h"

void Com_Init(char *s) {}
void NET_Init() {}

void InitializeGame(const int argc, const char *argv[]) {
  InitTimeBase();
  Com_Init(ReadCommandLine(argc, argv));
  NET_Init();
}