// ImGui
#pragma once
#include <imgui.h>
#include <imgui_impl_dx9.h>
#include <imgui_impl_win32.h>
#include "../utils/sampapi-i.h"
// Hooks
#include <kthook/kthook.hpp>
#include "../main.h"
#include "../recorder/recorder.h"
#include <ranges>

class menu {
public:
    static void ShowCursorImGUI_LOOP();
    static void styleImGui();
    static void DrawMenu();
};


enum class Tab {
    None,
    Capture,
    Lovlya,
    Bots,
    Cheats,
    Theme
};

enum class SubTab {
    NoneSub,
    TimerAndTableCapture,
    ClanCheckerAndClistChecker,
    Settings
};

inline Tab activeTab = Tab::None;
inline SubTab activeSubTab = SubTab::NoneSub;

inline void captilka_menu() {
    ImGui::InputTextWithHint("Input CMD", "/capture_biz", settings.cmd, 20);
    ImGui::InputTextWithHint("Enter time", "hour min sec ms", settings.captureTimer, 13);
    ImGui::Checkbox("Start the capture", &settings.captureEnable);
    if (settings.captureEnable) {
        SYSTEMTIME st;
        GetLocalTime(&st);
        ImGui::Text("Time: %s / %02d %02d %02d %03d", settings.captureTimer, st.wHour, st.wMinute, st.wSecond,
                    st.wMilliseconds);
    }
    ImGui::SeparatorText("CAPTURE TIMER");
    ImGui::Checkbox("Capture table (packets + chat)", &settings.captureTable);
}

inline void clanchecker_menu() {
    ImGui::Checkbox("Clist Filter", &settings.clistFilterEnable);
    if (settings.clistFilterEnable) {
        ImGui::SetNextWindowSize({500, 500});
        ImGui::Begin("clist filter | by yorenov", &settings.ImGuiEnable,
                 ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoResize);
        ImGui::InputInt("Id player", &settings.idFindClist);
        static std::vector<sampapi::ID> accepted_players;
        if (ImGui::Button("Click")) {
            accepted_players.clear();

            if (samp::RefNetGame()->GetPlayerPool()->IsConnected(settings.idFindClist)) {
                const auto RequiredPlayerColor =
                        samp::RefNetGame()->GetPlayerPool()->GetPlayer(settings.idFindClist)->GetColorAsRGBA();
                for (const auto i : std::ranges::iota_view<sampapi::ID, sampapi::ID>(0, 1000)) {
                    if (!samp::RefNetGame()->GetPlayerPool()->IsConnected(i)) {
                        continue;
                    }

                    if (RequiredPlayerColor != samp::RefNetGame()->GetPlayerPool()->GetPlayer(i)->GetColorAsRGBA())
                        continue;

                    accepted_players.emplace_back(i);
                }
            }
        }

        ImGui::BeginChild("##list_of_accepted");

        ImGui::Text("List of accepted players:");
        for (const auto id : accepted_players) {
            if (!samp::RefNetGame()->GetPlayerPool()->IsConnected(id))
                continue;
            ImGui::Text("%s[%d]", samp::RefNetGame()->GetPlayerPool()->GetName(id), id);
        }

        ImGui::EndChild();
        ImGui::End();
    }
}

inline void settings_menu() {
    ImGui::Checkbox("Enable skip table", &settings.skipEnable);
    ImGui::Checkbox("Show cursor?", &settings.cursorEnable);
    if (settings.cursorEnable) {
        ImGui::SliderInt("Timer cursor", &settings.timerCursor, 0, 100);
    }
    ImGui::Checkbox("Debug Packets", &settings.debugPackets);
    ImGui::InputInt("How much do you want to send?", &settings.countStringSend);
    ImGui::SliderInt("Sleep", &settings.sleepSendString, 0, 100);
    ImGui::Checkbox("Time on screen", &settings.timeOnScreenEnable);
    if (settings.timeOnScreenEnable) {
        ImGui::SliderInt("Pos X", &settings.posTimeOnScreen[0], 0, *(int*)0xC9C040);
        ImGui::SliderInt("Pos Y", &settings.posTimeOnScreen[1], 0, *(int*)0xC9C044);
    }
}

inline void cheats_menu() {
    const auto ptr = samp::RefNetGame()->GetPlayerPool()->GetLocalPlayer()->GetPed()->GetVehicle();
    const auto id = samp::RefNetGame()->GetVehiclePool()->Find(ptr);
    if (ImGui::Button("Fix Car") && ptr != nullptr) {
        samp::RefNetGame()->GetVehiclePool()->Get(id)->SetHealth(1000);
    }
    if (ImGui::Checkbox("FakeLag", &settings.enable_fakelag)) {
        if (settings.enable_fakelag)
            ImGui::SliderInt("Smooth fake lag", &settings.smooth_fakelag, 0, 2000);
    }
}

inline void bots_menu() {
    ImGui::InputText("RouteName", settings.routeName, 255);
    if (ImGui::Button("record")) {
        recorder::record_route(settings.routeName);
    }
    if (ImGui::Button("runroute")) {
        recorder::render_route(settings.routeName);
    }
    ImGui::Checkbox("Loop", &settings.cycle);
    ImGui::Checkbox("teleport on first point", &settings.tp_point);
}

inline void lovlya_menu() {
    ImGui::Checkbox("Flood alt + enter", &settings.flood_alt_enter);
}

inline void DisplayYorenovInfo() {
    ImGui::Text("Yorenov aka kenatori is the creator of this capture cheat. (Yorenov is akegga)");
    ImGui::Text("Yorenov - https://www.youtube.com/@yorenov");
    ImGui::Text("Yorenov - https://vk.com/yorenov");
    ImGui::Text("Yorenov - https://t.me/yorenov");
    ImGui::Text("Yorenov - https://github.com/yorenov");
    ImGui::Text("Yorenov - https://vk.com/kenatori");
    ImGui::Text("Yorenov - https://www.youtube.com/@kenatori_mafia");
}

inline void DisplayCaptureMenu() {
    if (ImGui::Button("Capture timer & table capture"))
        activeSubTab = SubTab::TimerAndTableCapture;
    if (ImGui::Button("Clan checker & clist checker"))
        activeSubTab = SubTab::ClanCheckerAndClistChecker;
    if (ImGui::Button("Settings & capture settings"))
        activeSubTab = SubTab::Settings;

    switch (activeSubTab) {
        case SubTab::NoneSub:
            ImGui::Text("If you haven't subscribed to yorenov yet, RUN and subscribe!!!");
        break;
        case SubTab::TimerAndTableCapture:
            captilka_menu();
        break;
        case SubTab::ClanCheckerAndClistChecker:
            clanchecker_menu();
        break;
        case SubTab::Settings:
            settings_menu();
        break;
    }
}

inline void DisplayTimeOnScreen() {
    ImGui::SetNextWindowPos({static_cast<float>(settings.posTimeOnScreen[0]), static_cast<float>(settings.posTimeOnScreen[1])});
    ImGui::SetNextWindowSize({100, 30});

    SYSTEMTIME st;
    GetLocalTime(&st);

    ImGui::Begin("##time", nullptr, ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs);
    ImGui::Text("%02d %02d %02d %03d", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
    ImGui::End();
}