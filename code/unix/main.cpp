#include "game.h"
#include <cstdlib>
#include <cstring>

int main(const int argc, const char *argv[]) {
  if (Sys_ParseArgs(argc, argv)) {
    return EXIT_FAILURE;
  }
  extern void InitializeGame(const int argc, const char *argv[]);
  InitializeGame(argc, argv);
  RunGame();
  return EXIT_SUCCESS; // never gets here.
}

void InitializeGame(const int argc, const char *argv[]) {
  Sys_Milliseconds();
  extern char *ReadCommandLine(const int argc, const char *argv[]);
  Com_Init(ReadCommandLine(argc, argv));
  NET_Init();
}

char *ReadCommandLine(const int argc, const char *argv[]) {
  size_t len = 1;
  int i = 1;
  for (; i < argc; i++) {
    len += strlen(argv[i]) + 1;
  }

  char *cmdline = (char *)malloc(len);
  *cmdline = '\0';
  for (int i = 1; i < argc; i++) {
    if (i > 1) {
      strcat(cmdline, " ");
    }
    strcat(cmdline, argv[i]);
  }

  const int MAX_CVAR_VALUE_STRING = 256;
  char con_title[MAX_CVAR_VALUE_STRING];
  int xpos, ypos;
  Com_EarlyParseCmdLine(cmdline, con_title, sizeof(con_title), &xpos, &ypos);
  return cmdline;
}