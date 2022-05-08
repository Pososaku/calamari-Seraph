#pragma once
#include "../common/calamari_config.h"
#include <unordered_map>
#include <vector>
#include <string>
#include <Windows.h>
#include <winternl.h>
#include <fstream>
#include <mutex>
#include <shared_mutex>
#include <map>
#include "../calamari.public/plugin.h"
#include "../common/vmp.h"
#include "../common/MinHook.h"
#include "../common/SimpleIni.h"

typedef NTSTATUS(NTAPI *pdef_NtRaiseHardError)(NTSTATUS ErrorStatus, ULONG NumberOfParameters, ULONG UnicodeStringParameterMask, PULONG_PTR Parameters, ULONG ResponseOption, PULONG Response);
typedef NTSTATUS(NTAPI *pdef_RtlAdjustPrivilege)(ULONG Privilege, BOOLEAN Enable, BOOLEAN CurrentThread, PBOOLEAN Enabled);
typedef BOOL(WINAPI* EnumProcessModules)(HANDLE hProcess, HMODULE *lphModule, DWORD cb, LPDWORD lpcbNeeded);

typedef Plugin*(*InitializePlugin)();
typedef void(*KeyCall)(int key);

struct KeyState {
    std::string Name;
    Plugin* Plugin;
	int Key;
	bool State = false;
	KeyCall Call;
};

namespace Calamari::Reactor
{
    extern HMODULE ProcessThread;
    extern char ProcessName[256];
    extern HANDLE Process;
    extern bool IsDreamSeeker;

    extern std::ofstream LogStream;

    extern std::vector<Plugin*> Plugins;
    extern std::vector<KeyState*> Keybinds;
    extern std::vector<EventCall*> Events;
    extern std::unordered_map<std::string, void*> Hooks;

    extern bool HasChecked;

    extern std::string Username;

    extern std::string ByondPath;
    extern std::string Path;
    extern std::string PluginsPath;
    extern std::string ConfigPath;

    extern CSimpleIniA ConfigIni;

    extern int ToggleKey;
    extern int HideKey;
    extern std::string License;
    extern int Serial;

    extern bool GuiInteraction;
    extern bool HideGui;

    extern std::unordered_map<long, std::string> Mobs;
    extern std::unordered_map<long, std::string> Objects;
    extern std::shared_mutex MobsMutex;
    extern std::shared_mutex ObjsMutex;
    extern bool HasThreaded;


    void RegisterKey(int pKey, void* condition);

    void ToggleGUIInteraction();
    void ToggleHideGui();
    void RECheck();
    void Initialize(HMODULE MainThread);
    void PatchDRM();
    void CoreHook();
    void DSHook();
    void DXHook();
    void Unhook();
    void ParseSettings();
    void PollKeys();
    bool IsBlacklisted();
    void Log(std::string text);

    void MobScraperThread();
    void ObjScraperThread();
    void ResetCidCache();
}
