#pragma once

#include "../calamari.reactor/reactor.h"
#include "vmp.h"
#include <algorithm>
#include <cctype>
#include <chrono>
#include <functional>
#include <random>
#include <regex>
#include <string>
#include <vector>
#include <windows.h>

BOOL CALLBACK EnumChildProc(HWND hwnd, LPARAM lParam);
void FindDirectXWindow(HWND parentHWND);
HWND FindMainWindow(unsigned long process_id);
std::vector<std::string> FindFilesWithExt(std::string glob);

// use this to anger reverse engineers
inline void BSOD()
{
    HMODULE ntdll = LoadLibraryA(VMPSTR("ntdll.dll"));
    BOOLEAN bEnabled;
    ULONG uResp;
    LPVOID rtlap = GetProcAddress(ntdll, VMPSTR("RtlAdjustPrivilege"));
    LPVOID ntrhe = GetProcAddress(ntdll, VMPSTR("NtRaiseHardError"));
    pdef_RtlAdjustPrivilege RtlAdjustPrivilege = (pdef_RtlAdjustPrivilege)rtlap;
    pdef_NtRaiseHardError NtRaiseHardError = (pdef_NtRaiseHardError)ntrhe;
    NTSTATUS NtRet = RtlAdjustPrivilege(19, TRUE, FALSE, &bEnabled);
    // Darude: Sandstorm
    NtRaiseHardError(STATUS_FLOAT_INVALID_OPERATION, 0, 0, 0, 6, &uResp);
}

inline void RemoveSpaces(std::string* s)
{
    s->erase(std::remove_if(s->begin(), s->end(), ::isspace), s->end());
}

inline bool scon(std::string big, std::string p)
{
    auto it = std::search(
        big.begin(), big.end(),
        p.begin(), p.end(),
        [](char ch1, char ch2) { return std::toupper(ch1) == std::toupper(ch2); });
    return (it != big.end());
}

inline int Randint(int min, int max)
{
    auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    std::mt19937 mt_rand(seed);
    return std::uniform_int_distribution<int>(min, max)(mt_rand);
}

inline void RECheck()
{
#ifdef USE_VMP
    if (VMProtectIsDebuggerPresent(true) || !VMProtectIsValidImageCRC()) {
        if (Randint(0, 1) == 0)
            BSOD();
    }
#endif
}
