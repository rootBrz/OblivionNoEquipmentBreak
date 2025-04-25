#include "main.h"
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
  // If OBSE initialized, do not run from PROCESS_ATTACH
  if (lpParam && OBSE_MESSAGE)
    return true;

  constexpr char pattern[]{"F3 0F 10 0D ?? ?? ?? ?? 44 0F 28 C8 F3 45 0F 5C C8 41 0F 2F C9"};
  uintptr_t absAddr{FindPattern(pattern, sizeof(pattern) / 3) + 0xC};

  SaveAddressToFile(absAddr);
  DURABILITY_MULTIPLIER = ReadFloatIniSetting("DurabilityMultiplier");

  LogToFile("Durability multiplier: %f", DURABILITY_MULTIPLIER);
  MH_Initialize();
  MH_CreateHook((LPVOID)absAddr, (LPVOID)changeDurabilityDamage, (LPVOID *)&originalFunc);
  MH_EnableHook((LPVOID)absAddr);

  return true;
}

// OBSE
void MessageHandler(OBSEMessagingInterface::Message *msg)
{
  if (msg->type == OBSEMessagingInterface::kMessage_PostPostLoad)
    InitThread(nullptr);
}
extern "C"
{
  __declspec(dllexport) OBSEPluginVersionData OBSEPlugin_Version =
      {
          OBSEPluginVersionData::kVersion,

          20,
          "Configurable Item Degradation",
          "rootBrz",

          OBSEPluginVersionData::kAddressIndependence_Signatures,
          OBSEPluginVersionData::kStructureIndependence_NoStructs};

  __declspec(dllexport) bool OBSEPlugin_Load(const OBSEInterface *obse)
  {
    PLUGIN_HANDLE = obse->GetPluginHandle();
    OBSE_MESSAGE = (OBSEMessagingInterface *)obse->QueryInterface(kInterface_Messaging);
    OBSE_MESSAGE->RegisterListener(PLUGIN_HANDLE, "OBSE", MessageHandler);

    return true;
  }
};
