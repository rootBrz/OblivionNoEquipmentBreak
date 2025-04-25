#include "utils.h"
#include "main.hpp"
#include <cstdarg>
#include <cstdio>
#include <libloaderapi.h>
#include <minwindef.h>
#include <processthreadsapi.h>
#include <psapi.h>

// Return float based on ini setting
float ReadFloatIniSetting(const char *setting)
{
  FILE *iniSettings = fopen(INI_NAME, "r");
  if (!iniSettings)
    return 0.0f;

  char line[256];
  while (fgets(line, sizeof(line), iniSettings))
  {
    if (strncmp(line, setting, sizeof(setting) - 1))
      continue;

    char *equalSign = strchr(line, '=');
    if (equalSign)
      return strtof(equalSign + 1, NULL);
  }

  fclose(iniSettings);

  return 0.0f;
}

// Find memory address based on pattern
uintptr_t FindPattern(const char *pat, unsigned int size)
{
  Pattern patternStruct[size];

  HMODULE hMod = GetModuleHandle(NULL);
  unsigned char *base = (unsigned char *)hMod;
  MODULEINFO modInfo;
  GetModuleInformation(GetCurrentProcess(), hMod, &modInfo, sizeof(modInfo));
  size_t imageSize = modInfo.SizeOfImage;

  char idx = 0;
  while (*pat)
  {
    patternStruct[idx].byte = *pat == '?' ? 0x00 : (unsigned char)strtol(pat, NULL, 16);
    patternStruct[idx].mask = *pat == '?' ? 0 : 1;
    pat += 3;
    idx++;
  }

  for (size_t i = 0; i + size <= imageSize; ++i)
  {
    int patternFound = 1;
    for (unsigned int j = 0; j < size; ++j)
      if (patternStruct[j].mask && base[i + j] != patternStruct[j].byte)
      {
        patternFound = 0;
        break;
      }

    if (patternFound)
      return (uintptr_t)(base + i);
  }

  return 0;
}

// Start logging with writing found address
void SaveAddressToFile(uintptr_t absAddr)
{
  FILE *file = fopen(LOG_NAME, "w");
  if (file)
  {
    fprintf(file, "Found address: 0x%p\n", (void *)absAddr);
    fclose(file);
  }
}

void LogToFile(const char *format, ...)
{
  FILE *file = fopen(LOG_NAME, "a");
  if (file)
  {
    va_list args;
    va_start(args, format);

    vfprintf(file, format, args);

    va_end(args);
    fclose(file);
  }
}