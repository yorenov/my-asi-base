#pragma once
#include "../main.h"
class Utils {
public:
    class GameUtils {
    public:
        static int getNearestVehicle(float radius);
        static void showCursorForCapture(bool state);
        static void showCursorForImGui(bool show, bool lockControls);
    private:

    };








    class Prots {
    public:
        enum class MemoryProt {
            None = (0),
            NoAccess = (1 << 1),
            ReadOnly = (1 << 2),
            ReadWrite = (1 << 3),
            WriteCopy = (1 << 4),
            Execute = (1 << 5),
            ExecuteRead = (1 << 6),
            ExecuteReadWrite = (1 << 7),
            ExecuteWriteCopy = (1 << 8)
        };

        inline uint32_t convert_memory_protection_constant(MemoryProt prot) {
            switch (prot) {
                case MemoryProt::NoAccess: return PAGE_NOACCESS;
                case MemoryProt::ReadOnly: return PAGE_READONLY;
                case MemoryProt::ReadWrite: return PAGE_READWRITE;
                case MemoryProt::WriteCopy: return PAGE_WRITECOPY;
                case MemoryProt::Execute: return PAGE_EXECUTE;
                case MemoryProt::ExecuteRead: return PAGE_EXECUTE_READ;
                case MemoryProt::ExecuteReadWrite: return PAGE_EXECUTE_READWRITE;
                case MemoryProt::ExecuteWriteCopy: return PAGE_EXECUTE_WRITECOPY;
                default: return 0;
            }
        }

        bool change_memory_protection(void* address, size_t size, MemoryProt prot, DWORD& oldProtect) {
            DWORD newProtect = convert_memory_protection_constant(prot);
            return VirtualProtect(address, size, newProtect, &oldProtect) != 0;
        }

        void restore_memory_protection(void* address, size_t size, DWORD oldProtect) {
            VirtualProtect(address, size, oldProtect, &oldProtect);
        }
    };

    class LLMO : public Prots {
    public:
        template<typename FuncType>
        static auto callfunction(FuncType function, void* address, typename std::remove_pointer<FuncType>::type::argument_type arg) -> typename std::remove_pointer<FuncType>::type::return_type {
            auto func = reinterpret_cast<FuncType>(address);
            return func(arg);
        }

        void fillMemory(void* address, void* bytes, int byteSize) {
            DWORD oldProtection;
            DWORD newProtection = convert_memory_protection_constant(MemoryProt::ReadWrite);

            if (change_memory_protection(address, byteSize, MemoryProt::ReadWrite, oldProtection)) {
                memcpy(address, bytes, byteSize);
                restore_memory_protection(address, byteSize, oldProtection);
            }
        }
        void write_memory(void* address, int value, size_t size) {
            DWORD oldProtection;
            DWORD newProtection = convert_memory_protection_constant(MemoryProt::ReadWrite);

            if (change_memory_protection(address, size, MemoryProt::ReadWrite, oldProtection)) {
                memset(address, value, size);
                restore_memory_protection(address, size, oldProtection);
            }
        }
        void nop_(PVOID address, int bytes) {
            DWORD oldProtection;
            DWORD newProtection = convert_memory_protection_constant(MemoryProt::ReadWrite);

            if (change_memory_protection(address, bytes, MemoryProt::ReadWrite, oldProtection)) {
                memset(address, 0x90, bytes);
                restore_memory_protection(address, bytes, oldProtection);
            }
        }
        size_t readBytesMemory(void* address) {
            MEMORY_BASIC_INFORMATION mbi;
            VirtualQuery(address, &mbi, sizeof(mbi));
            return mbi.RegionSize;
        }
        template<typename T>
        T read_memory(void* address) {
            T value;
            DWORD oldProtection;
            change_memory_protection(address, sizeof(T), MemoryProt::ReadOnly, oldProtection);
            value = *reinterpret_cast<T*>(address);
            restore_memory_protection(address, sizeof(T), oldProtection);
            return value;
        }

    };
};