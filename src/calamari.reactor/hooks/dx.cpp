#include "../../common/util.h"
#include "../../imgui/imgui.h"
#include "../../imgui/imgui_impl_dx9.h"
#include "../../imgui/imgui_impl_win32.h"
#include "../../imgui/imgui_internal.h"
#include "hooks.h"
#include <d3d9.h>
#include <filesystem>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

DXReset Calamari::Hooks::ResetFunc = NULL;
DXEndScene Calamari::Hooks::EndSceneFunc = NULL;
DXPresent Calamari::Hooks::PresentFunc = NULL;
WNDPROC Calamari::Hooks::WndProcFunc = NULL;
bool Calamari::Hooks::DXInitialized = false;

inline void DestroyImgui()
{
    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext(ImGui::GetCurrentContext());
}

inline void ImguiStyle()
{
    ImGuiStyle& style = ImGui::GetStyle();
    style.Alpha = 0.75f;
    style.WindowRounding = 5.0f;
    style.FramePadding = ImVec2(5, 5);
    style.FrameRounding = 4.0f;
    style.ItemSpacing = ImVec2(12, 8);
    style.ItemInnerSpacing = ImVec2(8, 6);
    style.IndentSpacing = 25.0f;
    style.ScrollbarSize = 15.0f;
    style.ScrollbarRounding = 9.0f;
    style.GrabMinSize = 5.0f;
    style.GrabRounding = 3.0f;
    // COLORS
    ImVec4* colors = style.Colors;
    colors[ImGuiCol_Text] = ImVec4(0.86f, 0.93f, 0.89f, 0.78f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.86f, 0.93f, 0.89f, 0.28f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.07f, 0.07f, 0.07f, 1.00f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.13f, 0.13f, 0.13f, 0.58f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.13f, 0.13f, 0.13f, 0.90f);
    colors[ImGuiCol_Border] = ImVec4(0.08f, 0.08f, 0.08f, 0.60f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.13f, 0.13f, 0.13f, 0.00f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.13f, 0.13f, 0.13f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.50f, 0.50f, 0.50f, 0.78f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.13f, 0.13f, 0.13f, 0.75f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.13f, 0.13f, 0.13f, 0.47f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.13f, 0.13f, 0.13f, 1.00f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.50f, 0.50f, 0.50f, 0.78f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.23f, 0.23f, 0.23f, 1.00f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.33f, 0.33f, 0.33f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.50f, 0.50f, 0.50f, 0.76f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.50f, 0.50f, 0.50f, 0.86f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);
    colors[ImGuiCol_Separator] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.47f, 0.77f, 0.83f, 0.04f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.50f, 0.50f, 0.50f, 0.78f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_Tab] = ImVec4(0.13f, 0.13f, 0.13f, 0.40f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);
    colors[ImGuiCol_TabActive] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.13f, 0.13f, 0.13f, 0.40f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.13f, 0.13f, 0.13f, 0.70f);
    colors[ImGuiCol_PlotLines] = ImVec4(0.86f, 0.93f, 0.89f, 0.63f);
    colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_PlotHistogram] = ImVec4(0.86f, 0.93f, 0.89f, 0.63f);
    colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.50f, 0.50f, 0.50f, 0.43f);
    colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
    colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.13f, 0.13f, 0.13f, 0.73f);
}

inline void SetupImgui()
{
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    RECT display;
    GetClientRect(Calamari::Byond::DirectXWnd, &display);
    io.DisplaySize.x = display.right;
    io.DisplaySize.y = display.bottom;
    if(std::filesystem::exists("font.ttf"))
        io.Fonts->AddFontFromFileTTF("font.ttf", 14);

    ImGui_ImplWin32_Init(Calamari::Byond::DirectXWnd);
    ImGui_ImplDX9_Init(Calamari::Byond::DXDevice);
    ImGui_ImplDX9_InvalidateDeviceObjects();
    ImGui_ImplDX9_CreateDeviceObjects();
}

inline void Render(const RECT* client)
{
    ImGui_ImplDX9_NewFrame();
    ImGui::NewFrame();
    ImGui::Begin(VMPSTR("Calamari-X"));
    ImguiStyle();
    if (!Calamari::Reactor::Username.empty())
        ImGui::Text((VMPSTR("Registered to ") + Calamari::Reactor::Username).c_str());
    for (EventCall* event : Calamari::Reactor::Events) {
        if (event->Event == EVENT_RENDER) {
            try {
                ((RenderEvent)event->Function)(event->Plugin, Calamari::Byond::DXDevice, client);
            } catch (std::exception& e) {
                Calamari::Reactor::Log(VMPSTR("Event '") + event->Event + VMPSTR("' from plugin '") + event->Plugin->Name + VMPSTR("' errored: ") + e.what());
            }
        }
    }
    ImGui::End();
    for (EventCall* event : Calamari::Reactor::Events) {
        if (event->Event == EVENT_LATERENDER) {
            try {
                ((LateRenderEvent)event->Function)(event->Plugin, Calamari::Byond::DXDevice, client);
            } catch (std::exception& e) {
                Calamari::Reactor::Log(VMPSTR("Event '") + event->Event + VMPSTR("' from plugin '") + event->Plugin->Name + VMPSTR("' errored: ") + e.what());
            }
        }
    }
    if (!Calamari::Reactor::HideGui) {
        ImGui::Render();
        ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
    } else {
        ImGui::EndFrame();
    }
}

long __stdcall Calamari::Hooks::Present(LPDIRECT3DDEVICE9 device, const RECT* pSourceRect, const RECT* pDestRect, HWND hDestWindowOverride, const RGNDATA* pDirtyRegion)
{
    VMP_BEGIN("Calamari::Hooks::Present");
#ifdef USE_BLACKLIST
    if (!Calamari::Reactor::HasChecked && Calamari::Byond::DungClient != NULL) {
        Calamari::Reactor::HasChecked = true;
        if (Calamari::Reactor::IsBlacklisted())
            Calamari::Reactor::Unhook();
        return 0;
    }
#endif
    Calamari::Byond::DXDevice = device;
    if (!DXInitialized) {
        SetupImgui();
        DXInitialized = true;
    }
    auto vr = Calamari::Byond::GetViewRange();
    Calamari::Reactor::PollKeys();
    if (!Calamari::Reactor::HasThreaded) {
        Calamari::Reactor::HasThreaded = true;
        std::thread(Calamari::Reactor::ObjScraperThread).detach();
        std::thread(Calamari::Reactor::MobScraperThread).detach();
        Calamari::Reactor::Log("[Calamari] Spawned object/mob scraping threads!");
    }
    Render(pDestRect);
    return PresentFunc(device, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
    VMP_END();
}

long __stdcall Calamari::Hooks::EndScene(LPDIRECT3DDEVICE9 device)
{
    VMP_BEGIN("Calamari::Hooks::EndScene");
    Calamari::Byond::DXDevice = device;
    return EndSceneFunc(device);
    VMP_END();
}

long __stdcall Calamari::Hooks::Reset(LPDIRECT3DDEVICE9 device, D3DPRESENT_PARAMETERS* param)
{
    VMP_BEGIN("Calamari::Hooks::ResetHook");
    if (!DXInitialized)
        return ResetFunc(device, param);
    DestroyImgui();
    long ret = ResetFunc(device, param);
    Calamari::Byond::DXDevice = device;
    FindDirectXWindow(FindMainWindow(GetCurrentProcessId()));
    SetupImgui();
    return ret;
    VMP_END();
}

LRESULT Calamari::Hooks::WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    VMP_BEGIN("Calamari::Hooks::WndProc");
    switch (uMsg) {
    case WM_SIZE:
        if (Calamari::Byond::DXDevice != NULL && wParam != SIZE_MINIMIZED) {
            ImGui_ImplDX9_InvalidateDeviceObjects();
            ImGui_ImplDX9_CreateDeviceObjects();
        }
        break;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return false;
        break;
    }

    if (Calamari::Reactor::GuiInteraction && ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam))
        return false;

    return CallWindowProcA(WndProcFunc, hwnd, uMsg, wParam, lParam);
    VMP_END();
}
