#include "radar.h"

static auto playerColor = ImColor(255, 255, 0);
static auto friendColor = ImColor(255, 105, 180);
static auto targetColor = ImColor(255, 0, 0);
static auto mobColor = ImColor(152, 251, 152);
static auto highPrioColor = ImColor(173, 33, 33);
static auto lowPrioColor = ImColor(172, 255, 15);
static bool showTextRadar = true;
static bool showVisualRadar = true;
static bool showInvalidPos = false;
static std::vector<std::string> highPrio;
static std::vector<std::string> lowPrio;

inline struct ImColor HexToImColor(std::string hex)
{
    int rgb = std::stoi(hex, 0, 16);
    return ImColor(rgb / 0x10000, (rgb / 0x100) % 0x100, rgb % 0x100);
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

void OnRender(Radar* self, LPDIRECT3DDEVICE9 dev, const RECT* client)
{
    if (ImGui::TreeNode("Radar")) {
        ImGui::Checkbox("Item Radar", &showTextRadar);
        ImGui::Checkbox("Visual Radar", &showVisualRadar);
        ImGui::TreePop();
    }
}

void PostRender(Radar* self, LPDIRECT3DDEVICE9 dev, const RECT* client)
{
    if (!showTextRadar && !showVisualRadar)
        return;
    std::vector<std::shared_ptr<Cid>> high;
    std::vector<std::shared_ptr<Cid>> low;
    auto objs = Calamari::Byond::GetObjectsOnScreen();
    auto mobs = Calamari::Byond::GetMobsOnScreen();
    std::vector<std::shared_ptr<Cid>> sorted_objs;
    std::vector<std::shared_ptr<Cid>> sorted_mobs;
    sorted_objs.reserve(objs.size());
    sorted_mobs.reserve(mobs.size());
    for (auto obj : objs)
        sorted_objs.push_back(obj.first);
    for (auto mob : mobs)
        sorted_mobs.push_back(mob.first);
    std::sort(sorted_objs.begin(), sorted_objs.end(), [](const std::shared_ptr<Cid>& a, const std::shared_ptr<Cid>& b) { return a->t2 > b->t2; });
    std::sort(sorted_mobs.begin(), sorted_mobs.end(), [](const std::shared_ptr<Cid>& a, const std::shared_ptr<Cid>& b) { return a->t2 > b->t2; });
    auto range = Calamari::Byond::GetViewRange();
    for (auto obj : sorted_objs) {
        auto pos = Calamari::Byond::GetCidTileCoords(*obj);
        if (!showInvalidPos && (pos->X < 0 || pos->X > range->X || pos->Y < 0 || pos->Y > range->Y))
            continue;
        auto name = Calamari::Byond::CidName(*obj);
        if (name.empty())
            continue;
        if (std::find(highPrio.begin(), highPrio.end(), name) != highPrio.end())
            high.push_back(obj);
        else if (std::find(lowPrio.begin(), lowPrio.end(), name) != lowPrio.end())
            low.push_back(obj);
    }
    std::sort(high.begin(), high.end(), [](const std::shared_ptr<Cid>& a, const std::shared_ptr<Cid>& b) { return a->t2 > b->t2; });
    std::sort(low.begin(), low.end(), [](const std::shared_ptr<Cid>& a, const std::shared_ptr<Cid>& b) { return a->t2 > b->t2; });
    if (showTextRadar) {
        ImGui::Begin("Item Radar");
        for (auto obj : high) {
            auto name = Calamari::Byond::CidName(*obj);
            ImGui::TextColored(highPrioColor, (name + " " + PositionString(Calamari::Byond::GetCidTileCoords(*obj).get())).c_str());
        }
        for (auto obj : low) {
            auto name = Calamari::Byond::CidName(*obj);
            ImGui::TextColored(lowPrioColor, (name + " " + PositionString(Calamari::Byond::GetCidTileCoords(*obj).get())).c_str());
        }
        ImGui::End();
    }
    if (showVisualRadar) {
        ImGui::Begin("Visual Radar");
        std::vector<long> friends;
        long target = 0;
        AimbotIPC ipc = { AIMBOT_GET_FRIENDS, NULL };
        void* f = self->IPCSend("Aimbot", (void*)&ipc);
        if (f != nullptr) {
            std::vector<Cid> fr = *(std::vector<Cid>*)f;
            friends.reserve(fr.size());
            for (auto fri : fr)
            {
                friends.push_back(fri.t2);
            }
        }
        ipc = { AIMBOT_GET_TARGET, NULL };
        void* t = self->IPCSend("Aimbot", (void*)&ipc);
        if (t != nullptr) {
            target = ((Cid*)t)->t2;
        }
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        ImVec2 winpos = ImGui::GetWindowPos();
        ImVec2 winsize = ImGui::GetWindowSize();
        auto me = Calamari::Byond::PlayerCid();

        int gridX = winsize.x / (range->X + 1);
        int gridY = winsize.y / (range->Y + 1);

        for (auto mob : sorted_mobs) {
            auto pos = Calamari::Byond::GetCidTileCoords(*mob);
            if (pos->X < 0 || pos->X > range->X || pos->Y < 0 || pos->Y > range->Y)
                continue;
            struct ImColor color;
            if (mob->t2 == me->t2)
                color = playerColor;
            else if(mob->t2 == target)
                color = targetColor;
            else if(std::find(friends.begin(), friends.end(), mob->t2) != friends.end())
                color = friendColor;
            else
                color = mobColor;
            
            draw_list->AddRectFilled(ImVec2(winpos.x + (gridX * pos->X), winpos.y + (gridY * ((range->Y - pos->Y) + 1))),
                ImVec2((gridX * pos->X) + gridX + winpos.x, (gridY * ((range->Y - pos->Y) + 1)) + gridY + winpos.y),
                color, 0.0f, 0);
        }
        for (auto obj : high) {
            auto pos = Calamari::Byond::GetCidTileCoords(*obj);
            draw_list->AddRectFilled(ImVec2(winpos.x + (gridX * pos->X), winpos.y + (gridY * ((range->Y - pos->Y) + 1))),
                ImVec2((gridX * pos->X) + gridX + winpos.x, (gridY * ((range->Y - pos->Y) + 1)) + gridY + winpos.y),
                highPrioColor, 0.0f, 0);
        }
        for (auto obj : low) {
            auto pos = Calamari::Byond::GetCidTileCoords(*obj);
            draw_list->AddRectFilled(ImVec2(winpos.x + (gridX * pos->X), winpos.y + (gridY * ((range->Y - pos->Y) + 1))),
                ImVec2((gridX * pos->X) + gridX + winpos.x, (gridY * ((range->Y - pos->Y) + 1)) + gridY + winpos.y),
                lowPrioColor, 0.0f, 0);
        }
        ImGui::End();
    }
}

Radar::Radar()
    : Plugin()
{
    std::string hpr = GetConfig("HighPriorityRadar");
    if (hpr.empty()) {
        hpr = "nuclear authentication disk,hand tele,advanced magboots,hypospray,antique laser gun,captain\\'s spare ID,ritual dagger,clockwork slab,pylon,pinpointer,captain\\'s jetpack,reactive teleport armor,plutonium core,supermatter sliver,station blueprints";
        SetConfig("HighPriorityRadar", hpr);
    }
    std::string lpr = GetConfig("LowPriorityRadar");
    if (lpr.empty()) {
        lpr = "insulated gloves,optical meson goggles,toolbelt,advanced sunglasses";
        SetConfig("LowPriorityRadar", lpr);
    }
    std::string dm = GetConfig("ShowInvalidPos");
    if (dm.empty()) {
        dm = "false";
        SetConfig("ShowInvalidPos", dm);
    }
    std::string pCol = GetConfig("PlayerColor");
    if (pCol.empty()) {
        pCol = "FFFF00";
        SetConfig("PlayerColor", pCol);
    }
    std::string fCol = GetConfig("AimbotFriendColor");
    if (fCol.empty()) {
        fCol = "FF69B4";
        SetConfig("AimbotFriendColor", fCol);
    }
    std::string mCol = GetConfig("MobColor");
    if (mCol.empty()) {
        mCol = "98FB98";
        SetConfig("MobColor", mCol);
    }
    std::string tCol = GetConfig("AimbotTargetColor");
    if (tCol.empty()) {
        tCol = "FF0000";
        SetConfig("AimbotTargetColor", tCol);
    }
    std::string hpiCol = GetConfig("HighPriorityItemColor");
    if (hpiCol.empty()) {
        hpiCol = "B22121";
        SetConfig("HighPriorityItemColor", hpiCol);
    }
    std::string lpiCol = GetConfig("LowPriorityItemColor");
    if (lpiCol.empty()) {
        lpiCol = "ACFF0F";
        SetConfig("LowPriorityItemColor", lpiCol);
    }
    playerColor = HexToImColor(pCol);
    friendColor = HexToImColor(fCol);
    mobColor = HexToImColor(mCol);
    targetColor = HexToImColor(tCol);
    highPrioColor = HexToImColor(hpiCol);
    lowPrioColor = HexToImColor(lpiCol);
    std::istringstream(dm) >> std::boolalpha >> showInvalidPos;
    AddEvent(EVENT_RENDER, &OnRender, false);
    AddEvent(EVENT_LATERENDER, &PostRender, false);
    size_t pos = 0;
    std::string token;
    std::string delimiter(",");
    while ((pos = hpr.find(delimiter)) != std::string::npos) {
        token = hpr.substr(0, pos);
        highPrio.push_back(token);
        hpr.erase(0, pos + delimiter.length());
    }
    pos = 0;
    while ((pos = lpr.find(delimiter)) != std::string::npos) {
        token = lpr.substr(0, pos);
        lowPrio.push_back(token);
        lpr.erase(0, pos + delimiter.length());
    }
}

std::string Radar::GetName()
{
    return "Radar";
}

extern "C" __declspec(dllexport) Radar* InitializePlugin()
{
    return new Radar;
}
