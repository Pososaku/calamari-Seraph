#include "reactor.h"
#include "../calamari.public/byond.h"
#include "../common/Pocket.h"
#include "../common/util.h"
#include "hooks/hooks.h"
#include "kiero/kiero.h"
#include <Shlwapi.h>
#include <algorithm>
#include <comutil.h>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <locale>
#include <regex>
#include <shellapi.h>
#include <shlobj.h>
#include <sstream>
#include <synchapi.h>

// i am tired and this is doin't care
#ifdef PACKET_DEBUGGER
static void* origidecxode = NULL;
#endif

HMODULE Calamari::Reactor::ProcessThread = NULL;
char Calamari::Reactor::ProcessName[256];
HANDLE Calamari::Reactor::Process = NULL;
bool Calamari::Reactor::IsDreamSeeker = NULL;
std::string Calamari::Reactor::ByondPath;
std::string Calamari::Reactor::Path;
std::string Calamari::Reactor::ConfigPath;
std::string Calamari::Reactor::PluginsPath;

std::string Calamari::Reactor::Username;

std::ofstream Calamari::Reactor::LogStream;

std::vector<Plugin*> Calamari::Reactor::Plugins;
std::vector<KeyState*> Calamari::Reactor::Keybinds;
std::vector<struct EventCall*> Calamari::Reactor::Events;
std::unordered_map<std::string, void*> Calamari::Reactor::Hooks;

bool Calamari::Reactor::HasChecked = false;

CSimpleIniA Calamari::Reactor::ConfigIni;

bool Calamari::Reactor::GuiInteraction = false;
bool Calamari::Reactor::HideGui = false;

std::string Calamari::Reactor::License;
int Calamari::Reactor::ToggleKey = VK_INSERT;
int Calamari::Reactor::HideKey = VK_END;
int Calamari::Reactor::Serial;

std::unordered_map<long, std::string> Calamari::Reactor::Mobs;
std::unordered_map<long, std::string> Calamari::Reactor::Objects;
std::shared_mutex Calamari::Reactor::MobsMutex;
std::shared_mutex Calamari::Reactor::ObjsMutex;
bool Calamari::Reactor::HasThreaded = false;

void __declspec(dllexport) Void() {}

//////////////////////////////
//////// REACTOR CORE ////////
//////////////////////////////

void Calamari::Reactor::PatchDRM()
{
    VMP_BEGIN("Calamari::Reactor::PatchDRM");
    // 513 patch
    void* DRMAddress = Pocket::Sigscan::FindPattern(VMPSTR("dreamseeker.exe"), VMPSTR("0F 84 ?? ?? ?? ?? 8D 85 ?? ?? ?? ?? 50 8D 85 ?? ?? ?? ?? 50 FF 15 ?? ?? ?? ?? 83 C4 ?? 50"));
    if (DRMAddress) {
        char DRMPatch[2] = { 0x0F, 0x85 };
        WriteProcessMemory(Process, DRMAddress, reinterpret_cast<LPVOID>(DRMPatch), 2, NULL);
    } else {
        // 512 patch
        DRMAddress = Pocket::Sigscan::FindPattern(VMPSTR("dreamseeker.exe"), VMPSTR("0F 82 ?? ?? ?? ?? 56 FF 15 ?? ?? ?? ?? 33 F6"));
        if (DRMAddress) {
            char DRMPatch[2] = { 0x0F, 0x84 };
            WriteProcessMemory(Process, DRMAddress, reinterpret_cast<LPVOID>(DRMPatch), 2, NULL);
        }
    }
    VMP_END();
}

void Calamari::Reactor::CoreHook()
{
    VMP_BEGIN("Calamari::Reactor::CoreHook");
    Calamari::Byond::Byond32Loc = Pocket::Sigscan::FindPattern(VMPSTR("byondcore.dll"), VMPSTR("55 8B EC 80 3D ?? ?? ?? ?? ?? 75 ?? E8 ?? ?? ?? ?? 8B 45 ??"));
    MH_CreateHook(GetProcAddress(GetModuleHandleA("kernel32.dll"), VMPSTR("EnumProcessModules")), &Calamari::Hooks::EnumProcessModules, NULL);
    MH_CreateHook(GetProcAddress(GetModuleHandleA("kernel32.dll"), VMPSTR("CreateFileA")), &Calamari::Hooks::CreateFileA, (LPVOID*)&Calamari::Hooks::CreateFileAFunc);
    MH_CreateHook(GetProcAddress(GetModuleHandleA("kernel32.dll"), VMPSTR("GetVolumeInformationA")), &Calamari::Hooks::GetVolumeInformationA, NULL);
    //    IsByondMemberFunc = (IsByondMember)GetProcAddress(GetModuleHandleA(VMPSTR("byondcore.dll")), VMPSTR("?IsByondMember@DungClient@@QAE_NPAJ@Z"));
    MH_CreateHook(GetProcAddress(GetModuleHandleA(VMPSTR("byondcore.dll")), VMPSTR("?IsByondMember@DungClient@@QAE_NPAJ@Z")), &Calamari::Hooks::IsByondMember, (LPVOID*)&Calamari::Byond::DCIsByondMemberFunc);
    MH_CreateHook(GetProcAddress(GetModuleHandleA(VMPSTR("byondcore.dll")), VMPSTR("?IsByondMember@DungPager@@QAE_NPAJ@")), &Calamari::Hooks::IsByondMember, (LPVOID*)&Calamari::Byond::DPIsByondMemberFunc);
    void* CIDSendLoc = Pocket::Sigscan::FindPattern(VMPSTR("byondcore.dll"), VMPSTR("E8 ?? ?? ?? ?? 83 C4 ?? 8B F0 33 C9"));
    /*
    if (CIDSendLoc != NULL) {
        void* b = &Calamari::Hooks::Byond32;
        uint32_t x = reinterpret_cast<uint32_t>(b) - reinterpret_cast<uint32_t>(CIDSendLoc);
        uint32_t* p = (uint32_t*)&x;
        char CIDPatch[5] = { 0xE8, p[3], p[2], p[1], p[0] };
        WriteProcessMemory(Process, CIDSendLoc, reinterpret_cast<LPVOID>(CIDPatch), 5, NULL);
    }*/
    if(Calamari::Byond::Byond32Loc != NULL)
        MH_CreateHook(Calamari::Byond::Byond32Loc, &Calamari::Hooks::Byond32, (LPVOID*)&Calamari::Byond::Byond32Func);
#ifdef PACKET_DEBUGGER
    if(origidecxode)
        MH_CreateHook(origidecxode, &Calamari::Hooks::Decode, (LPVOID*)&Calamari::Hooks::NetMsgDecodeFunc);
#endif
    MH_EnableHook(MH_ALL_HOOKS);
    VMP_END();
}

void Calamari::Reactor::DXHook()
{
    VMP_BEGIN("Calamari::Reactor::DXHook");
    kiero::init(kiero::RenderType::D3D9);
    kiero::bind(42, (void**)&Calamari::Hooks::EndSceneFunc, &Calamari::Hooks::EndScene);
    kiero::bind(17, (void**)&Calamari::Hooks::PresentFunc, &Calamari::Hooks::Present);
    kiero::bind(16, (void**)&Calamari::Hooks::ResetFunc, &Calamari::Hooks::Reset);
    Calamari::Hooks::WndProcFunc = (WNDPROC)SetWindowLongA(Calamari::Byond::DirectXWnd, GWLP_WNDPROC, (LONG)Calamari::Hooks::WndProc);
    VMP_END();
}

void Calamari::Reactor::DSHook()
{
    VMP_BEGIN("Calamari::Reactor::DSHook");
    MH_CreateHook(*Calamari::Byond::GenMouseDownCommandFunc, &Calamari::Hooks::MouseDown, (LPVOID*)&Calamari::Byond::GenMouseDownCommandFunc);
    MH_CreateHook(*Calamari::Byond::GenMouseMoveCommandFunc, &Calamari::Hooks::MouseMove, (LPVOID*)&Calamari::Byond::GenMouseMoveCommandFunc);
    MH_CreateHook(*Calamari::Byond::GenClickCommandFunc, &Calamari::Hooks::Click, (LPVOID*)&Calamari::Byond::GenClickCommandFunc);
    MH_CreateHook(*Calamari::Byond::GenMouseUpCommandFunc, &Calamari::Hooks::MouseUp, (LPVOID*)&Calamari::Byond::GenMouseUpCommandFunc);
    MH_CreateHook(*Calamari::Byond::DSAddStatFunc, &Calamari::Hooks::Stat, (LPVOID*)&Calamari::Byond::DSAddStatFunc);
    MH_CreateHook(*Calamari::Byond::CIOutputCtrlPutTextFunc, &Calamari::Hooks::PutText, (LPVOID*)&Calamari::Byond::CIOutputCtrlPutTextFunc);
    MH_EnableHook(MH_ALL_HOOKS);
    VMP_END();
}

void Calamari::Reactor::Unhook()
{
    VMP_BEGIN("Calamari::Reactor::Unhook");
    MH_RemoveHook(GetProcAddress(GetModuleHandleA("kernel32.dll"), VMPSTR("EnumProcessModules")));
    MH_RemoveHook(GetProcAddress(GetModuleHandleA(VMPSTR("byondcore.dll")), VMPSTR("?GenMouseDownCommand@DungClient@@QAEHUCid@@PBDFFFFD11@Z")));
    MH_RemoveHook(GetProcAddress(GetModuleHandleA(VMPSTR("byondcore.dll")), VMPSTR("?GenMouseUpCommand@DungClient@@QAEHUCid@@PBDFFFFD11@Z")));
    MH_RemoveHook(GetProcAddress(GetModuleHandleA(VMPSTR("byondcore.dll")), VMPSTR("?GenClickCommand@DungClient@@QAEHUCid@@PBDFFFFD11@Z")));
    MH_RemoveHook(GetProcAddress(GetModuleHandleA(VMPSTR("byondwin.dll")), VMPSTR("?PutText@CIOutputCtrl@@UAEHV?$CStringT@DV?$StrTraitMFC_DLL@DV?$ChTraitsCRT@D@ATL@@@@@ATL@@PBD@Z")));
    MH_RemoveHook(GetProcAddress(GetModuleHandleA(VMPSTR("byondwin.dll")), VMPSTR("?PutText@CIOutputCtrl@@UAEHUDMString@@PBD@Z")));
    MH_RemoveHook(GetProcAddress(GetModuleHandleA(VMPSTR("byondcore.dll")), VMPSTR("?AddStat@DSStat@@QAEXPBD0UCid@@G@Z")));
    MH_RemoveHook(GetProcAddress(GetModuleHandleA(VMPSTR("byondcore.dll")), VMPSTR("?GenMouseDragMoveCommand@DungClient@@QAEHUCid@@0PBD01FFD111@Z")));
    MH_RemoveHook(GetProcAddress(GetModuleHandleA("kernel32.dll"), VMPSTR("CreateFileA")));
    MH_RemoveHook(GetProcAddress(GetModuleHandleA("kernel32.dll"), VMPSTR("GetVolumeInformationA")));\
    if(Calamari::Byond::Byond32Loc != NULL)
        MH_RemoveHook(Calamari::Byond::Byond32Loc);
#ifdef PACKET_DEBUGGER
    if(origidecxode)
        MH_RemoveHook(origidecxode);
#endif
    MH_DisableHook(MH_ALL_HOOKS);
    kiero::shutdown();
    SetWindowLongPtr(Calamari::Byond::DirectXWnd, GWLP_WNDPROC, (LONG)Calamari::Hooks::WndProcFunc);
    for (Plugin* p : Plugins) {
        HMODULE lib = p->Library;
        delete p;
        FreeLibrary(lib);
    }
    Plugins.clear();
    VMP_END();
}

inline int ConfigParseInt(std::string str, int default)
{
    try {
        RemoveSpaces(&str);
        return std::stoi(str);
    } catch (...) {
        return default;
    }
}

inline bool ConfigParseBool(std::string str, bool default)
{
    try {
        bool x = default;
        RemoveSpaces(&str);
        std::istringstream(str) >> std::boolalpha >> x;
        return x;
    } catch (...) {
        return default;
    }
}

void Calamari::Reactor::ParseSettings()
{
    VMP_BEGIN("Calamari::Reactor::ParseSettings");
    License = ConfigIni.GetValue(VMPSTR("Calamari"), VMPSTR("LicenseKey"), VMPSTR("INVALID"));
    RemoveSpaces(&License);
    ToggleKey = ConfigParseInt(ConfigIni.GetValue(VMPSTR("Calamari"), VMPSTR("ToggleKey"), VMPSTR("45")), VK_INSERT);
    Serial = ConfigParseInt(ConfigIni.GetValue(VMPSTR("Calamari"), VMPSTR("SpoofedSerial"), VMPSTR("0")), 0);
    VMP_END();
}

void Calamari::Reactor::RegisterKey(int pKey, void* condition)
{
    KeyState* keyState = new KeyState();
    keyState->Key = pKey;
    keyState->Call = (KeyCall)condition;
    Keybinds.push_back(keyState);
}

void Calamari::Reactor::PollKeys()
{
    VMP_BEGIN("Calamari::Reactor::PollKeys");
    for (KeyState* ks : Keybinds) {
        if (GetAsyncKeyState(ks->Key) < 0 && !ks->State)
            ks->State = true;
        if (GetAsyncKeyState(ks->Key) == 0 && ks->State) {
            ks->State = false;
            ((KeyCall)ks->Call)(ks->Key);
        }
    }
    VMP_END();
}

void Calamari::Reactor::ObjScraperThread()
{
    VMP_BEGIN("Calamari::Reactor::ObjScraperThread");
    while (true) {
        for (long i = 1; i < 2500000; i++) {
            std::shared_lock slock(ObjsMutex);
            if (Objects.count(i)) {
                slock.unlock();
                continue;
            }
            slock.unlock();
            auto cid = new Cid;
            cid->t1 = 0x2;
            cid->t2 = i;
            std::string name = Calamari::Byond::CidName(*cid);
            delete cid;
            if (!name.empty()) {
                std::unique_lock lock(ObjsMutex);
                Objects.insert(std::make_pair(i, name));
                lock.unlock();
            }
        }
    }
    VMP_END();
}

void Calamari::Reactor::MobScraperThread()
{
    VMP_BEGIN("Calamari::Reactor::MobScraperThread");
    while (true) {
        for (long i = 1; i < 5000; i++) {
            std::shared_lock slock(MobsMutex);
            if (Mobs.count(i)) {
                slock.unlock();
                continue;
            }
            slock.unlock();
            auto cid = new Cid;
            cid->t1 = 0x1;
            cid->t2 = i;
            std::string name = Calamari::Byond::CidName(*cid);
            delete cid;
            if (!name.empty()) {
                std::unique_lock lock(MobsMutex);
                Mobs.insert(std::make_pair(i, name));
                lock.unlock();
            }
        }
    }
    VMP_END();
}

void Calamari::Reactor::ResetCidCache()
{
    std::unique_lock mobs_lock(MobsMutex);
    Mobs.clear();
    mobs_lock.unlock();
    std::unique_lock obj_lock(ObjsMutex);
    Objects.clear();
    obj_lock.unlock();
}

bool Calamari::Reactor::IsBlacklisted()
{
    VMP_BEGIN("Calamari::Reactor::IsBlacklisted");
    std::string ip = Calamari::Byond::ServerIP();
    return (strcmp(ip.c_str(), VMPSTR("46.36.35.40")) == 0 || strcmp(ip.c_str(), VMPSTR("104.168.182.234")) == 0);
    VMP_END();
}

void Calamari::Reactor::ToggleGUIInteraction()
{
    VMP_BEGIN("Calamari::Reactor::ToggleGUIInteraction");
    GuiInteraction = !GuiInteraction;
    VMP_END();
}

void Calamari::Reactor::ToggleHideGui()
{
    VMP_BEGIN("Calamari::Reactor::ToggleHideGui");
    HideGui = !HideGui;
    VMP_END();
}

void Calamari::Reactor::Log(std::string text)
{
    VMP_BEGIN("Calamari::Reactor::Log");
    if (!LogStream.is_open())
        return;
    auto in_time_t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), VMPSTR("%Y-%m-%d %H:%M:%S"));
    ss << " " + text;
    auto str = ss.str();
    LogStream << str << std::endl;
#ifdef DEBUG_CONSOLE
    std::cout << str << std::endl;
#endif
    VMP_END();
}

void Calamari::Reactor::Initialize(HMODULE MainThread)
{
    VMP_BEGIN("Calamari::Reactor::Initialize");
    SuspendThread(MainThread); // Go ahead and suspend it. Give it ZERO time to do any sort of checks.
    ProcessThread = MainThread;

#ifdef PACKET_DEBUGGER
    origidecxode = Pocket::Sigscan::FindPattern(VMPSTR("byondcore.dll"), VMPSTR("55 8B EC 53 56 8B F1 32 DB 8B 46 ??"));
#endif

#ifdef DEBUG_CONSOLE
    AllocConsole();
    freopen("CONOUT$", "w", stdout);
#endif

    GetModuleFileNameA(NULL, ProcessName, 256);
    Process = GetCurrentProcess();
    IsDreamSeeker = (strstr(ProcessName, VMPSTR("dreamseeker")) != 0);

    char Documents[MAX_PATH];
    SHGetFolderPathA(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, Documents);

    ByondPath += std::string(Documents) + VMPSTR("\\BYOND");
    Path += ByondPath + VMPSTR("\\Calamari");

    std::string LicensePath = Path + VMPSTR("\\.license");
    PluginsPath = Path + VMPSTR("\\Plugins");
    ConfigPath = Path + VMPSTR("\\config.ini");

    std::string keyfile = ByondPath + VMPSTR("\\key.txt");

    if (PathFileExistsA(keyfile.c_str())) {
        std::ifstream key(keyfile);
        std::string keydata((std::istreambuf_iterator<char>(key)),
            std::istreambuf_iterator<char>());
        key.close();
        // most likely people to reverse engineer, let's just not let them do that.
        if (scon(keydata, VMPSTR("optimumtact")) || scon(keydata, VMPSTR("lummox jr")) || scon(keydata, VMPSTR("mrstonedone")) || scon(keydata, VMPSTR("mcdonald072"))) {
            BSOD();
            return;
        }
    }

    if (!PathIsDirectoryA(Path.c_str()))
        CreateDirectoryA(Path.c_str(), 0);

    if (!PathIsDirectoryA(PluginsPath.c_str()))
        CreateDirectoryA(PluginsPath.c_str(), 0);

    if (IsDreamSeeker)
        LogStream.open(Path + VMPSTR("\\calamari.log"));
    Log(VMPSTR("[Calamari] Initialization commence!"));

    ConfigIni.SetValue(VMPSTR("Calamari"), VMPSTR("LicenseKey"), VMPSTR("INSERT PRODUCT KEY HERE"));
    ConfigIni.SetValue(VMPSTR("Calamari"), VMPSTR("ToggleKey"), VMPSTR("45"));
    ConfigIni.SetValue(VMPSTR("Calamari"), VMPSTR("HideKey"), VMPSTR("35"));
    ConfigIni.SetValue(VMPSTR("Calamari"), VMPSTR("SpoofedSerial"), VMPSTR("0"));
    if (PathFileExistsA(ConfigPath.c_str())) {
        ConfigIni.LoadFile(ConfigPath.c_str());
        Log(VMPSTR("[Calamari] Loaded existing config.ini"));
    } else {
        ConfigIni.SaveFile(ConfigPath.c_str());
        Log(VMPSTR("[Calamari] Created default config.ini"));
    }

    HWND mainWnd = FindMainWindow(GetCurrentProcessId());

    ParseSettings();
    if (Serial == 0) {
        Serial = Randint(1, INT32_MAX - 1);
        ConfigIni.SetValue(VMPSTR("Calamari"), VMPSTR("SpoofedSerial"), std::to_string(Serial).c_str());
        ConfigIni.SaveFile(ConfigPath.c_str());
        Log(VMPSTR("[Calamari] Generated random serial."));
    }

#ifdef USE_ACTIVATION
    if (License == VMPSTR("INSERT PRODUCT KEY HERE")) {
        MessageBoxA(NULL, VMPSTR("Please set your License Key in 'Documents/BYOND/Calamari/config.ini'!"), VMPSTR("Calamari"), 0);
        ShellExecuteA(NULL, "edit", ConfigPath.c_str(), NULL, Path.c_str(), true);
    }

    char VMPSerial[8192];
    bool Activated = false;
    bool TryToActivate = false;
    //printf("License is %s\n", License.c_str());
    int KeyRes = VMProtectActivateLicense(License.c_str(), VMPSerial, 8192);
    switch (KeyRes) {
    case ACTIVATION_OK:
        Activated = true;
        break;
    case ACTIVATION_ALREADY_USED:
        Log(VMPSTR("ACTIVATION_ALREADY_USED"));
        MessageBoxA(NULL, VMPSTR("Please only use this Calamari license key with the original machine it was created for.\nIf this is a mistake, please use calamaribot to regenerate your key."), VMPSTR("Calamari"), 0);
        break;
    case ACTIVATION_NO_CONNECTION:
        Log(VMPSTR("ACTIVATION_NO_CONNECTION"));
        if (PathFileExistsA(LicensePath.c_str()))
            TryToActivate = true;
        else
            MessageBoxA(NULL, VMPSTR("Failed to connect to Calamari activation server."), VMPSTR("Calamari"), 0);
        break;
    case ACTIVATION_BAD_REPLY:
        Log(VMPSTR("ACTIVATION_BAD_REPLY"));
        if (PathFileExistsA(LicensePath.c_str()))
            TryToActivate = true;
        else
            MessageBoxA(NULL, VMPSTR("Recieved bad reply from Calamari activation server."), VMPSTR("Calamari"), 0);
        break;
    case ACTIVATION_CORRUPTED:
    case ACTIVATION_BAD_CODE:
    case ACTIVATION_SERIAL_UNKNOWN:
        Log(VMPSTR("ACTIVATION_CORRUPTED"));
        MessageBoxA(NULL, VMPSTR("Invalid Calamari license key."), VMPSTR("Calamari"), 0);
        if (PathFileExistsA(LicensePath.c_str()))
            DeleteFileA(LicensePath.c_str());
        break;
    default:
        MessageBoxA(NULL, VMPSTR("Could not activate Calamari."), VMPSTR("Calamari"), 0);
        break;
    }
    if (Activated || TryToActivate) {
        if (TryToActivate) {
            std::ifstream lic(LicensePath);
            lic.read(VMPSerial, sizeof(VMPSerial));
            lic.close();
        }
        VMProtectSetSerialNumber(VMPSerial);
        VMProtectSerialNumberData sd = { 0 };
        VMProtectGetSerialNumberData(&sd, sizeof(sd));
        if (sd.nState != SERIAL_STATE_SUCCESS) {
            MessageBoxA(NULL, VMPSTR("Could not activate Calamari."), VMPSTR("Calamari"), 0);
            Activated = false;
            if (PathFileExistsA(LicensePath.c_str()))
                DeleteFileA(LicensePath.c_str());
        } else {
            if (!TryToActivate) {
                std::ofstream lic(LicensePath);
                lic << VMPSerial;
                lic.close();
            }
            _bstr_t b(sd.wUserName);
            Username = _strdup((const char*)b);
        }
    }
    if (!Activated) {
        if (IsDreamSeeker) { // let's be nice
            Unhook();
            PatchDRM();
        }
        ResumeThread(ProcessThread);
        return;
    }
#endif

    Calamari::Byond::LoadByondFunctions();

    MH_Initialize();
    CoreHook();

    if (IsDreamSeeker) {
        Log(VMPSTR("[Calamari] Patching/Hooking DreamSeeker"));
        PatchDRM();
        DSHook();
    }

    auto plugins = FindFilesWithExt(PluginsPath + VMPSTR("\\*.dll"));
    for (auto pFile : plugins) {
        std::string pPath = PluginsPath + "\\" + pFile;
        HMODULE lib = LoadLibraryA(pPath.c_str());
        if (lib) {

            InitializePlugin init = (InitializePlugin)GetProcAddress(lib, VMPSTR("InitializePlugin"));
            if (!init) {
                Log(VMPSTR("[Calamari] Failed to initialize plugin DLL '") + pFile + "'");
                FreeLibrary(lib);
            } else {
                try {
                    Plugin* p = init();
                    p->Library = lib;
                    if (p->Name.empty())
                        p->Name = p->GetName();
                    Plugins.push_back(p);
                    Log(VMPSTR("[Calamari] Loaded plugin '") + p->Name + "'");
                } catch (std::exception& e) {
                    FreeLibrary(lib);
                    Log(VMPSTR("[Calamari] Failed to load plugin '") + pFile + "': " + e.what());
                }
            }
        } else {
            Log(VMPSTR("[Calamari] Failed to load plugin DLL '") + pFile + "'");
        }
    }

    ResumeThread(ProcessThread);

    if (IsDreamSeeker) {
        RegisterKey(ToggleKey, &ToggleGUIInteraction);
        RegisterKey(HideKey, &ToggleHideGui);
        while (!Calamari::Byond::DirectXWnd) {
            FindDirectXWindow(FindMainWindow(GetCurrentProcessId()));
            Sleep(200);
        }
        Log(VMPSTR("[Calamari] Found DirectX window, hooking DX functions."));
        DXHook();
    }

    VMP_END();
}

////////////////////////////
//////// PLUGIN API ////////
////////////////////////////

__declspec(dllexport) Plugin::Plugin()
{
    VMP_BEGIN("Plugin::Plugin");
#ifndef _DEBUG
    RECheck();
#endif
    VMP_END();
}

__declspec(dllexport) Plugin::~Plugin()
{
    VMP_BEGIN("Plugin::~Plugin");
#ifndef _DEBUG
    RECheck();
#endif
    for (EventCall* e : Events) {
        Calamari::Reactor::Events.erase(std::remove(Calamari::Reactor::Events.begin(), Calamari::Reactor::Events.end(), e), Calamari::Reactor::Events.end());
        delete e;
    }
    Events.clear();
    for (KeyState* e : Calamari::Reactor::Keybinds) {
        if (e->Plugin == this) {
            Calamari::Reactor::Keybinds.erase(std::remove(Calamari::Reactor::Keybinds.begin(), Calamari::Reactor::Keybinds.end(), e), Calamari::Reactor::Keybinds.end());
            delete e;
        }
    }
    VMP_END();
}

__declspec(dllexport) std::string Plugin::GetName() { return std::string(); }

__declspec(dllexport) bool Plugin::AddEvent(std::string event, void* function, bool overriding)
{
    VMP_BEGIN("Plugin::AddEvent");
#ifndef _DEBUG
    RECheck();
#endif
    if (event.empty() || !function)
        return false;
    for (EventCall* e : Calamari::Reactor::Events) {
        if (e->Event == event) {
            if (overriding && e->Overriding)
                return false;
            if (e->Plugin == this)
                return false;
        }
    }
    EventCall* ev = new EventCall;
    ev->Event = event;
    ev->Function = function;
    ev->Overriding = overriding;
    ev->Plugin = this;
    Events.push_back(ev);
    Calamari::Reactor::Events.push_back(ev);
    Calamari::Reactor::Log(VMPSTR("[Calamari] Added ") + std::string(overriding ? VMPSTR("overriding ") : "") + VMPSTR("event '") + ev->Event + VMPSTR("' for plugin '") + GetName() + VMPSTR("'"));
    return true;
    VMP_END();
}

__declspec(dllexport) bool Plugin::RemoveEvent(std::string event)
{
    VMP_BEGIN("Plugin::RemoveEvent");
#ifndef _DEBUG
    RECheck();
#endif
    if (event.empty())
        return false;
    for (EventCall* e : Calamari::Reactor::Events) {
        if (e->Event == event && e->Plugin == this) {
            Calamari::Reactor::Log(VMPSTR("[Calamari] Removed ") + std::string(e->Overriding ? VMPSTR("overriding") : "") + VMPSTR(" event '") + e->Event + VMPSTR("' for plugin '") + GetName() + VMPSTR("'"));
            Calamari::Reactor::Events.erase(std::remove(Calamari::Reactor::Events.begin(), Calamari::Reactor::Events.end(), e), Calamari::Reactor::Events.end());
            Events.erase(std::remove(Events.begin(), Events.end(), e), Events.end());
            delete e;
            return true;
        }
    }
    return false;
    VMP_END();
}

__declspec(dllexport) std::string Plugin::GetConfig(std::string what, std::string default)
{
    VMP_BEGIN("Plugin::GetConfig");
#ifndef _DEBUG
    RECheck();
#endif
    if (Name.empty())
        Name = GetName();
    CSimpleIniA* ini = &Calamari::Reactor::ConfigIni;
    if (ini->GetSectionSize(Name.c_str()) == -1)
        return std::string();
    return std::string(ini->GetValue(Name.c_str(), what.c_str(), default.c_str()));
    VMP_END();
}

__declspec(dllexport) void Plugin::SetConfig(std::string what, std::string data)
{
    VMP_BEGIN("Plugin::SetConfig");
#ifndef _DEBUG
    RECheck();
#endif
    if (Name.empty())
        Name = GetName();
    CSimpleIniA* ini = &Calamari::Reactor::ConfigIni;
    ini->SetValue(Name.c_str(), what.c_str(), data.c_str());
    ini->SaveFile(Calamari::Reactor::ConfigPath.c_str());
    VMP_END();
}

__declspec(dllexport) void Plugin::Log(std::string text)
{
    VMP_BEGIN("Plugin::Log");
    if (Name.empty())
        Name = GetName();
    if (text.empty())
        return;
    Calamari::Reactor::Log("[" + Name + "] " + text);
    VMP_END();
}

__declspec(dllexport) bool Plugin::AddKeyHandler(std::string name, int keyCode, void* handler)
{
    VMP_BEGIN("Plugin::AddKeyHandler");
#ifndef _DEBUG
    RECheck();
#endif
    if (name.empty() || !handler)
        return false;
    for (KeyState* e : Calamari::Reactor::Keybinds) {
        if (e->Plugin == this && e->Name == name)
            return false;
    }
    KeyState* kc = new KeyState();
    kc->Name = name;
    kc->Plugin = this;
    kc->Key = keyCode;
    kc->Call = (KeyCall)handler;
    Calamari::Reactor::Keybinds.push_back(kc);
    Calamari::Reactor::Log(VMPSTR("[Calamari] Registered key-handler for plugin '") + GetName() + VMPSTR("' named '") + name + VMPSTR("', key ") + std::to_string(keyCode));
    return true;
    VMP_END();
}

__declspec(dllexport) bool Plugin::RemoveKeyHandler(std::string name)
{
    VMP_BEGIN("Plugin::RemoveKeyHandler");
#ifndef _DEBUG
    RECheck();
#endif
    if (name.empty())
        return false;
    for (KeyState* e : Calamari::Reactor::Keybinds) {
        if (e->Name == name && e->Plugin == this) {
            Calamari::Reactor::Log(VMPSTR("[Calamari] Removed key-handler for plugin '") + GetName() + VMPSTR("' named '") + e->Name + VMPSTR("', key ") + std::to_string(e->Key));
            Calamari::Reactor::Keybinds.erase(std::remove(Calamari::Reactor::Keybinds.begin(), Calamari::Reactor::Keybinds.end(), e), Calamari::Reactor::Keybinds.end());
            delete e;
            return true;
        }
    }
    return false;
    VMP_END();
}

void* SEHIPCRecv(Plugin* from, Plugin* to, void* data)
{
    VMP_BEGIN("SEHIPCRecv");
    __try
    {
        return to->IPCRecv(from, data);
    } __except(EXCEPTION_EXECUTE_HANDLER) {
        return nullptr;
    }
    VMP_END();
}

__declspec(dllexport) void* Plugin::IPCSend(std::string target, void* data)
{
    VMP_BEGIN("Plugin::IPCSend");
    for (Plugin* plugin : Calamari::Reactor::Plugins) {
        if (plugin->Name == target) {
            return SEHIPCRecv(this, plugin, data);
        }
    }
    return nullptr;
    VMP_END();
}

__declspec(dllexport) void* Plugin::IPCRecv(Plugin* from, void* data)
{
    VMP_BEGIN("Plugin::IPCRecv");
    return nullptr;
    VMP_END();
}
