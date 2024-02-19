#include "main.h"
#include <rakhook/rakhook.hpp>
#include "rakhookT.h"
#include "ImGui-set/menu.h"
#include "utils/Utils.h"
#include "ImGui-set/ImGui_set.h"
#include <RakNet/StringCompressor.h>
#include <imgui.h>
#include <imgui_impl_dx9.h>
#include <imgui_impl_win32.h>
#include "utils/sampapi-i.h"
#include "recorder/recorder.h"
#include <d3d9.h>

#include "fish_bot/fishbot.h"

stSettings settings;

void cmd(const char* param) {
    settings.ImGuiEnable ^= true;
    if (settings.ImGuiEnable)
        samp::RefChat()->AddMessage(-1, "[Yorenov-Capture]: Включено");
    else
        samp::RefChat()->AddMessage(-1, "[Yorenov-Capture]: Выключено");
}

ktwait loopFunc() {
    while (true) {
        co_await (1);
        if (settings.captureEnable || settings.temp_captureTable) {
            SYSTEMTIME st;
            GetLocalTime(&st);

            char *endPtr;
            int captureHour = strtol(settings.captureTimer, &endPtr, 10);
            int captureMinute = strtol(endPtr, &endPtr, 10);
            int captureSecond = strtol(endPtr, &endPtr, 10);
            int captureMilliseconds = strtol(endPtr, nullptr, 10);

            if ((st.wHour >= captureHour && st.wMinute >= captureMinute && st.wSecond >= captureSecond &&
                 st.wMilliseconds >= captureMilliseconds - 40 && settings.captureEnable) || settings.temp_captureTable) {
                for (int i = 0; i < settings.countStringSend; i++) {
                    RakNet::BitStream bs;
                    settings.cmd[0] == '/' ? bs.Write<UINT32>(strlen(settings.cmd)) : bs.Write<UINT8>(
                            strlen(settings.cmd));
                    bs.Write(settings.cmd, (BYTE) strlen(settings.cmd));
                    rakhook::send_rpc(settings.cmd[0] == '/' ? 50 : 101, &bs, HIGH_PRIORITY, RELIABLE_SEQUENCED, 0,
                                      false);
                    if (settings.cursorEnable) {
                        if (settings.debugPackets)
                            samp::RefChat()->AddMessage(-1, "Cursor on");
                        Utils::GameUtils::showCursorForCapture(true);
                        co_await (settings.timerCursor);
                        Utils::GameUtils::showCursorForCapture(false);
                        if (settings.debugPackets)
                            samp::RefChat()->AddMessage(-1, "Cursor off");
                        settings.temp_cursorEnable = false;
                    }
                    co_await (settings.sleepSendString + (settings.cursorEnable ? settings.timerCursor : 0));
                }
                settings.temp_captureTable = false;
                settings.captureTable = false;
                settings.captureEnable = false;
            }
        }
        if (settings.cursorEnable && settings.temp_cursorEnable) {
            if (settings.debugPackets)
                samp::RefChat()->AddMessage(-1, "Cursor on");
            Utils::GameUtils::showCursorForCapture(true);
            co_await (settings.timerCursor);
            Utils::GameUtils::showCursorForCapture(false);
            if (settings.debugPackets)
                samp::RefChat()->AddMessage(-1, "Cursor off");
            settings.temp_cursorEnable = false;
        }
    }
}

void Plugin::mainloop(const decltype(hookCTimerUpdate)& hook) { // Main Game Thread
    static bool inited = false;
    if (!inited && samp::RefChat() != nullptr && rakhook::initialize()) {
        using namespace std::placeholders;
        samp::RefChat()->AddMessage(0xFFFFFFFF, "Plugin loaded");
        StringCompressor::AddReference();
        inited = true;
        rakhookT::installRakHook();
        tasks.push_back(taskManager.add_task(loopFunc));
        samp::RefInputBox()->AddCommand("menu", reinterpret_cast<sampapi::CMDPROC>(cmd));
    }
    else {
        taskManager.process();
        if (settings.flood_alt_enter)
            rakhookT::sendAlt();
        if (settings.render)
            recorder::recorderLoop();
        if (settings.enable_fish)
            fishbot::loopFish_Bot();
    }
    return hook.get_trampoline()();
}

Plugin::Plugin(HMODULE hndl) : hModule(hndl) { // Main Constructor
    using namespace std::placeholders;
    hookCTimerUpdate.set_cb(std::bind(&Plugin::mainloop, this, _1));
    hookCTimerUpdate.install();
}


std::unique_ptr<Plugin> plugin;

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved) { // Entry Point
    UnInitilize unIt;
    switch (dwReason) {
        case DLL_PROCESS_ATTACH: {
            DisableThreadLibraryCalls(hModule);
            plugin = std::make_unique<Plugin>(hModule);
            present_hook.before += on_present;
            reset_hook.before += on_lost;
            reset_hook.after += on_reset;
            break;
        }
        case DLL_PROCESS_DETACH: {
            unIt.unInit();
            rakhook::destroy();
            plugin.reset();
            present_hook.remove();
            reset_hook.remove();
            ImGui_ImplDX9_Shutdown();
            ImGui_ImplWin32_Shutdown();
            ImGui::DestroyContext();
            break;
        }
        default: {
            break;
        }
    }
    return TRUE;
}
