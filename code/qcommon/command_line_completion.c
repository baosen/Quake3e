#include "q_shared.h"
#include "qcommon.h"

void Field_Clear(field_t *edit) {
  memset(edit->buffer, 0, sizeof(edit->buffer));
  edit->cursor = 0;
  edit->scroll = 0;
}

static const char *completionString;
static char shortestMatch[MAX_TOKEN_CHARS];
static int matchCount;
// field we are working on, passed to Field_AutoComplete(&g_consoleCommand for
// instance)
static field_t *completionField;

static void FindMatches(const char *s) {
  int i, n;

  if (Q_stricmpn(s, completionString, strlen(completionString))) {
    return;
  }
  matchCount++;
  if (matchCount == 1) {
    Q_strncpyz(shortestMatch, s, sizeof(shortestMatch));
    return;
  }

  n = (int)strlen(s);
  // cut shortestMatch to the amount common with s
  for (i = 0; shortestMatch[i]; i++) {
    if (i >= n) {
      shortestMatch[i] = '\0';
      break;
    }

    if (tolower(shortestMatch[i]) != tolower(s[i])) {
      shortestMatch[i] = '\0';
    }
  }
}

static void PrintMatches(const char *s) {
  if (!Q_stricmpn(s, shortestMatch, strlen(shortestMatch))) {
    Com_Printf("    %s\n", s);
  }
}

static void PrintCvarMatches(const char *s) {
  char value[TRUNCATE_LENGTH];

  if (!Q_stricmpn(s, shortestMatch, strlen(shortestMatch))) {
    Com_TruncateLongString(value, Cvar_VariableString(s));
    Com_Printf("    %s = \"%s\"\n", s, value);
  }
}

static char *Field_FindFirstSeparator(char *s) {
  char c;
  while ((c = *s) != '\0') {
    if (c == ';')
      return s;
    s++;
  }
  return NULL;
}

static void Field_AddSpace(void) {
  size_t len = strlen(completionField->buffer);
  if (len && len < sizeof(completionField->buffer) - 1 &&
      completionField->buffer[len - 1] != ' ') {
    memcpy(completionField->buffer + len, " ", 2);
    completionField->cursor = (int)(len + 1);
  }
}

static qboolean Field_Complete(void) {
  int completionOffset;

  if (matchCount == 0)
    return qtrue;

  completionOffset = strlen(completionField->buffer) - strlen(completionString);

  Q_strncpyz(&completionField->buffer[completionOffset], shortestMatch,
             sizeof(completionField->buffer) - completionOffset);

  completionField->cursor = strlen(completionField->buffer);

  if (matchCount == 1) {
    Field_AddSpace();
    return qtrue;
  }

  Com_Printf("]%s\n", completionField->buffer);

  return qfalse;
}

void Field_CompleteKeyname(void) {
  matchCount = 0;
  shortestMatch[0] = '\0';

  Key_KeynameCompletion(FindMatches);

  if (!Field_Complete())
    Key_KeynameCompletion(PrintMatches);
}

void Field_CompleteKeyBind(int key) {
  const char *value;
  int vlen;
  int blen;

  value = Key_GetBinding(key);
  if (value == NULL || *value == '\0')
    return;

  blen = (int)strlen(completionField->buffer);
  vlen = (int)strlen(value);

  if (Field_FindFirstSeparator((char *)value)) {
    value = va("\"%s\"", value);
    vlen += 2;
  }

  if (vlen + blen > sizeof(completionField->buffer) - 1) {
    return;
  }

  memcpy(completionField->buffer + blen, value, vlen + 1);
  completionField->cursor = blen + vlen;

  Field_AddSpace();
}

static void Field_CompleteCvarValue(const char *value, const char *current) {
  int vlen;
  int blen;

  if (*value == '\0')
    return;

  blen = (int)strlen(completionField->buffer);
  vlen = (int)strlen(value);

  if (*current != '\0') {
    return;
  }

  if (Field_FindFirstSeparator((char *)value)) {
    value = va("\"%s\"", value);
    vlen += 2;
  }

  if (vlen + blen > sizeof(completionField->buffer) - 1) {
    return;
  }

  if (blen > 1) {
    if (completionField->buffer[blen - 1] == '"' &&
        completionField->buffer[blen - 2] == ' ') {
      completionField->buffer[blen--] = '\0'; // strip starting quote
    }
  }

  memcpy(completionField->buffer + blen, value, vlen + 1);
  completionField->cursor = vlen + blen;

  Field_AddSpace();
}

void Field_CompleteFilename(const char *dir, const char *ext, qboolean stripExt,
                            int flags) {
  matchCount = 0;
  shortestMatch[0] = '\0';

  FS_FilenameCompletion(dir, ext, stripExt, FindMatches, flags);

  if (!Field_Complete())
    FS_FilenameCompletion(dir, ext, stripExt, PrintMatches, flags);
}

void Field_CompleteCommand(char *cmd, qboolean doCommands, qboolean doCvars) {
  int completionArgument;

  // Skip leading whitespace and quotes
  cmd = Com_SkipCharset(cmd, " \"");

  Cmd_TokenizeStringIgnoreQuotes(cmd);
  completionArgument = Cmd_Argc();

  // If there is trailing whitespace on the cmd
  if (*(cmd + strlen(cmd) - 1) == ' ') {
    completionString = "";
    completionArgument++;
  } else
    completionString = Cmd_Argv(completionArgument - 1);

#ifndef DEDICATED
  // Unconditionally add a '\' to the start of the buffer
  if (completionField->buffer[0] && completionField->buffer[0] != '\\') {
    if (completionField->buffer[0] != '/') {
      // Buffer is full, refuse to complete
      if (strlen(completionField->buffer) + 1 >=
          sizeof(completionField->buffer))
        return;

      memmove(&completionField->buffer[1], &completionField->buffer[0],
              strlen(completionField->buffer) + 1);
      completionField->cursor++;
    }

    completionField->buffer[0] = '\\';
  }
#endif

  if (completionArgument > 1) {
    const char *baseCmd = Cmd_Argv(0);
    char *p;

#ifndef DEDICATED
    // This should always be true
    if (baseCmd[0] == '\\' || baseCmd[0] == '/')
      baseCmd++;
#endif

    if ((p = Field_FindFirstSeparator(cmd)) != NULL) {
      Field_CompleteCommand(p + 1, qtrue, qtrue); // Compound command
    } else {
      qboolean argumentCompleted =
          Cmd_CompleteArgument(baseCmd, cmd, completionArgument);
      if ((matchCount == 1 || argumentCompleted) && doCvars) {
        if (cmd[0] == '/' || cmd[0] == '\\')
          cmd++;
        Cmd_TokenizeString(cmd);
        Field_CompleteCvarValue(Cvar_VariableString(Cmd_Argv(0)), Cmd_Argv(1));
      }
    }
  } else {
    if (completionString[0] == '\\' || completionString[0] == '/')
      completionString++;

    matchCount = 0;
    shortestMatch[0] = '\0';

    if (completionString[0] == '\0') {
      return;
    }

    if (doCommands)
      Cmd_CommandCompletion(FindMatches);

    if (doCvars)
      Cvar_CommandCompletion(FindMatches);

    if (!Field_Complete()) {
      // run through again, printing matches
      if (doCommands)
        Cmd_CommandCompletion(PrintMatches);

      if (doCvars)
        Cvar_CommandCompletion(PrintCvarMatches);
    }
  }
}

// Perform Tab expansion
void Field_AutoComplete(field_t *field) {
  completionField = field;

  Field_CompleteCommand(completionField->buffer, qtrue, qtrue);
}

// fills string array with len random bytes, preferably from the OS randomizer
void Com_RandomBytes(byte *string, int len) {
  int i;

  if (Sys_RandomBytes(string, len))
    return;

  Com_Printf(S_COLOR_YELLOW "Com_RandomBytes: using weak randomization\n");
  srand(time(NULL));
  for (i = 0; i < len; i++)
    string[i] = (unsigned char)(rand() % 256);
}

static qboolean IsStringGreaterThan(const char *s0, const char *s1) {
  int i;

  int l0 = strlen(s0);
  int l1 = strlen(s1);

  if (l1 < l0) {
    l0 = l1;
  }

  for (i = 0; i < l0; i++) {
    if (s1[i] > s0[i]) {
      return qtrue;
    }
    if (s1[i] < s0[i]) {
      return qfalse;
    }
  }
  return qfalse;
}

static void Com_SortList(char **list, int n) {
  const char *m;
  char *temp;
  int i, j;
  i = 0;
  j = n;
  m = list[n >> 1];
  do {
    while (strcmp(list[i], m) < 0)
      i++;
    while (strcmp(list[j], m) > 0)
      j--;
    if (i <= j) {
      temp = list[i];
      list[i] = list[j];
      list[j] = temp;
      i++;
      j--;
    }
  } while (i <= j);
  if (j > 0)
    Com_SortList(list, j);
  if (n > i)
    Com_SortList(list + i, n - i);
}

void Com_SortFileList(char **list, int nfiles, int fastSort) {
  if (nfiles > 1 && fastSort) {
    Com_SortList(list, nfiles - 1);
  } else // defrag mod demo UI can't handle _properly_ sorted directories
  {
    int i, flag;
    do {
      flag = 0;
      for (i = 1; i < nfiles; i++) {
        if (IsStringGreaterThan(list[i - 1], list[i])) {
          char *temp = list[i];
          list[i] = list[i - 1];
          list[i - 1] = temp;
          flag = 1;
        }
      }
    } while (flag);
  }
}