#include "main.hpp"
#include <libloaderapi.h>
#include <processthreadsapi.h>

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call,
                      LPVOID lpReserved)
{
  if (ul_reason_for_call == DLL_PROCESS_ATTACH)
  {
    DisableThreadLibraryCalls(hModule);
    CreateThread(NULL, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(InitThread),
                 NULL, 0, NULL);
  }
  return TRUE;
}
