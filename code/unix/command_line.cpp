#include <cstdlib>
#include <cstring>

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

  return cmdline;
}
