#include "Utils.h"
#include "../utils/sampapi-i.h"
#include <d3d9.h>

int Utils::GameUtils::getNearestVehicle(float radius)
{
    int iVehicleID = -1;
    float fNearestDistance = radius ? radius : -1;

    for (int i = 0; i < 2000; i++)
    {
        if (!samp::RefNetGame()->GetVehiclePool()->Get(i)) continue;

        float fDistance = samp::RefNetGame()->GetVehiclePool()->Get(i)->GetDistanceToLocalPlayer();

        if (fNearestDistance == -1.0f || fDistance < fNearestDistance)
        {
            fNearestDistance = fDistance;
            iVehicleID = i;
        }
    }
    return iVehicleID;
}

void Utils::GameUtils::showCursorForImGui(bool show, bool lockControls)
{
    if (show)
    {
        if (lockControls)
        {
            samp::RefGame()->SetCursorMode(samp::CURSOR_LOCKCAMANDCONTROL, false);
        } else {
            samp::RefGame()->SetCursorMode(samp::CURSOR_LOCKCAM, false);
        }
    }
    else
    {
        samp::RefGame()->SetCursorMode(samp::CURSOR_NONE, true);
    }
}

void Utils::GameUtils::showCursorForCapture(bool state)
{
    using RwD3D9GetCurrentD3DDevice_t = LPDIRECT3DDEVICE9(__cdecl *)();

    auto rwCurrentD3dDevice{reinterpret_cast<
                                    RwD3D9GetCurrentD3DDevice_t>(0x7F9D50U)()};

    if (nullptr == rwCurrentD3dDevice) {
        return;
    }

    static DWORD
            updateMouseProtection,
            rsMouseSetPosProtFirst,
            rsMouseSetPosProtSecond;

    if (state)
    {
        ::VirtualProtect(reinterpret_cast<void *>(0x53F3C6U), 5U,
                         PAGE_EXECUTE_READWRITE, &updateMouseProtection);

        ::VirtualProtect(reinterpret_cast<void *>(0x53E9F1U), 5U,
                         PAGE_EXECUTE_READWRITE, &rsMouseSetPosProtFirst);

        ::VirtualProtect(reinterpret_cast<void *>(0x748A1BU), 5U,
                         PAGE_EXECUTE_READWRITE, &rsMouseSetPosProtSecond);

        // NOP: CPad::UpdateMouse
        *reinterpret_cast<uint8_t *>(0x53F3C6U) = 0xE9U;
        *reinterpret_cast<uint32_t *>(0x53F3C6U + 1U) = 0x15BU;

        // NOP: RsMouseSetPos
        memset(reinterpret_cast<void *>(0x53E9F1U), 0x90, 5U);
        memset(reinterpret_cast<void *>(0x748A1BU), 0x90, 5U);

        rwCurrentD3dDevice->ShowCursor(TRUE);
    }
    else
    {
        // Original: CPad::UpdateMouse
        memcpy(reinterpret_cast<void *>(0x53F3C6U), "\xE8\x95\x6C\x20\x00", 5U);

        // Original: RsMouseSetPos
        memcpy(reinterpret_cast<void *>(0x53E9F1U), "\xE8\xAA\xAA\x0D\x00", 5U);
        memcpy(reinterpret_cast<void *>(0x748A1BU), "\xE8\x80\x0A\xED\xFF", 5U);

        using CPad_ClearMouseHistory_t = void(__cdecl *)();
        using CPad_UpdatePads_t = void(__cdecl *)();

        reinterpret_cast<CPad_ClearMouseHistory_t>(0x541BD0U)();
        reinterpret_cast<CPad_UpdatePads_t>(0x541DD0U)();

        ::VirtualProtect(reinterpret_cast<void *>(0x53F3C6U), 5U,
                         updateMouseProtection, &updateMouseProtection);

        ::VirtualProtect(reinterpret_cast<void *>(0x53E9F1U), 5U,
                         rsMouseSetPosProtFirst, &rsMouseSetPosProtFirst);

        ::VirtualProtect(reinterpret_cast<void *>(0x748A1BU), 5U,
                         rsMouseSetPosProtSecond, &rsMouseSetPosProtSecond);

        rwCurrentD3dDevice->ShowCursor(FALSE);
    }
}