#include "menu.h"
#include "../utils/Utils.h"

void menu::DrawMenu() {
    ImGui::Begin("Capture-cheat by yorenov", &settings.ImGuiEnable,
                 ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoResize);
    if (ImGui::Button("Capture"))
        activeTab = Tab::Capture;
    ImGui::SameLine();
    if (ImGui::Button("Lovlya"))
        activeTab = Tab::Lovlya;
    ImGui::SameLine();
    if (ImGui::Button("Bots"))
        activeTab = Tab::Bots;
    ImGui::SameLine();
    if (ImGui::Button("Cheats"))
        activeTab = Tab::Cheats;
    ImGui::SameLine();
    if (ImGui::Button("Theme")) {
        activeTab = Tab::Theme;
    }

    switch (activeTab) {
        case Tab::None: {
            DisplayYorenovInfo();
            break;
        }
        case Tab::Capture: {
            ImGui::Text("Capture");
            DisplayCaptureMenu();
            break;
        }
        case Tab::Lovlya: {
            ImGui::Text("Lovlya");
            lovlya_menu();
            break;
        }
        case Tab::Bots: {
            ImGui::Text("Bots");
            bots_menu();
            break;
        }
        case Tab::Cheats: {
            ImGui::Text("Cheats");
            cheats_menu();
            break;
        }
        case Tab::Theme: {
            ImGui::Text("Theme");
            if (ImGui::Button("Theme 1")) { ImGui::StyleColorsDark(); }
            if (ImGui::Button("Theme 2")) { ImGui::StyleColorsClassic(); }
            if (ImGui::Button("Theme 3")) { ImGui::StyleColorsLight(); }
            if (ImGui::Button("Theme 4")) { menu::styleImGui(); }
        }
    }
    ImGui::End();
}

void menu::ShowCursorImGUI_LOOP() {
    if (settings.tempImGuiEnable != settings.ImGuiEnable) {
        Utils::GameUtils::showCursorForImGui(settings.ImGuiEnable, settings.ImGuiEnable);
        settings.tempImGuiEnable = settings.ImGuiEnable;
    }
}

void menu::styleImGui() {
    ImGuiStyle &style = ImGui::GetStyle();

    // Color palette
    constexpr auto Color_Black = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    constexpr auto Color_White = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    constexpr auto Color_Grey = ImVec4(0.60f, 0.60f, 0.60f, 0.35f);
    constexpr auto Color_Dark = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);
    constexpr auto Color_Darkgrey = ImVec4(0.23f, 0.23f, 0.23f, 0.35f);
    constexpr auto Color_Lighgrey = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);

    // Color array
    style.Colors[ImGuiCol_Text] = Color_White;
    style.Colors[ImGuiCol_TextDisabled] = Color_Grey;
    style.Colors[ImGuiCol_WindowBg] = Color_Dark;
    style.Colors[ImGuiCol_ChildBg] = Color_Dark;
    style.Colors[ImGuiCol_PopupBg] = Color_Dark;
    style.Colors[ImGuiCol_Border] = Color_Grey;
    style.Colors[ImGuiCol_BorderShadow] = Color_Black;
    style.Colors[ImGuiCol_FrameBg] = Color_Darkgrey;
    style.Colors[ImGuiCol_FrameBgHovered] = Color_Grey;
    style.Colors[ImGuiCol_FrameBgActive] = Color_Grey;
    style.Colors[ImGuiCol_TitleBg] = Color_Darkgrey;
    style.Colors[ImGuiCol_TitleBgActive] = Color_Darkgrey;
    style.Colors[ImGuiCol_TitleBgCollapsed] = Color_Darkgrey;
    style.Colors[ImGuiCol_MenuBarBg] = Color_Darkgrey;
    style.Colors[ImGuiCol_ScrollbarBg] = Color_Darkgrey;
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = Color_Grey;
    style.Colors[ImGuiCol_ScrollbarGrabActive] = Color_Grey;
    style.Colors[ImGuiCol_CheckMark] = Color_Lighgrey;
    style.Colors[ImGuiCol_SliderGrab] = Color_Lighgrey;
    style.Colors[ImGuiCol_SliderGrabActive] = Color_White;
    style.Colors[ImGuiCol_Button] = Color_Darkgrey;
    style.Colors[ImGuiCol_ButtonHovered] = Color_Grey;
    style.Colors[ImGuiCol_ButtonActive] = Color_Darkgrey;
    style.Colors[ImGuiCol_Header] = Color_Darkgrey;
    style.Colors[ImGuiCol_HeaderHovered] = Color_Grey;
    style.Colors[ImGuiCol_HeaderActive] = Color_Grey;
    style.Colors[ImGuiCol_Separator] = Color_Grey;
    style.Colors[ImGuiCol_SeparatorHovered] = Color_Grey;
    style.Colors[ImGuiCol_SeparatorActive] = Color_Grey;
    style.Colors[ImGuiCol_ResizeGrip] = Color_Darkgrey;
    style.Colors[ImGuiCol_ResizeGripHovered] = Color_Grey;
    style.Colors[ImGuiCol_ResizeGripActive] = Color_Grey;
    style.Colors[ImGuiCol_Tab] = Color_Darkgrey;
    style.Colors[ImGuiCol_TabHovered] = Color_Grey;
    style.Colors[ImGuiCol_TabActive] = Color_Grey;
    style.Colors[ImGuiCol_TabUnfocused] = Color_Grey;
    style.Colors[ImGuiCol_TabUnfocused] = Color_Grey;
    style.Colors[ImGuiCol_TabUnfocusedActive] = Color_Grey;
    style.Colors[ImGuiCol_PlotLines] = Color_White;
    style.Colors[ImGuiCol_PlotLinesHovered] = Color_Grey;
    style.Colors[ImGuiCol_PlotHistogram] = Color_White;
    style.Colors[ImGuiCol_PlotHistogramHovered] = Color_Grey;
    style.Colors[ImGuiCol_TableHeaderBg] = Color_Dark;
    style.Colors[ImGuiCol_TableBorderStrong] = Color_Darkgrey;
    style.Colors[ImGuiCol_TableBorderLight] = Color_Grey;
    style.Colors[ImGuiCol_TableRowBg] = Color_Black;
    style.Colors[ImGuiCol_TableRowBgAlt] = Color_White;
    style.Colors[ImGuiCol_TextSelectedBg] = Color_Darkgrey;
    style.Colors[ImGuiCol_DragDropTarget] = Color_Darkgrey;
    style.Colors[ImGuiCol_NavHighlight] = Color_Grey;
    style.Colors[ImGuiCol_NavWindowingHighlight] = Color_Grey;
    style.Colors[ImGuiCol_NavWindowingDimBg] = Color_Grey;
    style.Colors[ImGuiCol_ModalWindowDimBg] = Color_Grey;
}
