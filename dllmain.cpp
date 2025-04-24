#include <cstdio>
#include <libloaderapi.h>
#include <memoryapi.h>
#include <minwindef.h>
#include <processthreadsapi.h>
#include <psapi.h>

typedef struct
{
  unsigned char byte;
  unsigned char mask;
} Pattern;

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

void SaveAddressToFile(uintptr_t absAddr, const char *filename)
{
  FILE *file = fopen(filename, "w");
  if (file)
  {
    fprintf(file, "0x%p\n", (void *)absAddr);
    fclose(file);
  }
}

DWORD WINAPI InitThread(LPVOID lpParam)
{
  constexpr char pattern[]{"48 8B D9 B2 2B E8 ?? ?? ?? ?? 48 85 C0 74 10"};
  uintptr_t absAddr{FindPattern(pattern, sizeof(pattern)) + 0x0D};
  SaveAddressToFile(absAddr, "disableequipbreakage.txt");
  constexpr unsigned char patch[]{0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90};
  WriteProcessMemory(GetCurrentProcess(), (LPVOID)absAddr, patch, sizeof(patch), NULL);
  return true;
}

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
