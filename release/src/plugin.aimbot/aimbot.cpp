#include "aimbot.h"

static std::unordered_map<long, std::string> friends;
static Cid* aimbotTarget = NULL;
static std::string targetName;
static bool aimbotToggle = false;
static bool rageToggle = false;
static bool aimbotAutoclick = false;
static int aimbotCooldown = 350;
static std::chrono::time_point<std::chrono::steady_clock> lastClick;
static bool drawMarkers = true;

static std::vector<std::string> blacklist;

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

std::string PositionString(Position* pos)
{
    std::string x = "(";
    if (pos->X == 8) {
        x += "0, ";
    } else if (pos->X < 8) {
        x += std::to_string(8 - pos->X) + " west, ";
    } else if (pos->X > 8) {
        x += std::to_string(pos->X - 8) + " east, ";
    }
    if (pos->Y == 8) {
        x += "0)";
    } else if (pos->Y < 8) {
        x += std::to_string(8 - pos->Y) + " south)";
    } else if (pos->Y > 8) {
        x += std::to_string(pos->Y - 8) + " north)";
    }
    return x;
}

void Draw2DCircle(POINT pt, float radius, D3DCOLOR colour, LPDIRECT3DDEVICE9 dev)
{
    const int NUMPOINTS = 20;
    const float PI = 3.14159;

    D3DTLVERTEX circle[NUMPOINTS + 1];
    int i;
    float X;
    float Y;
    float Theta;
    float WedgeAngle;
    WedgeAngle = (float)((2 * PI) / NUMPOINTS);

    for (i = 0; i <= NUMPOINTS; i++) {
        Theta = i * WedgeAngle;
        X = (float)(pt.x + radius * cos(Theta));
        Y = (float)(pt.y - radius * sin(Theta));

        circle[i] = CreateD3DTLVERTEX(X, Y, 0.0f, 1.0f, colour, 0.0f, 0.0f);
    }

    dev->SetFVF((D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1));
    dev->SetTexture(0, NULL);
    dev->DrawPrimitiveUP(D3DPT_LINESTRIP, NUMPOINTS, &circle[0], sizeof(circle[0]));
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

void OnRender(Aimbot* self, LPDIRECT3DDEVICE9 dev, const RECT* client)
{
    if (rageToggle)
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Rage Active!");
    if (ImGui::TreeNode("Aimbot")) {
        if (aimbotTarget) {
            if (ImGui::Button("Reset")) {
                aimbotTarget = NULL;
                aimbotToggle = false;
            }
            ImGui::SameLine();
            ImGui::Text("Numpad * to reset target.");
            ImGui::Text(("Current Target: " + targetName).c_str());
            if (Calamari::Byond::CidName(*aimbotTarget).empty())
                ImGui::Text("Position: [Off Screen]");
            else {
                auto pos = Calamari::Byond::GetCidTileCoords(*aimbotTarget);
                if (pos->X < 0 || pos->Y < 0 || pos->X > 15 || pos->Y > 15)
                    ImGui::Text("Position: [Off Screen]");
                else
                    ImGui::Text(("Position: " + PositionString(pos.get())).c_str());
            }
        }
        ImGui::Checkbox("Aimbot Toggle (Numpad 0)", &aimbotToggle);
        ImGui::Checkbox("Aimbot Rage (Numpad 1)", &rageToggle);
        ImGui::Checkbox("Autoclick Toggle (Numpad 2)", &aimbotAutoclick);
        //		ImGui::SliderInt("Autoclick Cooldown (ms)", &aimbotCooldown, 50, 1000);
        ImGui::TreePop();
    }
    // Rage overrides normal targeting
    if (rageToggle) {
        auto mobs = Calamari::Byond::GetMobsOnScreen();
        std::vector<std::shared_ptr<Cid>> sorted_mobs;
        sorted_mobs.reserve(mobs.size());
        for (auto mob : mobs) {
            auto name = Calamari::Byond::CidName(*mob.first);
            if (name.empty() || name == "mob")
                continue;
            auto pos = Calamari::Byond::GetCidTileCoords(*mob.first);
            if (!(pos->X >= 7 && pos->X <= 9 && pos->Y >= 7 && pos->Y <= 9))
                continue;
            if (pos->X == 8 && pos->Y == 8)
                continue;
            if (friends.count(mob.first->t2))
                continue;
            sorted_mobs.push_back(mob.first);
        }
        std::sort(sorted_mobs.begin(), sorted_mobs.end(), [](const std::shared_ptr<Cid>& a, const std::shared_ptr<Cid>& b) { return a->t2 > b->t2; });
        if (sorted_mobs.size() > 0) {
            auto rageTarget = sorted_mobs[0];
            if (rageTarget) {
                if (drawMarkers) {
                    auto pos = Calamari::Byond::GetCidTileCoords(*rageTarget);
                    long width = client->right - client->left;
                    long height = client->bottom - client->top;
                    auto vr = Calamari::Byond::GetViewRange();
                    long ix = width / vr->X;
                    long iy = height / vr->Y;
                    POINT ctarget = { ((ix * (pos->X - 1)) + (long)(ix * 0.5)), ((vr->Y * iy) - ((pos->Y * iy) - (long)(iy * 0.5))) };
                    Draw2DCircle(ctarget, (ix + iy) * 0.25, D3DCOLOR_RGBA(255, 0, 0, 255), dev);
                }
                auto now = std::chrono::steady_clock::now();
                auto since = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastClick).count();
                if (since >= aimbotCooldown) {
                    auto pos = Calamari::Byond::GetCidTileCoords(*rageTarget);
                    Calamari::Byond::Click(*rageTarget);
                    lastClick = std::chrono::steady_clock::now();
                }
            }
        }
    } else {
        if (aimbotToggle && aimbotAutoclick && aimbotTarget) {
            auto now = std::chrono::steady_clock::now();
            auto since = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastClick).count();
            if (since >= aimbotCooldown) {
                auto pos = Calamari::Byond::GetCidTileCoords(*aimbotTarget);
                if (pos->X >= 7 && pos->X <= 9 && pos->Y >= 7 && pos->Y <= 9) // Are they adjacent to us?
                {
                    Calamari::Byond::Click(*aimbotTarget);
                    lastClick = std::chrono::steady_clock::now();
                }
            }
        }
        if (aimbotTarget) {
            auto pos = Calamari::Byond::GetCidTileCoords(*aimbotTarget);
            if (drawMarkers && pos->X > 0 && pos->Y > 0 && pos->X < 15 && pos->Y < 15) {
                long width = client->right - client->left;
                long height = client->bottom - client->top;
                auto vr = Calamari::Byond::GetViewRange();
                long ix = width / vr->X;
                long iy = height / vr->Y;
                POINT ctarget = { ((ix * (pos->X - 1)) + (long)(ix * 0.5)), ((vr->Y * iy) - ((pos->Y * iy) - (long)(iy * 0.5))) };
                Draw2DLine({ (long)(width * 0.5), (long)(height * 0.5) }, ctarget, D3DCOLOR_RGBA(255, 0, 0, 255), dev);
                Draw2DCircle(ctarget, (ix + iy) * 0.25, D3DCOLOR_RGBA(255, 0, 0, 255), dev);
            }
        }
    }
}

void OnPostRender(Aimbot* self, LPDIRECT3DDEVICE9 dev, const RECT* client)
{
    ImGui::Begin("Targets");
    auto mobs = Calamari::Byond::GetMobsOnScreen();
    std::vector<std::shared_ptr<Cid>> sorted_mobs;
    sorted_mobs.reserve(mobs.size());
    for (auto mob : mobs)
        sorted_mobs.push_back(mob.first);
    std::sort(sorted_mobs.begin(), sorted_mobs.end(), [](const std::shared_ptr<Cid>& a, const std::shared_ptr<Cid>& b) { return a->t2 > b->t2; });
    auto range = Calamari::Byond::GetViewRange();
    for (auto mob : sorted_mobs) {
        auto pos = Calamari::Byond::GetCidTileCoords(*mob);
        if (pos->X < 0 || pos->X > range->X || pos->Y < 0 || pos->Y > range->Y)
            continue;
        auto name = Calamari::Byond::CidName(*mob);
        if (name.empty() || name == "mob")
            continue;
        auto pcid = Calamari::Byond::PlayerCid();
        if (mob->t2 == pcid->t2)
            continue;
        // add the t2 as an additional ID to the button, to make sure they all work
        if (ImGui::Button(("[T]##" + std::to_string(mob->t2)).c_str())) {
            targetName = name;
            aimbotTarget = new Cid;
            aimbotTarget->t1 = mob->t1;
            aimbotTarget->t2 = mob->t2;
            aimbotToggle = true;
        }
        ImGui::SameLine();
        if (ImGui::Button(("[F]##" + std::to_string(mob->t2)).c_str())) {
            if (friends.count(mob->t2))
                friends.erase(mob->t2);
            else
                friends.insert(std::make_pair(mob->t2, name));
        }
        ImGui::SameLine();
        ImGui::TextColored(((aimbotTarget != NULL && mob->t2 == aimbotTarget->t2) ? ImVec4(1.0f, 0.0f, 0.0f, 1.0f) : friends.count(mob->t2) ? ImVec4(1.0f, 0.6f, 0.7f, 1.0f) : ImVec4(0.6f, 1.0f, 0.6f, 1.0f)), (name + " " + PositionString(pos.get())).c_str());
    }
    ImGui::End();
}

inline bool CanInteract(Cid* cid)
{
    auto pos = Calamari::Byond::GetCidTileCoords(*cid);
    if (cid->t1 == 0x2) {
        if (aimbotTarget && cid->t2 == aimbotTarget->t2)
            return true;
        if (std::find(blacklist.begin(), blacklist.end(), Calamari::Byond::CidName(*cid)) != blacklist.end())
            return false;
        if (pos->X >= 7 && pos->X <= 9 && pos->Y >= 7 && pos->Y <= 9)
            return true;
        if (pos->X < 1 || pos->X > 15 || pos->Y < 1 || pos->Y > 15)
            return true;
    } else if (cid->t1 == 0x1)
        return true;
    return false;
}

Cid* OnClick(Aimbot* self, struct Cid target, int button, std::unordered_map<std::string, bool> modifiers)
{
    if (target.t1 == 0x1 && button == BUTTON_MIDDLE && modifiers["ctrl"] == true) {
        auto name = Calamari::Byond::CidName(target);
        if (name.empty())
            return NULL;
        if (friends.count(target.t2))
            friends.erase(target.t2);
        else
            friends.insert(std::make_pair(target.t2, name));
    } else if (CanInteract(&target) && button == BUTTON_MIDDLE && !friends.count(target.t2)) {
        aimbotToggle = true;
        targetName = Calamari::Byond::CidName(target);
        aimbotTarget = new Cid;
        aimbotTarget->t1 = target.t1;
        aimbotTarget->t2 = target.t2;
    }
    if (aimbotToggle && aimbotTarget != NULL && !CanInteract(&target) && !Calamari::Byond::CidName(*aimbotTarget).empty() && button == BUTTON_LEFT)
        return aimbotTarget;
    return NULL;
}

Cid* OnMouseMove(Aimbot* self, struct Cid target)
{
    if (aimbotToggle && aimbotTarget == NULL && target.t1 == 0x1 && !friends.count(target.t2) && !Calamari::Byond::CidName(target).empty()) {
        auto pcid = Calamari::Byond::PlayerCid();
        if (pcid && pcid->t2 == target.t2)
            return NULL;
        targetName = Calamari::Byond::CidName(target);
        aimbotTarget = new Cid;
        aimbotTarget->t1 = target.t1;
        aimbotTarget->t2 = target.t2;
    }
    return NULL;
}

Cid* OnMouseDown(Aimbot* self, struct Cid target, int button, std::unordered_map<std::string, bool> modifiers)
{
    if (aimbotToggle && aimbotTarget != NULL && !CanInteract(&target) && !Calamari::Byond::CidName(*aimbotTarget).empty() && button == BUTTON_LEFT)
        return aimbotTarget;
    return NULL;
}

Cid* OnMouseUp(Aimbot* self, struct Cid target, int button, std::unordered_map<std::string, bool> modifiers)
{
    if (aimbotToggle && aimbotTarget != NULL && !CanInteract(&target) && !Calamari::Byond::CidName(*aimbotTarget).empty() && button == BUTTON_LEFT)
        return aimbotTarget;
    return NULL;
}

void ToggleAimbot(int key)
{
    aimbotToggle = !aimbotToggle;
}

void ToggleAutoclick(int key)
{
    aimbotAutoclick = !aimbotAutoclick;
}

void ToggleRage(int key)
{
    rageToggle = !rageToggle;
}

void ResetTarget(int key)
{
    if (aimbotTarget) {
        delete aimbotTarget;
        aimbotTarget = NULL;
    }
    rageToggle = false;
    aimbotToggle = false;
}

Aimbot::Aimbot()
    : Plugin()
{
    lastClick = std::chrono::steady_clock::now();

    AddEvent(EVENT_RENDER, &OnRender, false);
    AddEvent(EVENT_LATERENDER, &OnPostRender, false);
    AddEvent(EVENT_CLICK, &OnClick, true);
    AddEvent(EVENT_MOUSEDOWN, &OnMouseDown, true);
    AddEvent(EVENT_MOUSEUP, &OnMouseUp, true);
    AddEvent(EVENT_MOUSEMOVE, &OnMouseMove, false);

    AddKeyHandler("aimbot toggle", VK_NUMPAD0, &ToggleAimbot);
    AddKeyHandler("aimbot reset", VK_MULTIPLY, &ResetTarget);
    AddKeyHandler("rage toggle", VK_NUMPAD1, &ToggleRage);
    AddKeyHandler("autoclick toggle", VK_NUMPAD2, &ToggleAutoclick);

    std::string bl = GetConfig("Blacklist");
    if (bl.empty()) {
        bl = "grille,window,reinforced window,disposal pipe,power cable,scrubbers pipe,air supply pipe,firelock,security camera,footprints,blood,drips of blood,plasma window,reinforced plasma window,holopad,interior door,disposal unit,chair,comfy chair,bed,table,reinforced table,gambling table,stool,wooden table,rack,catwalk,lattice,chempile";
        SetConfig("Blacklist", bl);
    }
    std::string dm = GetConfig("DrawMarkers");
    if (dm.empty()) {
        dm = "true";
        SetConfig("DrawMarkers", dm);
    }
    std::istringstream(dm) >> std::boolalpha >> drawMarkers;
    size_t pos = 0;
    std::string token;
    std::string delimiter(",");
    while ((pos = bl.find(delimiter)) != std::string::npos) {
        token = bl.substr(0, pos);
        blacklist.push_back(token);
        bl.erase(0, pos + delimiter.length());
    }
}

std::string Aimbot::GetName()
{
    return "Aimbot";
}

void* Aimbot::IPCRecv(Plugin* from, void* data)
{
    AimbotIPC* ipc = (AimbotIPC*)data;
    switch (ipc->msg) {
    case AIMBOT_GET_TARGET:
        return (void*)aimbotTarget;
    case AIMBOT_SET_TARGET:
        if (data) {
            Cid* cid = (Cid*)data;
            aimbotTarget = new Cid;
            aimbotTarget->t1 = cid->t1;
            aimbotTarget->t2 = cid->t2;
            aimbotToggle = true;
        }
        break;
    case AIMBOT_UNSET_TARGET:
        delete aimbotTarget;
        aimbotTarget = NULL;
        aimbotToggle = false;
        break;
    case AIMBOT_ADD_FRIEND:
        if (data) {
            Cid* cid = (Cid*)data;
            if (friends.count(cid->t2))
                break;
            friends.insert(std::make_pair(cid->t2, Calamari::Byond::CidName(*cid)));
        }
        break;
    case AIMBOT_REMOVE_FRIEND:
        if (data) {
            Cid* cid = (Cid*)data;
            if (!friends.count(cid->t2))
                break;
            friends.erase(cid->t2);
        }
        break;
    case AIMBOT_GET_FRIENDS:
        std::vector<Cid> ret;
        ret.reserve(friends.size());
        for (auto fr : friends) {
            Cid friendCid = { 0x1, fr.first };
            ret.push_back(friendCid);
        }
        return &ret;
    }
    return nullptr;
}

extern "C" __declspec(dllexport) Aimbot* InitializePlugin()
{
    return new Aimbot;
}
