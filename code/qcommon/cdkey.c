#include "q_shared.h"

char cl_cdkey[34] = "";

qboolean Com_CDKeyValidate(const char *key, const char *checksum) {
  return qtrue;
}

void Com_ReadCDKey(const char *filename) {}

void Com_AppendCDKey(const char *filename) {}

#ifndef DEDICATED
static void Com_WriteCDKey(const char *filename, const char *ikey) {}
#endif