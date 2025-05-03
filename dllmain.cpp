#include "main.h"
#include <libloaderapi.h>
#include <process.h>

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call,
                      LPVOID)
{
  if (ul_reason_for_call == DLL_PROCESS_ATTACH)
  {
    DLL_HANDLE = hModule;
    DisableThreadLibraryCalls(hModule);
    _beginthreadex(NULL, 0, InitThread, NULL, 0, NULL);
  }
  return TRUE;
}
