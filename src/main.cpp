#include <stdint.h>
#include <coreinit/cache.h>
#include <coreinit/dynload.h>
#include <sysapp/launch.h>
#include <nsysnet/socket.h>
#include <proc_ui/procui.h>
#include <coreinit/foreground.h>

#include "ElfUtils.h"
#include "module/ModuleData.h"
#include "module/ModuleDataFactory.h"
#include "common/module_defines.h"

#include <whb/log.h>
#include <whb/log_udp.h>

#include "main.h"
#include "kernel.h"
#include "dynamic.h"
#include "utils/logger.h"
#include "utils/utils.h"

bool doRelocation(std::vector<RelocationData *> &relocData, relocation_trampolin_entry_t *tramp_data, uint32_t tramp_length);

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

extern "C" int _start(int argc, char **argv) {
    doKernelSetup();
    InitFunctionPointers();
    doKernelSetup2();

    __init_wut();

    WHBLogUdpInit();

    int res = 0;

    uint32_t ApplicationMemoryEnd;

    asm volatile("lis %0, __CODE_END@h; ori %0, %0, __CODE_END@l" : "=r" (ApplicationMemoryEnd));

    ApplicationMemoryEnd = (ApplicationMemoryEnd + 0x100) & 0xFFFFFF00;

    module_information_t *gModuleData = (module_information_t *) ApplicationMemoryEnd;

    uint32_t moduleDataStartAddress = ((uint32_t) gModuleData + sizeof(module_information_t));
    moduleDataStartAddress = (moduleDataStartAddress + 0x10000) & 0xFFFF0000;

    ModuleData *moduleData = ModuleDataFactory::load("fs:/vol/external01/wiiu/payload.rpx", 0x00FFF000, 0x00FFF000 - ApplicationMemoryEnd, gModuleData->trampolines, DYN_LINK_TRAMPOLIN_LIST_LENGTH);
    if (moduleData != NULL) {
        DEBUG_FUNCTION_LINE("Loaded module data");
        std::vector<RelocationData *> relocData = moduleData->getRelocationDataList();
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
        DEBUG_FUNCTION_LINE("New entrypoint: %08X", moduleData->getEntrypoint());
        ((int (*)(int, char **)) moduleData->getEntrypoint())(argc, argv);
        delete moduleData;
    } else {
        DEBUG_FUNCTION_LINE("Failed to load module");
    }

    SYSLaunchMenu();

    ProcUIInit(OSSavesDone_ReadyToRelease);
    DEBUG_FUNCTION_LINE("In ProcUI loop");
    while (CheckRunning()) {
        // wait.
        OSSleepTicks(OSMillisecondsToTicks(100));
    }
    ProcUIShutdown();

    __fini_wut();
    return 0;
}

bool doRelocation(std::vector<RelocationData *> &relocData, relocation_trampolin_entry_t *tramp_data, uint32_t tramp_length) {
    for (auto const &curReloc : relocData) {
        RelocationData *cur = curReloc;
        std::string functionName = cur->getName();
        std::string rplName = cur->getImportRPLInformation()->getName();
        int32_t isData = cur->getImportRPLInformation()->isData();
        OSDynLoad_Module rplHandle = 0;
        OSDynLoad_Acquire(rplName.c_str(), &rplHandle);

        uint32_t functionAddress = 0;
        OSDynLoad_FindExport(rplHandle, isData, functionName.c_str(), (void **) &functionAddress);
        if (functionAddress == 0) {
            return false;
        }
        if (!ElfUtils::elfLinkOne(cur->getType(), cur->getOffset(), cur->getAddend(), (uint32_t) cur->getDestination(), functionAddress, tramp_data, tramp_length, RELOC_TYPE_IMPORT)) {
            DEBUG_FUNCTION_LINE("Relocation failed");
            return false;
        }
    }

    DCFlushRange(tramp_data, tramp_length * sizeof(relocation_trampolin_entry_t));
    ICInvalidateRange(tramp_data, tramp_length * sizeof(relocation_trampolin_entry_t));
    return true;
}
