/****************************************************************************
 * Copyright (C) 2018-2020 Maschell
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ****************************************************************************/

#include <coreinit/cache.h>
#include <coreinit/dynload.h>
#include <coreinit/foreground.h>
#include <coreinit/screen.h>
#include <coreinit/title.h>
#include <cstdint>
#include <nn/act/client_cpp.h>
#include <proc_ui/procui.h>
#include <sysapp/launch.h>

#include "ElfUtils.h"
#include "common/module_defines.h"
#include "module/ModuleData.h"
#include "module/ModuleDataFactory.h"

#include <utils/StringTools.h>

#include "dynamic.h"
#include "kernel.h"
#include "utils/logger.h"
#include <coreinit/debug.h>
#include <coreinit/memexpheap.h>
#include <malloc.h>

bool doRelocation(const std::vector<RelocationData> &relocData, relocation_trampolin_entry_t *tramp_data, uint32_t tramp_length);

void SplashScreen(const char *message, int32_t durationInMs);

uint32_t do_start(int argc, char **argv);

bool CheckRunning() {
    switch (ProcUIProcessMessages(true)) {
        case PROCUI_STATUS_EXITING: {
            return false;
        }
        case PROCUI_STATUS_RELEASE_FOREGROUND: {
            ProcUIDrawDoneRelease();
            break;
        }
        case PROCUI_STATUS_IN_FOREGROUND: {
            break;
        }
        case PROCUI_STATUS_IN_BACKGROUND:
        default:
            break;
    }
    return true;
}

extern "C" void __init_wut();
extern "C" void __fini_wut();

extern "C" int _start(int argc, char **argv) __attribute__((section(".start_code")));
extern "C" int _start(int argc, char **argv) {
    doKernelSetup();
    InitFunctionPointers();
    doKernelSetup2();

    __init_wut();

    // Save last entry on mem2 heap to detect leaked memory
    MEMHeapHandle mem2_heap_handle = MEMGetBaseHeapHandle(MEM_BASE_HEAP_MEM2);
    auto heap                      = (MEMExpHeap *) mem2_heap_handle;
    MEMExpHeapBlock *memory_start  = heap->usedList.tail;

    initLogging();
    DEBUG_FUNCTION_LINE("Hello from CustomRPXloader");

    uint32_t entrypoint = do_start(argc, argv);

    deinitLogging();

    // free leaked memory
    if (memory_start) {
        int leak_count = 0;
        while (true) {
            MEMExpHeapBlock *memory_end = heap->usedList.tail;
            if (memory_end == memory_start) {
                break;
            }
            auto mem_ptr = &memory_end[1]; // &memory_end + sizeof(MEMExpHeapBlock);
            free(mem_ptr);
            leak_count++;
        }
        OSReport("Freed %d leaked memory blocks\n", leak_count);
    }

    __fini_wut();

    if (entrypoint > 0) {
        // clang-format off
        return ((int(*)(int, char **)) entrypoint)(argc, argv);
        // clang-format on
    }

    return -1;
}

uint32_t do_start(int argc, char **argv) {
    // If we load from our CustomRPXLoader the argv is set with "safe.rpx"
    // in this case we don't want to do any ProcUi stuff on error, only on success
    bool doProcUI = (argc >= 1 && std::string(argv[0]) != "safe.rpx");

    auto *cfwLaunchedWithPtr = (uint64_t *) 0x00FFFFF8;
    *cfwLaunchedWithPtr      = OSGetTitleID();

    uint32_t ApplicationMemoryEnd;

    asm volatile("lis %0, __CODE_END@h; ori %0, %0, __CODE_END@l"
                 : "=r"(ApplicationMemoryEnd));

    ApplicationMemoryEnd = (ApplicationMemoryEnd + 0x100) & 0xFFFFFF00;

    auto *gModuleData = (module_information_t *) ApplicationMemoryEnd;

    uint32_t moduleDataStartAddress = ((uint32_t) gModuleData + sizeof(module_information_t));
    moduleDataStartAddress          = (moduleDataStartAddress + 0x10000) & 0xFFFF0000;

    std::string filepath("fs:/vol/external01/wiiu/payload.rpx");
    int result = 0;
    // The module will be loaded to 0x00FFF000 - sizeof(payload.rpx)
    std::optional<ModuleData> moduleData = ModuleDataFactory::load(filepath, 0x00FFF000, 0x00FFF000 - moduleDataStartAddress, gModuleData->trampolines, DYN_LINK_TRAMPOLIN_LIST_LENGTH);
    if (moduleData) {
        DEBUG_FUNCTION_LINE("Loaded module data");
        std::vector<RelocationData> relocData = moduleData->getRelocationDataList();
        if (!doRelocation(relocData, gModuleData->trampolines, DYN_LINK_TRAMPOLIN_LIST_LENGTH)) {
            DEBUG_FUNCTION_LINE("relocations failed");
        }
        if (moduleData->getBSSAddr() != 0) {
            DEBUG_FUNCTION_LINE("memset .bss %08X (%d)", moduleData->getBSSAddr(), moduleData->getBSSSize());
            memset((void *) moduleData->getBSSAddr(), 0, moduleData->getBSSSize());
        }
        if (moduleData->getSBSSAddr() != 0) {
            DEBUG_FUNCTION_LINE("memset .sbss %08X (%d)", moduleData->getSBSSAddr(), moduleData->getSBSSSize());
            memset((void *) moduleData->getSBSSAddr(), 0, moduleData->getSBSSSize());
        }
        DCFlushRange((void *) 0x00800000, 0x00800000);
        ICInvalidateRange((void *) 0x00800000, 0x00800000);
        DEBUG_FUNCTION_LINE("Calling entrypoint at: %08X", moduleData->getEntrypoint());
        return moduleData->getEntrypoint();
    } else {
        DEBUG_FUNCTION_LINE("Failed to load module, revert main_hook");
        revertMainHook();
        SplashScreen(StringTools::strfmt("Failed to load \"%s\"", filepath.c_str()).c_str(), 3000);
        result = 0;
    }

    if (doProcUI) {
        nn::act::Initialize();
        nn::act::SlotNo slot        = nn::act::GetSlotNo();
        nn::act::SlotNo defaultSlot = nn::act::GetDefaultAccount();
        nn::act::Finalize();

        if (defaultSlot) {
            //normal menu boot
            SYSLaunchMenu();
        } else {
            //show mii select
            _SYSLaunchMenuWithCheckingAccount(slot);
        }
        ProcUIInit(OSSavesDone_ReadyToRelease);
        DEBUG_FUNCTION_LINE("In ProcUI loop");
        while (CheckRunning()) {
            // wait.
            OSSleepTicks(OSMillisecondsToTicks(100));
        }
        ProcUIShutdown();
    }

    return result;
}

bool doRelocation(const std::vector<RelocationData> &relocData, relocation_trampolin_entry_t *tramp_data, uint32_t tramp_length) {
    for (auto const &curReloc : relocData) {
        const RelocationData &cur  = curReloc;
        std::string functionName   = cur.getName();
        std::string rplName        = cur.getImportRPLInformation().getName();
        int32_t isData             = cur.getImportRPLInformation().isData();
        OSDynLoad_Module rplHandle = nullptr;
        OSDynLoad_Acquire(rplName.c_str(), &rplHandle);

        uint32_t functionAddress = 0;
        OSDynLoad_FindExport(rplHandle, isData, functionName.c_str(), (void **) &functionAddress);
        if (functionAddress == 0) {
            return false;
        }
        if (!ElfUtils::elfLinkOne(cur.getType(), cur.getOffset(), cur.getAddend(), (uint32_t) cur.getDestination(), functionAddress, tramp_data, tramp_length, RELOC_TYPE_IMPORT)) {
            DEBUG_FUNCTION_LINE("Relocation failed");
            return false;
        }
    }

    DCFlushRange(tramp_data, tramp_length * sizeof(relocation_trampolin_entry_t));
    ICInvalidateRange(tramp_data, tramp_length * sizeof(relocation_trampolin_entry_t));
    return true;
}

void SplashScreen(const char *message, int32_t durationInMs) {
    // Init screen and screen buffers
    OSScreenInit();
    uint32_t screen_buf0_size = OSScreenGetBufferSizeEx(SCREEN_TV);
    uint32_t screen_buf1_size = OSScreenGetBufferSizeEx(SCREEN_DRC);
    auto *screenBuffer        = (uint8_t *) memalign(0x100, screen_buf0_size + screen_buf1_size);
    OSScreenSetBufferEx(SCREEN_TV, (void *) screenBuffer);
    OSScreenSetBufferEx(SCREEN_DRC, (void *) (screenBuffer + screen_buf0_size));

    OSScreenEnableEx(SCREEN_TV, 1);
    OSScreenEnableEx(SCREEN_DRC, 1);

    // Clear screens
    OSScreenClearBufferEx(SCREEN_TV, 0);
    OSScreenClearBufferEx(SCREEN_DRC, 0);

    OSScreenPutFontEx(SCREEN_TV, 0, 0, message);
    OSScreenPutFontEx(SCREEN_DRC, 0, 0, message);

    OSScreenFlipBuffersEx(SCREEN_TV);
    OSScreenFlipBuffersEx(SCREEN_DRC);

    OSSleepTicks(OSMillisecondsToTicks(durationInMs));
    free(screenBuffer);
}