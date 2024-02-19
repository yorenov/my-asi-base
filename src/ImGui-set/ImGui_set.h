#pragma once
#include <d3d9.h>
#include "kthook/kthook.hpp"

std::uintptr_t find_device(std::uint32_t length);

using PresentSignature = HRESULT(__stdcall*)(IDirect3DDevice9*, const RECT*, const RECT*, HWND, const RGNDATA*);
using ResetSignature = HRESULT(__stdcall*)(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);
using InitGameInstance = HWND(__cdecl*)(HINSTANCE);

inline kthook::kthook_signal<PresentSignature> present_hook{ (*reinterpret_cast<void***>(find_device(0x128000)))[17] };
inline kthook::kthook_signal<ResetSignature> reset_hook{ (*reinterpret_cast<void***>(find_device(0x128000)))[16] };
inline kthook::kthook_simple<WNDPROC> wndproc_hook{};
inline kthook::kthook_signal<InitGameInstance> game_instance_init_hook{ 0x745560 };

HRESULT __stdcall on_wndproc(const decltype(wndproc_hook)& hook, HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
std::optional<HRESULT> on_present(const decltype(present_hook)& hook, IDirect3DDevice9* device_ptr, const RECT*, const RECT*, HWND, const RGNDATA*);
std::optional<HRESULT> on_lost(const decltype(reset_hook)& hook, IDirect3DDevice9* device_ptr, D3DPRESENT_PARAMETERS* parameters);
void on_reset(const decltype(reset_hook)& hook, HRESULT& return_value, IDirect3DDevice9* device_ptr, D3DPRESENT_PARAMETERS* parameters);