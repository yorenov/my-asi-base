#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>

#include "utils/ktcoro_wait.hpp"
#include "kthook/kthook.hpp"

#include <memory>
#include <fstream>


// settings

struct stSettings {
    bool ImGuiEnable{};
    bool tempImGuiEnable{};

    bool timeOnScreenEnable{};
    int posTimeOnScreen[2]{};

    bool debugPackets{};

    bool skipEnable{};
    bool cursorEnable{};
    bool temp_cursorEnable{};
    int countStringSend{4};
    int timerCursor{0};
    char cmd[20]{"/capture_biz"};
    char captureTimer[13]{"00 00 00 000"};
    bool captureEnable{};
    int sleepSendString{0};
    bool clistFilterEnable{};
    int idFindClist{0};
    bool captureTable{};
    bool temp_captureTable{};

    // lovlya
    bool flood_alt_enter{};
    bool record = false;
    bool render = false;
    DWORD last_point_time = 0;
    bool cycle = false; // Цикличность воспроизведения
    bool tp_point = false; // ТП на точку при воспроизведении
    int last_point = 0;
    std::ofstream wRoute_file;
    std::ifstream rRoute_file;
    char routeName[255];

    //fish bot
    bool enable_fish{};

    //fakelag
    bool enable_fakelag{};
    int smooth_fakelag{100};

}; extern stSettings settings;












class UnInitilize {
public:
    void unInit() {
        hookCTimerUpdate.remove();
    }
private:
    using CTimer_UpdateSignature = void(__cdecl*)();
    kthook::kthook_simple<CTimer_UpdateSignature> hookCTimerUpdate{ reinterpret_cast<void*>(0x561B10) };
};

class Plugin {
private:
    using CTimer_UpdateSignature = void(__cdecl*)();
    kthook::kthook_simple<CTimer_UpdateSignature> hookCTimerUpdate{ reinterpret_cast<void*>(0x561B10) };
    std::vector<ktwait> tasks;
    ktcoro_tasklist taskManager;
public:
    Plugin(HMODULE hModule);
    void mainloop(const decltype(hookCTimerUpdate)& hook);
    HMODULE hModule;
    std::unique_ptr<Plugin> plugin;
};