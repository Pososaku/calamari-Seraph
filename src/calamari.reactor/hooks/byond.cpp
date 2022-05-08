#include "hooks.h"

inline int ConvertToButton(char b)
{
    switch (b) {
    case 1:
        return BUTTON_LEFT;
    case 4:
        return BUTTON_MIDDLE;
    case 0x9:
        return BUTTON_LEFT;
    }
    return BUTTON_LEFT;
}

inline std::string ConvertToButtonStr(char b)
{
    switch (b) {
    case 1:
        return "left";
    case 4:
        return "middle";
    case 0x9:
        return "left";
    }
    return "left";
}

inline std::unordered_map<std::string, bool> CreateButtonList(const char* thing)
{
    std::unordered_map<std::string, bool> out;
    out["alt"] = false;
    out["ctrl"] = false;
    out["shift"] = false;
    if (strstr(thing, "alt"))
        out["alt"] = true;
    if (strstr(thing, "ctrl"))
        out["ctrl"] = true;
    if (strstr(thing, "shift"))
        out["shift"] = true;
    return out;
}

int __fastcall Calamari::Hooks::MouseDown(void* DungClient, void* DONOTUSE, struct Cid a1, char const* a2, short a3, short a4, short a5, short a6, char a7, char const* a8, char const* a9)
{
    VMP_BEGIN("Calamari::Hooks::MouseDown");
    if (Calamari::Byond::DungClient == NULL)
        Calamari::Byond::DungClient = DungClient;
    Cid* newCid = NULL;
    std::string params;
    int button = ConvertToButton(a7);
    auto button_s = ConvertToButtonStr(a7);
    auto mods = CreateButtonList(a8);
    for (EventCall* event : Calamari::Reactor::Events) {
        if (event->Event == EVENT_MOUSEDOWN) {
            try {
                Cid* x = ((MouseDownEvent)event->Function)(event->Plugin, a1, button, mods);
                if (event->Overriding && x)
                    newCid = x;
            } catch (std::exception& e) {
                Calamari::Reactor::Log("Event '" + event->Event + "' from plugin '" + event->Plugin->Name + "' errored: " + e.what());
            }
        }
    }
    if (newCid != NULL) {
        auto coords = Calamari::Byond::GetCidTileCoords(*newCid);
        params.clear();
        params += VMPSTR("icon-x=") + std::to_string(a5);
        params += VMPSTR(";icon-y=") + std::to_string(a6);
        params += ";" + button_s + "=1";
        for (std::pair<std::string, bool> modifier : mods) {
            if (modifier.second == true)
                params += ";" + modifier.first + "=1";
        }
        params += VMPSTR(";screen-loc=") + std::to_string(coords->X) + ":" + std::to_string(a5) + "," + std::to_string(coords->Y) + ":" + std::to_string(a6);
        return Calamari::Byond::GenMouseDownCommandFunc(DungClient, *newCid, a2, a3, a4, a5, a6, a7, params.c_str(), a9);
    } else
        return Calamari::Byond::GenMouseDownCommandFunc(DungClient, a1, a2, a3, a4, a5, a6, a7, a8, a9);
    VMP_END();
}

int __fastcall Calamari::Hooks::MouseUp(void* DungClient, void* DONOTUSE, struct Cid a1, char const* a2, short a3, short a4, short a5, short a6, char a7, char const* a8, char const* a9)
{
    VMP_BEGIN("Calamari::Hooks::MouseUp");
    if (Calamari::Byond::DungClient == NULL)
        Calamari::Byond::DungClient = DungClient;
    Cid* newCid = NULL;
    std::string params;
    int button = ConvertToButton(a7);
    auto button_s = ConvertToButtonStr(a7);
    auto mods = CreateButtonList(a8);
    for (EventCall* event : Calamari::Reactor::Events) {
        if (event->Event == EVENT_MOUSEUP) {
            try {
                Cid* x = ((MouseUpEvent)event->Function)(event->Plugin, a1, button, mods);
                if (event->Overriding && x)
                    newCid = x;
            } catch (std::exception& e) {
                Calamari::Reactor::Log("Event '" + event->Event + "' from plugin '" + event->Plugin->Name + "' errored: " + e.what());
            }
        }
    }
    if (newCid != NULL) {
        auto coords = Calamari::Byond::GetCidTileCoords(*newCid);
        params.clear();
        params += VMPSTR("icon-x=") + std::to_string(a5);
        params += VMPSTR(";icon-y=") + std::to_string(a6);
        params += ";" + button_s + "=1";
        for (std::pair<std::string, bool> modifier : mods) {
            if (modifier.second == true)
                params += ";" + modifier.first + "=1";
        }
        params += VMPSTR(";screen-loc=") + std::to_string(coords->X) + ":" + std::to_string(a5) + "," + std::to_string(coords->Y) + ":" + std::to_string(a6);
        return Calamari::Byond::GenMouseUpCommandFunc(DungClient, *newCid, a2, a3, a4, a5, a6, a7, params.c_str(), a9);
    } else
        return Calamari::Byond::GenMouseUpCommandFunc(DungClient, a1, a2, a3, a4, a5, a6, a7, a8, a9);
    VMP_END();
}

int __fastcall Calamari::Hooks::Click(void* DungClient, void* DONOTUSE, struct Cid a1, char const* a2, short a3, short a4, short a5, short a6, char a7, char const* a8, char const* a9)
{
    VMP_BEGIN("Calamari::Hooks::Click");
    if (Calamari::Byond::DungClient == NULL)
        Calamari::Byond::DungClient = DungClient;
    Cid* newCid = NULL;
    std::string params;
    int button = ConvertToButton(a7);
    auto button_s = ConvertToButtonStr(a7);
    auto mods = CreateButtonList(a8);
    for (EventCall* event : Calamari::Reactor::Events) {
        if (event->Event == EVENT_CLICK) {
            try {
                Cid* x = ((ClickEvent)event->Function)(event->Plugin, a1, button, mods);
                if (event->Overriding && x)
                    newCid = x;
            } catch (std::exception& e) {
                Calamari::Reactor::Log(VMPSTR("Event '") + event->Event + VMPSTR("' from plugin '") + event->Plugin->Name + VMPSTR("' errored: ") + e.what());
            }
        }
    }
    if (newCid != NULL) {
        auto coords = Calamari::Byond::GetCidTileCoords(*newCid);
        params.clear();
        params += VMPSTR("icon-x=") + std::to_string(a5);
        params += VMPSTR(";icon-y=") + std::to_string(a6);
        params += ";" + button_s + "=1";
        for (std::pair<std::string, bool> modifier : mods) {
            if (modifier.second == true)
                params += ";" + modifier.first + "=1";
        }
        params += VMPSTR(";screen-loc=") + std::to_string(coords->X) + ":" + std::to_string(a5) + "," + std::to_string(coords->Y) + ":" + std::to_string(a6);
        return Calamari::Byond::GenClickCommandFunc(DungClient, *newCid, a2, a3, a4, a5, a6, a7, params.c_str(), a9);
    } else
        return Calamari::Byond::GenClickCommandFunc(DungClient, a1, a2, a3, a4, a5, a6, a7, a8, a9);
    VMP_END();
}

int __fastcall Calamari::Hooks::MouseMove(void* DungClient, void* DONOTUSE, struct Cid a1, char const* a2, short a3, short a4, short a5, short a6, char a7, char const* a8, char const* a9)
{
    VMP_BEGIN("Calamari::Hooks::MouseMove");
    if (Calamari::Byond::DungClient == NULL)
        Calamari::Byond::DungClient = DungClient;
    for (EventCall* event : Calamari::Reactor::Events) {
        if (event->Event == EVENT_MOUSEMOVE) {
            try {
                ((MouseMoveEvent)event->Function)(event->Plugin, a1);
            } catch (std::exception& e) {
                Calamari::Reactor::Log(VMPSTR("Event '") + event->Event + VMPSTR("' from plugin '") + event->Plugin->Name + VMPSTR("' errored: ") + e.what());
            }
        }
    }
    return Calamari::Byond::GenMouseMoveCommandFunc(DungClient, a1, a2, a3, a4, a5, a6, a7, a8, a9);
    VMP_END();
}

void* __fastcall Calamari::Hooks::DungClientCtor(void* pThis)
{
    VMP_BEGIN("Calamari::Hooks::DungClientCtor");
    Calamari::Byond::DungClient = pThis;
    return Calamari::Byond::DungClientCtor(pThis);
    VMP_END();
}

bool __fastcall Calamari::Hooks::IsByondMember(void* DungClient, void* DONOTUSE, long who_cares) {
    return true;
}

void __fastcall Calamari::Hooks::Stat(void* pThis, void* DONOTUSE, char const* a1, char const* a2, struct Cid a3, unsigned short a4)
{
    VMP_BEGIN("Calamari::Hooks::DSAddStat");
    for (EventCall* event : Calamari::Reactor::Events) {
        if (event->Event == EVENT_STAT) {
            try {
                ((StatEvent)event->Function)(event->Plugin, std::string(a1), std::string(a2));
            } catch (std::exception& e) {
                Calamari::Reactor::Log(VMPSTR("Event '") + event->Event + VMPSTR("' from plugin '") + event->Plugin->Name + VMPSTR("' errored: ") + e.what());
            }
        }
    }
    Calamari::Byond::DSAddStatFunc(pThis, a1, a2, a3, a4);
    VMP_END();
}

int __fastcall Calamari::Hooks::PutText(void* pThis, void* DONOTUSE, char* a1, char* a2)
{
    VMP_BEGIN("Calamari::Hooks::PutText");
    std::string text(a1);
    if (text.find(VMPSTR("<div class=\"connectionClosed internal restarting\">The connection has been closed because the server is restarting. Please wait while you automatically reconnect.</div>")) != std::string::npos \
		|| text.find(VMPSTR("<span class='boldannounce'>Rebooting World.")) != std::string::npos) {
		Calamari::Reactor::Log(VMPSTR("Detected world reboot, clearing cid cache."));
        Calamari::Reactor::ResetCidCache();
    }
    for (EventCall* event : Calamari::Reactor::Events) {
        if (event->Event == EVENT_PUTTEXT) {
            try {
                ((PutTextEvent)event->Function)(event->Plugin, text);
            } catch (std::exception& e) {
                Calamari::Reactor::Log(VMPSTR("Event '") + event->Event + VMPSTR("' from plugin '") + event->Plugin->Name + VMPSTR("' errored: ") + e.what());
            }
        }
    }
    return Calamari::Byond::CIOutputCtrlPutTextFunc(pThis, a1, a2);
    VMP_END();
}

unsigned int __cdecl Calamari::Hooks::Byond32(unsigned int a1, unsigned int a2, unsigned int a3)
{
    VMP_BEGIN("Calamari::Hooks::Byond32");
    /*if (a3 == 32)
    {
        int* a = (int*)a2;
        *a = 1864845531;
        printf("overriding\n");
        return 1864845531;
    }*/
    unsigned int ret = Calamari::Byond::Byond32Func(a1, a2, a3);
#ifdef DEBUG_CONSOLE
    //printf("%u, %u, %u = %u\n", a1, a2, a3, ret);
#endif
    return ret;
    VMP_END();
}

#ifdef PACKET_DEBUGGER
int __fastcall Calamari::Hooks::Decode(void* NetMsg, void* DONOTUSE, unsigned int key)
{
    printf("decode key %u\n", key);
    return NetMsgDecodeFunc(NetMsg, key);
}
#endif
