#include "../calamari.public/byond.h"
#include "../calamari.public/events.h"
#include "../calamari.public/plugin.h"
#include "../imgui/imgui.h"
#include <Psapi.h>
#include <chrono>
#include <map>
#include <vector>
#include <windows.h>

struct ProjectileVector {
    double vX; // tiles per ms
    double vY; // tiles per ms
    Position from;
};

struct PreProjectileVector {
    int X;
    int Y;
    long long time;
};

static bool smashWindows = false;

static std::vector<std::string> highPrio;
static std::vector<std::string> lowPrio;

static int clickCooldown = 400;
static std::chrono::time_point<std::chrono::steady_clock> lastClick;

static std::vector<std::string> projectileNames;

static std::map<long, ProjectileVector> projectileTracking;
static std::map<long, PreProjectileVector> projectileProto;

D3DTLVERTEX CreateD3DTLVERTEX(float X, float Y, float Z, float RHW, D3DCOLOR color, float U, float V)
{
    _D3DTLVERTEX v;

    v.sx = X;
    v.sy = Y;
    v.sz = Z;
    v.rhw = RHW;
    v.color = color;
    v.tu = U;
    v.tv = V;

    return v;
}

void Draw2DLine(POINT start, POINT finish, D3DCOLOR colour, LPDIRECT3DDEVICE9 dev)
{
    D3DTLVERTEX line[2];
    line[0] = CreateD3DTLVERTEX(start.x, start.y, 0.0f, 1.0f, colour, 0.0f, 0.0f);
    line[1] = CreateD3DTLVERTEX(finish.x, finish.y, 0.0f, 1.0f, colour, 0.0f, 0.0f);

    dev->SetFVF((D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1));
    dev->SetTexture(0, NULL);
    dev->DrawPrimitiveUP(D3DPT_LINESTRIP, 2, &line[0], sizeof(line[0]));
}

char* GetAddressOfData(DWORD pid, const char* data, size_t len, size_t starting = 0, int limit = 0x20CAF000)
{
    HANDLE process = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, FALSE, pid);
    if (process) {
        SYSTEM_INFO si;
        GetSystemInfo(&si);

        MEMORY_BASIC_INFORMATION info;
        std::vector<char> chunk;
        char* p = (char*)starting;
        while (p < si.lpMaximumApplicationAddress) {
            if (VirtualQuery(p, &info, sizeof(info)) == sizeof(info)) {
                p = (char*)info.BaseAddress;
                if ((int)p > limit)
                    return 0;
                chunk.resize(info.RegionSize);
                SIZE_T bytesRead;
                if (info.Protect == PAGE_READWRITE && info.State == MEM_COMMIT) {
                    //memcpy(&)
                    //if (ReadProcessMemory(process, p, &chunk[0], info.RegionSize, &bytesRead))
                    //{
                    for (size_t i = 0; i < (info.RegionSize - len); ++i) {
                        try {
                            if (memcmp(data, (char*)(p + i), len) == 0) {
                                if (((char*)p + i) != data) {
                                    return (char*)p + i;
                                }
                            }
                        } catch (...) {
                            p += info.RegionSize;
                            break;
                        }
                    }
                    //}
                }
                p += info.RegionSize;
            }
        }
    }
    return 0;
}

void RemoveOverlay(char* text)
{
    int lastFound = 0;
    MODULEINFO mi;
    K32GetModuleInformation(OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, FALSE, GetCurrentProcessId()), GetModuleHandleA("calamari-assist.dll"), &mi, sizeof(mi));
    while (true) {
        char* overlay = (char*)GetAddressOfData(GetCurrentProcessId(), text, strlen(text), lastFound);
        if (overlay == 0)
            break;
        if ((int)overlay >= (int)mi.lpBaseOfDll && (int)overlay <= ((int)mi.lpBaseOfDll + (int)mi.SizeOfImage)) {
            lastFound = (int)mi.lpBaseOfDll + (int)mi.SizeOfImage;
            continue;
        }
        lastFound = (int)overlay;
        strcpy(overlay, "nil");
    }
}

std::string spamstring;

inline unsigned int getAngle(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2)
{
    int dy = (32 * y2) - (32 * y1);
    int dx = (32 * x2) - (32 * x1);
    if (dy == 0)
        return (dx >= 0) ? 90 : 270;
    int ret = atan(dx / dy);
    if (dy < 0)
        ret += 180;
    else if (dx < 0)
        ret += 360;
    return ret;
}

void ToggleWindow()
{
    smashWindows = !smashWindows;
}

void OnRender(Plugin* self, LPDIRECT3DDEVICE9 dev, const RECT* client)
{
    if (ImGui::TreeNode("Assists")) {
        ImGui::Checkbox("Smash Windows", &smashWindows);
        if (ImGui::Button("Clean Overlays")) {
            RemoveOverlay("blackimageoverlay");
            RemoveOverlay("oxydamageoverlay0");
            RemoveOverlay("oxydamageoverlay1");
            RemoveOverlay("oxydamageoverlay2");
            RemoveOverlay("oxydamageoverlay3");
            RemoveOverlay("oxydamageoverlay4");
            RemoveOverlay("oxydamageoverlay5");
            RemoveOverlay("oxydamageoverlay6");
            RemoveOverlay("oxydamageoverlay7");
            RemoveOverlay("oxydamageoverlay8");
            RemoveOverlay("oxydamageoverlay9");
            RemoveOverlay("oxydamageoverlay10");
            RemoveOverlay("brutedamageoverlay0");
            RemoveOverlay("brutedamageoverlay1");
            RemoveOverlay("brutedamageoverlay2");
            RemoveOverlay("brutedamageoverlay3");
            RemoveOverlay("brutedamageoverlay4");
            RemoveOverlay("brutedamageoverlay5");
            RemoveOverlay("brutedamageoverlay6");
            RemoveOverlay("impairedoverlay1");
            RemoveOverlay("impairedoverlay2");
            RemoveOverlay("transparent");
        }
        ImGui::TreePop();
    }

    if (smashWindows) {
        auto objs = Calamari::Byond::GetObjectsOnScreen();
        std::vector<std::shared_ptr<Cid>> sorted_objs;
        std::vector<std::shared_ptr<Cid>> grilles;
        for (auto mob : objs) {
            auto name = Calamari::Byond::CidName(*mob.first);
            if (name != "window" && name != "reinforced window" && name != "tinted window" && name != "grille")
                continue;
            auto pos = Calamari::Byond::GetCidTileCoords(*mob.first);
            if (!(pos->X >= 7 && pos->X <= 9 && pos->Y >= 7 && pos->Y <= 9))
                continue;
            if (pos->X == 8 && pos->Y == 8)
                continue;
            if (name == "grille")
                grilles.push_back(mob.first);
            else
                sorted_objs.push_back(mob.first);
        }
        std::sort(sorted_objs.begin(), sorted_objs.end(), [](const std::shared_ptr<Cid>& a, const std::shared_ptr<Cid>& b) { return a->t2 > b->t2; });
        std::shared_ptr<Cid> target = NULL;
        if (sorted_objs.size() > 0)
            target = sorted_objs[0];
        else if (grilles.size() > 0)
            target = grilles[0];
        if (target) {
            auto now = std::chrono::steady_clock::now();
            auto since = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastClick).count();
            if (since >= clickCooldown) {
                auto pos = Calamari::Byond::GetCidTileCoords(*target);
                Calamari::Byond::Click(*target);
                lastClick = std::chrono::steady_clock::now();
            }
        }
    }
/*
    auto objs = Calamari::Byond::GetObjectsOnScreen();
    std::vector<std::shared_ptr<Cid>> sorted_objs;
    sorted_objs.reserve(objs.size());
    for (auto obj : objs)
        sorted_objs.push_back(obj.first);
    std::sort(sorted_objs.begin(), sorted_objs.end(), [](const std::shared_ptr<Cid>& a, const std::shared_ptr<Cid>& b) { return a->t2 > b->t2; });
    auto range = Calamari::Byond::GetViewRange();
    std::vector<long> waitTilNextTick;
    for (auto obj : sorted_objs) {
        auto apos = Calamari::Byond::GetCidAbsCoords(*obj);
        auto pos = Calamari::Byond::GetCidTileCoords(*obj);
        if (pos->X < 0 || pos->X > range->X || pos->Y < 0 || pos->Y > range->Y)
            continue;
        auto name = Calamari::Byond::CidName(*obj);
        if (name.empty())
            continue;
        if (projectileProto.count(obj->t2) || projectileTracking.count(obj->t2))
            continue;
        if (std::find(projectileNames.begin(), projectileNames.end(), name) == projectileNames.end())
            continue;
        printf("Start tracking '%s', %i/%i\n", name.c_str(), apos->X, apos->Y);
        auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        PreProjectileVector p = { apos->X, apos->Y, millis };
        projectileProto.insert(std::make_pair(obj->t2, p));
        waitTilNextTick.push_back(obj->t2);
    }
    for (auto proto : projectileProto) {
        if (std::find(waitTilNextTick.begin(), waitTilNextTick.end(), proto.first) != waitTilNextTick.end())
            continue;
        auto cid = new Cid;
        cid->t1 = 0x2;
        cid->t2 = proto.first;
        auto pos = Calamari::Byond::GetCidTileCoords(*cid);
        if (pos->X < 0 || pos->X > range->X || pos->Y < 0 || pos->Y > range->Y) {
            delete cid;
            continue;
        }
        auto apos = Calamari::Byond::GetCidAbsCoords(*cid);
        auto name = Calamari::Byond::CidName(*cid);
        if (name.empty()) {
            projectileProto.erase(proto.first);
            delete cid;
            continue;
        }
        if (apos->X == proto.second.X && apos->Y == proto.second.Y) {
            delete cid;
            continue;
        }
        auto nowMillis = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        auto time = nowMillis - proto.second.time;
        double distX = abs(abs(apos->X) - abs(proto.second.X)) ^ 2;
        double distY = abs(abs(apos->Y) - abs(proto.second.Y)) ^ 2;
        printf("Tracking '%s' - start (%i, %i), now (%i, %i), dist %f/%f\n", name.c_str(), proto.second.X, apos->X, proto.second.Y, apos->Y, distX, distY);
        auto angle = getAngle(proto.second.X, proto.second.Y, apos->X, apos->Y);
        ProjectileVector vector = { distX, distY, { apos->X, apos->Y } };
        projectileTracking.insert(std::make_pair(proto.first, vector));
        printf("Tracking '%s' - Angle %hi, Velocity %f tiles/ms\n", name.c_str(), angle, sqrt(distX + distY));
        projectileProto.erase(proto.first);
        delete cid;
    }
    auto vr = Calamari::Byond::GetViewRange();
    for (auto projectile : projectileTracking) {
        auto cid = new Cid;
        cid->t1 = 0x2;
        cid->t2 = projectile.first;
        auto vector = projectile.second;
        auto pos = Calamari::Byond::GetCidTileCoords(*cid);
        if (pos->X < 0 || pos->X > range->X || pos->Y < 0 || pos->Y > range->Y) {
            delete cid;
            continue;
        }
        auto name = Calamari::Byond::CidName(*cid);
        if (name.empty()) {
            delete cid;
            continue;
        }
        long width = client->right - client->left;
        long height = client->bottom - client->top;
        long ix = width / vr->X;
        long iy = height / vr->Y;
        POINT ctarget = { ((ix * (pos->X - 1)) + (long)(ix * 0.5)), ((vr->Y * iy) - ((pos->Y * iy) - (long)(iy * 0.5))) };
        Draw2DLine(ctarget, { (LONG)floor(vector.vX * 900), (LONG)floor(vector.vY * 900)}, D3DCOLOR_RGBA(255, 0, 0, 255), dev);
        delete cid;
    }*/
}

class Assist : public Plugin {
public:
    Assist()
        : Plugin()
    {
        AddEvent(EVENT_RENDER, &OnRender, false);
        AddKeyHandler("toggle window smashing", VK_DIVIDE, &ToggleWindow);
        std::string bl = GetConfig("ProjectileNames");
        if (bl.empty()) {
            bl = "bullet,laser,bolt,bolt of death,bolt of teleportation,bolt of change,arcane bolt, locker bolt,lightning bolt,bolt of fireball,infernal fireball,bolt,energy,7.12x82mm bullet,7.12x82mm armor-piercing bullet,7.12x82mm hollow-point bullet,7.12x82mm incendiary bullet,7.12x82mm match bullet,12g shotgun slug,beanbag slug,dragonsbreath pellet,stunslug,incendiary slug,meteorslug,buckshot pellet,rubbershot pellet,5.56mm bullet,7.62 bullet,electrode,disabler beam";
            SetConfig("ProjectileNames", bl);
        }
        size_t pos = 0;
        std::string token;
        std::string delimiter(",");
        while ((pos = bl.find(delimiter)) != std::string::npos) {
            token = bl.substr(0, pos);
            projectileNames.push_back(token);
            bl.erase(0, pos + delimiter.length());
        }
    }

    std::string GetName() override
    {
        return "Assist";
    }
};

extern "C" __declspec(dllexport) Assist* InitializePlugin()
{
    return new Assist;
}
