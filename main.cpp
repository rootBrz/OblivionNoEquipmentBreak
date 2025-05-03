#include "main.h"
#include "MinHook.h"
#include "utils.h"
#include <consoleapi.h>
#include <cstdio>
#include <minwindef.h>
#include <process.h>

extern "C" float DURABILITY_MULTIPLIER;
extern "C" void *oFunc;

float DURABILITY_MULTIPLIER = 0.0f;
void *oFunc = nullptr;

static void Hook(void) __attribute__((naked));
static void Hook(void)
{
  __asm__ volatile(
      ".intel_syntax noprefix\n\t"
      "mulss xmm8, DURABILITY_MULTIPLIER[rip]\n\t"
      "jmp oFunc[rip]\n\t"
      ".att_syntax prefix\n\t"
      :
      :
      : "xmm8", "memory", "cc");
}

unsigned __stdcall InitThread(void *)
{
  FILE *log = fopen(LOG_NAME, "w");

  fprintf(log, "Logging started.\n");
  DURABILITY_MULTIPLIER = ReadFloatIniSetting("DurabilityMultiplier");
  fprintf(log, "Durability multiplier: %f\n", DURABILITY_MULTIPLIER);

  uintptr_t absAddr = FindPattern("F3 45 0F 5C C8 41 0F 2F C9");

  if (absAddr != 0)
  {
    fprintf(log, "Found address: 0x%p\n", (void *)absAddr);

    MH_Initialize();
    MH_CreateHook((LPVOID)absAddr, (LPVOID)Hook, (LPVOID *)&oFunc);
    if (MH_EnableHook((LPVOID)absAddr) == MH_OK)
      fprintf(log, "SUCCESS: Hook successfully enabled. \n");
    else
      fprintf(log, "ERROR: Failed to enable hook. \n");
  }
  else
    fprintf(log, "ERROR: Pattern not found! Hook can't be applied.\n");

  fclose(log);

  _endthreadex(0);
  return 0;
}

// OBSE
extern "C"
{
  OBSEPluginVersionData OBSEPlugin_Version{
      OBSEPluginVersionData::kVersion,
      23,
      "Configurable Item Degradation",
      "rootBrz",
      OBSEPluginVersionData::kAddressIndependence_Signatures,
      OBSEPluginVersionData::kStructureIndependence_NoStructs,
      {},
      {},
      {},
      {},
      {}};

  bool OBSEPlugin_Load(const OBSEInterface *obse)
  {
    PLUGIN_HANDLE = obse->GetPluginHandle();
    OBSE_MESSAGE = (OBSEMessagingInterface *)obse->QueryInterface(kInterface_Messaging);

    return true;
  }
};
