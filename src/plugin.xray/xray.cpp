#include "../calamari.public/events.h"
#include "../calamari.public/plugin.h"
#include "../common/Pocket.h"
#include "../imgui/imgui.h"
#include <iomanip>
#include <sstream>

static DWORD xrayLoc = NULL;
static bool xrayToggle = false;
static int xrayMode = 0;

void OnRender()
{
    if (xrayLoc) {
        DWORD sight = *(DWORD*)xrayLoc;
        DWORD* xray = (DWORD*)(sight + 0xA0);
        ImGui::Text("Xray");
        if (ImGui::RadioButton("Off", xrayMode == 0))
            xrayMode = 0;
        ImGui::SameLine();
        if (ImGui::RadioButton("Xray", xrayMode == 1))
            xrayMode = 1;
        ImGui::SameLine();
        if (ImGui::RadioButton("Extended", xrayMode == 2))
            xrayMode = 2;
        switch (xrayMode) {
        case 0:
            *xray = 0;
            break;
        case 1:
            *xray = 60;
            break;
        case 2:
            *xray = 894;
            break;
        }
    }
}

void ToggleXray()
{
    xrayMode++;
    if (xrayMode > 2)
        xrayMode = 0;
}

class Xray : public Plugin {
public:
    Xray()
        : Plugin()
    {
        xrayLoc = *(DWORD*)((DWORD)Pocket::Sigscan::FindPattern("byondcore.dll", "89 3D ?? ?? ?? ?? E8 ?? ?? ?? ?? 8B 47 ??", 2));
        if (xrayLoc) {
            std::stringstream ss;
            ss << std::hex << (int)xrayLoc;
            Log("Found SIGHT at 0x" + ss.str());
        } else
            Log("Failed to find SIGHT! (BYOND version " + Calamari::Byond::ByondVersion() + ")");
        AddEvent(EVENT_RENDER, &OnRender, false);
        AddKeyHandler("xray toggle", VK_DECIMAL, &ToggleXray);
    }

    std::string GetName() override
    {
        return "Xray";
    }
};

extern "C" __declspec(dllexport) Xray* InitializePlugin()
{
    return new Xray;
}
