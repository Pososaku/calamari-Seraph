#include "byond.h"
#include "../common/util.h"

void* Calamari::Byond::ViewRangeXLoc = NULL;
void* Calamari::Byond::ViewRangeYLoc = NULL;
void* Calamari::Byond::Byond32Loc = NULL;

LPDIRECT3DDEVICE9 Calamari::Byond::DXDevice;
HWND Calamari::Byond::DirectXWnd;
HWND Calamari::Byond::InputWnd;

DungClient__Ctor Calamari::Byond::DungClientCtor = NULL;
GenMouseDownCommand Calamari::Byond::GenMouseDownCommandFunc = NULL;
GenMouseUpCommand Calamari::Byond::GenMouseUpCommandFunc = NULL;
GenClickCommand Calamari::Byond::GenClickCommandFunc = NULL;
GenMouseMoveCommand Calamari::Byond::GenMouseMoveCommandFunc = NULL;
GetCidLoc Calamari::Byond::GetCidLocFunc = NULL;
GetCidName Calamari::Byond::GetCidNameFunc = NULL;
GetServerPort Calamari::Byond::GetServerPortFunc = NULL;
GetServerIP Calamari::Byond::GetServerIPFunc = NULL;
GetPlayerCid Calamari::Byond::GetPlayerCidFunc = NULL;
CIOutputCtrlPutText Calamari::Byond::CIOutputCtrlPutTextFunc = NULL;
DSAddStat Calamari::Byond::DSAddStatFunc = NULL;
CommandEvent Calamari::Byond::CommandEventFunc = NULL;
ParseCommand Calamari::Byond::ParseCommandFunc = NULL;
Byond32 Calamari::Byond::Byond32Func = NULL;
IsByondMember Calamari::Byond::DPIsByondMemberFunc = NULL;
IsByondMember Calamari::Byond::DCIsByondMemberFunc = NULL;

void* Calamari::Byond::DungClient;
int Calamari::Byond::Major = 0;
int Calamari::Byond::Minor = 0;

int __declspec(dllexport) Calamari::Byond::ByondMajor()
{
    VMP_BEGIN("Calamari::Byond::ByondMajor");
    return Major;
    VMP_END();
}

int __declspec(dllexport) Calamari::Byond::ByondMinor()
{
    VMP_BEGIN("Calamari::Byond::ByondMinor");
    return Minor;
    VMP_END();
}

std::string __declspec(dllexport) Calamari::Byond::ByondVersion()
{
    VMP_BEGIN("Calamari::Byond::ByondVersion");
    return std::to_string(Major) + "." + std::to_string(Minor);
    VMP_END();
}

void Calamari::Byond::LoadByondFunctions()
{
    VMP_BEGIN("Calamari::Byond::LoadByondFunctions");
    DungClientCtor = (DungClient__Ctor)GetProcAddress(GetModuleHandleA(VMPSTR("byondcore.dll")), VMPSTR("??0DungClient@@QAE@XZ"));
    GenMouseDownCommandFunc = (GenMouseDownCommand)GetProcAddress(GetModuleHandleA(VMPSTR("byondcore.dll")), VMPSTR("?GenMouseDownCommand@DungClient@@QAEHUCid@@PBDFFFFD11@Z"));
    GenMouseUpCommandFunc = (GenMouseUpCommand)GetProcAddress(GetModuleHandleA(VMPSTR("byondcore.dll")), VMPSTR("?GenMouseUpCommand@DungClient@@QAEHUCid@@PBDFFFFD11@Z"));
    GenClickCommandFunc = (GenClickCommand)GetProcAddress(GetModuleHandleA(VMPSTR("byondcore.dll")), VMPSTR("?GenClickCommand@DungClient@@QAEHUCid@@PBDFFFFD11@Z"));
    GenMouseMoveCommandFunc = (GenMouseMoveCommand)GetProcAddress(GetModuleHandleA(VMPSTR("byondcore.dll")), VMPSTR("?GenMouseMoveCommand@DungClient@@QAEHUCid@@PBDFFFFD11@Z"));
    GetCidLocFunc = (GetCidLoc)GetProcAddress(GetModuleHandleA(VMPSTR("byondcore.dll")), VMPSTR("?GetCidLoc@DungClient@@QAE?AUCid@@U2@@Z"));
    GetCidNameFunc = (GetCidName)GetProcAddress(GetModuleHandleA(VMPSTR("byondcore.dll")), VMPSTR("?GetCidName@DungClient@@QAEPBDUCid@@@Z"));
    GetServerPortFunc = (GetServerPort)GetProcAddress(GetModuleHandleA(VMPSTR("byondcore.dll")), VMPSTR("?GetRemoteServerPort@DungClient@@QAEKXZ_0"));
    GetServerIPFunc = (GetServerIP)GetProcAddress(GetModuleHandleA(VMPSTR("byondcore.dll")), VMPSTR("?GetServerIP@DungClient@@QAEPBDPBD@Z"));
    GetPlayerCidFunc = (GetPlayerCid)GetProcAddress(GetModuleHandleA(VMPSTR("byondcore.dll")), VMPSTR("?GetPlayerCid@DungClient@@QAE?AUCid@@XZ"));
    CIOutputCtrlPutTextFunc = (CIOutputCtrlPutText)GetProcAddress(GetModuleHandleA(VMPSTR("byondwin.dll")), VMPSTR("?PutText@CIOutputCtrl@@UAEHV?$CStringT@DV?$StrTraitMFC_DLL@DV?$ChTraitsCRT@D@ATL@@@@@ATL@@PBD@Z"));
    if (!CIOutputCtrlPutTextFunc)
        CIOutputCtrlPutTextFunc = (CIOutputCtrlPutText)GetProcAddress(GetModuleHandleA(VMPSTR("byondwin.dll")), VMPSTR("?PutText@CIOutputCtrl@@UAEHUDMString@@PBD@Z"));
    DSAddStatFunc = (DSAddStat)GetProcAddress(GetModuleHandleA(VMPSTR("byondcore.dll")), VMPSTR("?AddStat@DSStat@@QAEXPBD0UCid@@G@Z"));
    CommandEventFunc = (CommandEvent)GetProcAddress(GetModuleHandleA(VMPSTR("byondcore.dll")), VMPSTR("?CommandEvent@DungClient@@QAEXEGE@Z"));
    ParseCommandFunc = (ParseCommand)GetProcAddress(GetModuleHandleA(VMPSTR("byondcore.dll")), VMPSTR("?ParseCommand@DungClient@@QAEEPBDEE@Z"));

    GetByondVersion gbv = (GetByondVersion)GetProcAddress(GetModuleHandleA(VMPSTR("byondcore.dll")), VMPSTR("?GetByondVersion@ByondLib@@QAEJXZ"));
    GetByondBuild gbb = (GetByondBuild)GetProcAddress(GetModuleHandleA(VMPSTR("byondcore.dll")), VMPSTR("?GetByondBuild@ByondLib@@QAEJXZ"));
    if (gbb && gbv) {
        Major = gbv();
        Minor = gbb();
    }
    VMP_END();
}

std::unique_ptr<Position> __declspec(dllexport) Calamari::Byond::GetViewRange()
{
    VMP_BEGIN("Calamari::Byond::GetViewRange");
    std::unique_ptr<Position> pos(new Position());
    if (!ViewRangeXLoc)
        ViewRangeXLoc = Pocket::Sigscan::FindPattern(VMPSTR("byondcore.dll"), VMPSTR("0F BF 05 ?? ?? ?? ?? 50 0F BF 05 ?? ?? ?? ?? 50 0F BF 05 ?? ?? ?? ?? 50 C6 05 ?? ?? ?? ?? ??"), 3);
    if (!ViewRangeYLoc)
        ViewRangeYLoc = Pocket::Sigscan::FindPattern(VMPSTR("byondcore.dll"), VMPSTR("0F BF 05 ?? ?? ?? ?? 8B 15 ?? ?? ?? ??"), 3);
    if (ViewRangeXLoc && ViewRangeYLoc) {
        pos->X = **(int**)ViewRangeXLoc;
        pos->Y = **(int**)ViewRangeYLoc;
    }
    return pos;
    VMP_END();
}

std::unique_ptr<Position> __declspec(dllexport) Calamari::Byond::ConvertIntToPosition(int pos)
{
    VMP_BEGIN("Calamari::Byond::ConvertIntToPosition");
    std::unique_ptr<Position> newPos(new Position());
    newPos->X = pos & 0x000FFF;
    newPos->Y = ((pos & 0xFFF000) >> 12) / 16;
    return newPos;
    VMP_END();
}

std::unique_ptr<Position> __declspec(dllexport) Calamari::Byond::GetTilePositionFromAbsolute(Position centerPosition, Position objectPosition)
{
    VMP_BEGIN("Calamari::Byond::GetTilePositionFromAbsolute");
    std::unique_ptr<Position> pos(new Position());
    pos->X = (objectPosition.X - centerPosition.X) + 8;
    pos->Y = (objectPosition.Y - centerPosition.Y) + 8;
    return pos;
    VMP_END();
}

std::unique_ptr<Position> __declspec(dllexport) Calamari::Byond::GetCidAbsCoords(struct Cid target)
{
    VMP_BEGIN("Calamari::Byond::GetCidTileCoords");
    std::unique_ptr<struct Cid> cid(new Cid);
    std::unique_ptr<struct Cid> playerCid(new Cid);
    DWORD* anus = GetCidLocFunc((DWORD*)DungClient, cid.get(), target.t1, target.t2);
    auto pTargetPos = ConvertIntToPosition(anus[1]);
    if (DungClient != NULL)
        GetPlayerCidFunc(DungClient, playerCid.get());
    anus = GetCidLocFunc((DWORD*)DungClient, cid.get(), playerCid->t1, playerCid->t2);
    return pTargetPos;
    VMP_END();
}

std::unique_ptr<Position> __declspec(dllexport) Calamari::Byond::GetCidTileCoords(struct Cid target)
{
    VMP_BEGIN("Calamari::Byond::GetCidTileCoords");
    std::unique_ptr<struct Cid> cid(new Cid);
    std::unique_ptr<struct Cid> playerCid(new Cid);
    DWORD* anus = GetCidLocFunc((DWORD*)DungClient, cid.get(), target.t1, target.t2);
    auto pTargetPos = ConvertIntToPosition(anus[1]);
    if (DungClient != NULL)
        GetPlayerCidFunc(DungClient, playerCid.get());
    anus = GetCidLocFunc((DWORD*)DungClient, cid.get(), playerCid->t1, playerCid->t2);
    auto pPlayerPos = ConvertIntToPosition(anus[1]);
    auto relativePos = GetTilePositionFromAbsolute(*pPlayerPos, *pTargetPos);
    return relativePos;
    VMP_END();
}

std::unique_ptr<Position> __declspec(dllexport) Calamari::Byond::CenterPosition()
{
    VMP_BEGIN("Calamari::Byond::CenterPosition");
    std::unique_ptr<struct Cid> cid(new Cid);
    std::unique_ptr<struct Cid> playerCid(new Cid);
    if (DungClient != NULL)
        GetPlayerCidFunc(DungClient, playerCid.get());
    DWORD* anus = GetCidLocFunc((DWORD*)DungClient, cid.get(), playerCid->t1, playerCid->t2);
    std::unique_ptr<Position> pPlayerPos = ConvertIntToPosition(anus[1]);
    delete anus;
    return pPlayerPos;
    VMP_END();
}

void SEHPlayerCid(void* DungClient, Cid* playerCid)
{
    VMP_BEGIN("SEHPlayerCid");
    __try {
        Calamari::Byond::GetPlayerCidFunc(DungClient, playerCid);
    } __except (EXCEPTION_EXECUTE_HANDLER) {
    }
    VMP_END();
}

std::unique_ptr<Cid> __declspec(dllexport) Calamari::Byond::PlayerCid()
{
    VMP_BEGIN("Calamari::Byond::PlayerCid");
    std::unique_ptr<Cid> playerCid(new Cid);
    if (DungClient != NULL)
        SEHPlayerCid(DungClient, playerCid.get());
    return playerCid;
    VMP_END();
}

char* SEHCidName(void* DungClient, Cid playerCid)
{
    VMP_BEGIN("SEHCidName");
    __try {
        return Calamari::Byond::GetCidNameFunc(DungClient, playerCid);
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        return "";
    }
    VMP_END();
}

std::string __declspec(dllexport) Calamari::Byond::CidName(Cid cid)
{
    VMP_BEGIN("Calamari::Byond::CidName");
    if (DungClient == NULL)
        return "";
    return std::string(SEHCidName(DungClient, cid));
    VMP_END();
}

void __declspec(dllexport) Calamari::Byond::Click(Cid cid)
{
    auto pos = GetCidTileCoords(cid);
    int px = Randint(4, 15);
    int py = Randint(4, 15);
    std::string params;
    params += VMPSTR("icon-x=") + std::to_string(px);
    params += VMPSTR(";icon-y=") + std::to_string(py);
    params += VMPSTR(";left=1");
    params += VMPSTR(";screen-loc=") + std::to_string(pos->X) + ":" + std::to_string(px) + "," + std::to_string(pos->Y) + ":" + std::to_string(py);
    GenClickCommandFunc(DungClient, cid, VMPSTR("map"), pos->X, pos->Y, px, py, 1, params.c_str(), VMPSTR("mapwindow.map"));
}

std::string __declspec(dllexport) Calamari::Byond::ServerIP()
{
    VMP_BEGIN("Calamari::Byond::ServerIP");
    if (!DungClient)
        return std::string();
    char serverIp[256];
    return std::string(GetServerIPFunc(DungClient, 0, serverIp));
    VMP_END();
}

unsigned int __declspec(dllexport) Calamari::Byond::ServerPort()
{
    VMP_BEGIN("Calamari::Byond::ServerPort");
    if (!DungClient)
        return 0;
    return GetServerPortFunc(DungClient);
    VMP_END();
}

void __declspec(dllexport) Calamari::Byond::RunCommand(std::string cmd)
{
    VMP_BEGIN("Calamari::Byond::RunCommand");
    if (!DungClient)
        return;
    CommandEventFunc(DungClient, ParseCommandFunc(DungClient, cmd.c_str(), 0x0, 0x0), 0, 0);
    VMP_END();
}

std::unordered_map<std::shared_ptr<Cid>, std::string> __declspec(dllexport) Calamari::Byond::GetObjectsOnScreen()
{
    VMP_BEGIN("Calamari::Byond::GetObjectsOnScreen");
    std::unordered_map<std::shared_ptr<Cid>, std::string> res;
    std::vector<long> objs;
    std::shared_lock lock(Calamari::Reactor::ObjsMutex);
    objs.reserve(Calamari::Reactor::Objects.size());
    for (std::pair<long, std::string> element : Calamari::Reactor::Objects)
        objs.push_back(element.first);
    lock.unlock();
    std::sort(objs.begin(), objs.end());
    for (long id : objs) {
        std::shared_ptr<Cid> cid(new Cid);
        cid->t1 = 0x2;
        cid->t2 = id;
        std::string name = CidName(*cid);
        if (!name.empty())
            res.insert(std::make_pair(cid, name));
    }
    return res;
    VMP_END();
}

std::unordered_map<std::shared_ptr<Cid>, std::string> __declspec(dllexport) Calamari::Byond::GetMobsOnScreen()
{
    VMP_BEGIN("Calamari::Byond::GetMobsOnScreen");
    std::unordered_map<std::shared_ptr<Cid>, std::string> res;
    std::vector<long> mobs;
    std::shared_lock lock(Calamari::Reactor::MobsMutex);
    mobs.reserve(Calamari::Reactor::Mobs.size());
    for (std::pair<long, std::string> element : Calamari::Reactor::Mobs)
        mobs.push_back(element.first);
    lock.unlock();
    std::sort(mobs.begin(), mobs.end());
    for (long id : mobs) {
        std::shared_ptr<Cid> cid(new Cid);
        cid->t1 = 0x1;
        cid->t2 = id;
        std::string name = CidName(*cid);
        if (!name.empty())
            res.insert(std::make_pair(cid, name));
    }
    return res;
    VMP_END();
}
