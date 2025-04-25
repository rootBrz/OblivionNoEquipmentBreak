#pragma once

#include <minwindef.h>

inline float DURABILITY_MULTIPLIER = 0.0f;
inline constexpr const char *LOG_NAME = "disableequipbreakage.log";
inline constexpr const char *INI_NAME = "equipbreakage.ini";

DWORD WINAPI InitThread(LPVOID lpParam);