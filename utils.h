#pragma once

#include <cstdint>

typedef struct
{
  unsigned char byte;
  unsigned char mask;
} Pattern;

uintptr_t FindPattern(const char *pat, unsigned int size);
void SaveAddressToFile(uintptr_t absAddr);
void LogToFile(const char *format, ...);
float ReadFloatIniSetting(const char *setting);