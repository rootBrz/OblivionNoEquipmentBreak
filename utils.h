#pragma once

#include <cstdint>

typedef struct
{
  unsigned char byte;
  unsigned char mask;
} Pattern;

uintptr_t FindPattern(const char *pat);
float ReadFloatIniSetting(const char *setting);