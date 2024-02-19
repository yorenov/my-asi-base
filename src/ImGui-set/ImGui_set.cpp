#include "ImGui_set.h"
#include <imgui.h>
#include <imgui_impl_dx9.h>
#include <imgui_impl_win32.h>
#include "../utils/sampapi-i.h"
#include "menu.h"

// External ImGui handler for Win32 window procedure
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

constexpr std::uintptr_t INVALID_BASE = 0;

// Function to find the base address of the device
std::uintptr_t find_device(std::uint32_t length) {
    static std::uintptr_t base = [](std::size_t length) {
        std::string path(MAX_PATH, '\0');
        if (auto size = GetSystemDirectoryA(path.data(), MAX_PATH)) {
            path.resize(size);
            path += "\\d3d9.dll";
            auto baseAddress = reinterpret_cast<std::uintptr_t>(LoadLibraryA(path.c_str()));
            while (baseAddress++ < baseAddress + length) {
                if (*reinterpret_cast<std::uint16_t*>(baseAddress + 0x00) == 0x06C7 &&
                    *reinterpret_cast<std::uint16_t*>(baseAddress + 0x06) == 0x8689 &&
                    *reinterpret_cast<std::uint16_t*>(baseAddress + 0x0C) == 0x8689) {
                    baseAddress += 2;
                    break;
                }
            }
            return baseAddress;
        }
        return INVALID_BASE;
    }(length);
    return base;
}

// Lambda function to get the game window handle
HWND game_hwnd = []() {
    // Pointer to HWND inside the game engine
    HWND* hwnd_ptr = *reinterpret_cast<HWND**>(0xC17054);
    if (hwnd_ptr != nullptr) {
        return *hwnd_ptr;
    }
    else {
        // Set callback after the execution of the original function, as we need its return
        game_instance_init_hook.after += [](const auto& hook, HWND& return_value, HINSTANCE inst)  {
            // Assign our variable the value that the function returned to us
            game_hwnd = return_value;
        };
        return HWND(0);
    }
}();

// Function to handle window procedure events
HRESULT __stdcall on_wndproc(const decltype(wndproc_hook)& hook, HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == WM_KEYDOWN && (wParam == VK_F9 && (HIWORD(lParam) & KF_REPEAT) != KF_REPEAT)) {
        settings.ImGuiEnable ^= true;
        if (settings.ImGuiEnable)
            samp::RefChat()->AddMessage(-1, "[Yorenov-Capture]: Включено");
        else
            samp::RefChat()->AddMessage(-1, "[Yorenov-Capture]: Выключено");
    }
    if (uMsg == WM_CHAR) {
        wchar_t wch;
        MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, reinterpret_cast<char*>(&wParam), 1, &wch, 1);
        wParam = wch;
    }
    ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam);
    auto& io = ImGui::GetIO();
    if (io.WantCaptureKeyboard || io.WantCaptureMouse) {
        return 1;
    }
    // Call the original function
    return hook.get_trampoline()(hwnd, uMsg, wParam, lParam);
}

std::optional<HRESULT> on_present(const decltype(present_hook)& hook, IDirect3DDevice9* device_ptr, const RECT*, const RECT*, HWND, const RGNDATA*) {
    static bool ImGui_inited = false;
    if (!ImGui_inited) {
        // Создаем имгуи контекст
        ImGui::CreateContext();
        // Инициализируем OS зависимую часть(обрабатывает открытие шрифтов, обработку нажатия клавиш и т.д.)
        ImGui_ImplWin32_Init(game_hwnd);
        // Инициализируем render framework зависимую часть(обрабатывает отрисовку на экране, создание текстур шрифтов и т.д.)
        ImGui_ImplDX9_Init(device_ptr);

        auto latest_wndproc_ptr = GetWindowLongPtrA(game_hwnd, GWLP_WNDPROC);
        wndproc_hook.set_dest(latest_wndproc_ptr);
        wndproc_hook.set_cb(&on_wndproc);
        wndproc_hook.install();
        ImGui_inited = true;
    }
    // Инициализируем render часть для нового кадра
    ImGui_ImplDX9_NewFrame();
    // Инициализируем OS часть для нового кадра
    ImGui_ImplWin32_NewFrame();
    // Создаем новый кадр внутри ImGui
    ImGui::NewFrame();

    menu::ShowCursorImGUI_LOOP();

    if (settings.ImGuiEnable)
        menu::DrawMenu();
    if (settings.timeOnScreenEnable)
        DisplayTimeOnScreen();

    // Завершаем кадр ImGui
    ImGui::EndFrame();
    // Рендерим ImGuiв внутренний буффер
    ImGui::Render();
    // Отдаем Directx внутренний буффер на рендер
    ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
    return std::nullopt; // не нужно прерывать выполнение
}

std::optional<HRESULT> on_lost(const decltype(reset_hook)& hook, IDirect3DDevice9* device_ptr, D3DPRESENT_PARAMETERS* parameters) {
    ImGui_ImplDX9_InvalidateDeviceObjects();
    return std::nullopt; // не нужно прерывать выполнение
}

void on_reset(const decltype(reset_hook)& hook, HRESULT& return_value, IDirect3DDevice9* device_ptr, D3DPRESENT_PARAMETERS* parameters) {

}