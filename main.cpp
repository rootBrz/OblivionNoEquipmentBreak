#include "main.hpp"
#include "MinHook.h"
#include "utils.h"
#include <minwindef.h>

extern "C"
{
  void *originalFunc = nullptr;
  void changeDurabilityDamage();
  float durabilityDamageAdj(float value)
  {
    return value * DURABILITY_MULTIPLIER;
  }
};

DWORD WINAPI InitThread(LPVOID lpParam)
{
  constexpr char pattern[]{"F3 0F 10 0D ?? ?? ?? ?? 44 0F 28 C8 F3 45 0F 5C C8 41 0F 2F C9"};
  uintptr_t absAddr{FindPattern(pattern, sizeof(pattern)) + 0xC};
  SaveAddressToFile(absAddr);
  DURABILITY_MULTIPLIER = ReadFloatIniSetting("DurabilityMultiplier");

  LogToFile("Durability multiplier: %f", DURABILITY_MULTIPLIER);
  MH_Initialize();
  MH_CreateHook((LPVOID)absAddr, (LPVOID)changeDurabilityDamage, (LPVOID *)&originalFunc);
  MH_EnableHook((LPVOID)absAddr);

  return true;
}