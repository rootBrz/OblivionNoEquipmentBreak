#pragma once

#include "OBSE/obse64/PluginAPI.h"
#include <minwindef.h>

inline constexpr const char *LOG_NAME = "disableequipbreakage.log";
inline constexpr const char *INI_NAME = "equipbreakage.ini";
inline OBSEMessagingInterface *OBSE_MESSAGE = nullptr;
inline PluginHandle PLUGIN_HANDLE = kPluginHandle_Invalid;
inline HMODULE DLL_HANDLE = nullptr;

unsigned __stdcall InitThread(void *param);