#include "utils.h"
#include "main.h"
#include <cmath>
#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <libloaderapi.h>
#include <processthreadsapi.h>
#include <psapi.h>

// Return float based on ini setting
float ReadFloatIniSetting(const char *setting)
{
  char *path = (char *)malloc(MAX_PATH);

  DWORD len = GetModuleFileName(DLL_HANDLE, path, MAX_PATH);
  for (DWORD i = len; i > 0; --i)
    if (path[i - 1] == '\\' || path[i - 1] == '/')
    {
      path[i] = '\0';
      break;
    }
  snprintf(path + strlen(path), MAX_PATH - strlen(path), "\\%s", INI_NAME);

  FILE *iniSettings = fopen(path, "r");
  free(path);

  if (!iniSettings)
    return 0.0f;

  char line[1024];
  while (fgets(line, sizeof(line), iniSettings))
  {
    if (strncmp(line, setting, strlen(setting)))
      continue;

    char *equalSign = strchr(line, '=');
    if (equalSign)
      return strtof(equalSign + 1, NULL);
  }

  fclose(iniSettings);
  return 0.0f;
}

// Find memory address based on pattern
uintptr_t FindPattern(const char *pat)
{
  size_t patLen = (strlen(pat) + 1) / 3;
  unsigned char *patBytes = (unsigned char *)_alloca(patLen);
  unsigned char *mask = (unsigned char *)_alloca(patLen);

  for (size_t i = 0; i < patLen; ++i)
  {
    const char *p = pat + i * 3;
    bool isWildcard = (*p == '?');

    patBytes[i] = isWildcard ? 0x00 : (unsigned char)strtoul(p, nullptr, 16);
    mask[i] = isWildcard ? 0 : 1;
  }

  HMODULE hMod = GetModuleHandle(NULL);
  MODULEINFO mi;
  GetModuleInformation(GetCurrentProcess(), hMod, &mi, sizeof(mi));
  unsigned char *base = (unsigned char *)hMod;
  size_t max = mi.SizeOfImage - patLen;

  for (size_t i = 0; i <= max; i++)
  {
    if (base[i] != patBytes[0])
      continue;

    bool found = true;

    for (size_t j = 0; j < patLen; j++)
      if (mask[j] && base[i + j] != patBytes[j])
      {
        found = false;
        break;
      }

    if (found)
      return (uintptr_t)base + i;
  }

  return 0;
}
