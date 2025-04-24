#include <cstdio>
#include <libloaderapi.h>
#include <minwindef.h>
#include <psapi.h>
#include <windows.h>

uintptr_t FindPattern(const char *pat) {
  HMODULE hMod = GetModuleHandle(NULL);
  if (!hMod)
    return 0;

  unsigned char *base = (unsigned char *)hMod;
  MODULEINFO modInfo;
  if (!GetModuleInformation(GetCurrentProcess(), hMod, &modInfo,
                            sizeof(modInfo)))
    return 0;

  size_t imageSize = modInfo.SizeOfImage;
  if (!imageSize)
    return 0;

  size_t maxTokens = 0;
  for (const char *p = pat; *p; ++p)
    if (*p == ' ')
      maxTokens++;
  maxTokens += 1;

  unsigned char *bytes = (unsigned char *)malloc(maxTokens);
  char *mask = (char *)malloc(maxTokens); // 1 = compare, 0 = wildcard
  if (!bytes || !mask) {
    free(bytes);
    free(mask);
    return 0;
  }

  size_t idx = 0;
  const char *cur = pat;
  while (*cur) {
    if (*cur == ' ') {
      cur++;
      continue;
    }
    if (cur[0] == '?' && (cur[1] == '?' || cur[1] == ' ' || cur[1] == '\0')) {
      bytes[idx] = 0x00;
      mask[idx] = 0;
      cur += (cur[1] == '?') ? 2 : 1;
    } else {
      unsigned int byteVal;
      if (sscanf(cur, "%2x", &byteVal) != 1)
        break;
      bytes[idx] = (unsigned char)byteVal;
      mask[idx] = 1;
      cur += 2;
    }
    idx++;
  }
  size_t patLen = idx;

  for (size_t i = 0; i + patLen <= imageSize; ++i) {
    int ok = 1;
    for (size_t j = 0; j < patLen; ++j) {
      if (mask[j] && base[i + j] != bytes[j]) {
        ok = 0;
        break;
      }
    }
    if (ok) {
      uintptr_t addr = (uintptr_t)(base + i);
      free(bytes);
      free(mask);
      return addr;
    }
  }

  free(bytes);
  free(mask);
  return 0;
}

void SaveAddressToFile(uintptr_t absAddr, const char *filename) {
  FILE *file = fopen(filename, "w");
  if (file) {
    fprintf(file, "0x%p\n", (void *)absAddr); // Print as a pointer
    fclose(file);
  } else {
    perror("Failed to open file");
  }
}

DWORD WINAPI InitThread(LPVOID lpParam) {
  constexpr char pattern[]{"48 8B D9 B2 2B E8 ?? ?? ?? ?? 48 85 C0 74 10"};
  uintptr_t absAddr = FindPattern(pattern) + 0x0D;
  SaveAddressToFile(absAddr, "address.txt");
  constexpr unsigned char patch[]{0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
                                  0x90, 0x90, 0x90, 0x90, 0x90, 0x90};
  memcpy((void *)absAddr, patch, sizeof(patch));

  return true;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call,
                      LPVOID lpReserved) {
  if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
    DisableThreadLibraryCalls(hModule);
    CreateThread(NULL, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(InitThread),
                 NULL, 0, NULL);
  }
  return TRUE;
}
