#include <coreinit/dynload.h>
#include <coreinit/debug.h>

#define IMPORT(name) void* addr_##name
#define IMPORT_BEGIN(lib)
#define IMPORT_END()
#include "imports.h"

#undef IMPORT
#undef IMPORT_BEGIN
#undef IMPORT_END

#define IMPORT(name)       do{if(OSDynLoad_FindExport(handle, 0, #name, &addr_##name) < 0)OSFatal("Function " # name " is NULL");} while(0)
#define IMPORT_BEGIN(lib)  OSDynLoad_Acquire(#lib ".rpl", &handle)
/* #define IMPORT_END()       OSDynLoad_Release(handle) */
#define IMPORT_END()

#define EXPORT_VAR(type, var)           type var __attribute__((section(".data")));

EXPORT_VAR(uint32_t *, pMEMAllocFromDefaultHeapEx);
EXPORT_VAR(uint32_t *, pMEMAllocFromDefaultHeap);
EXPORT_VAR(uint32_t *, pMEMFreeToDefaultHeap);


void InitFunctionPointers(void) {
    OSDynLoad_Module handle;
    addr_OSDynLoad_Acquire = (void*) 0x0102A3B4;
    addr_OSDynLoad_FindExport = (void*) 0x0102B828;

    OSDynLoad_Acquire("coreinit.rpl", &handle);
    OSDynLoad_FindExport(handle, 1, "MEMAllocFromDefaultHeapEx", (void**) &pMEMAllocFromDefaultHeapEx);
    OSDynLoad_FindExport(handle, 1, "MEMAllocFromDefaultHeap", (void**) &pMEMAllocFromDefaultHeap);
    OSDynLoad_FindExport(handle, 1, "MEMFreeToDefaultHeap", (void**) &pMEMFreeToDefaultHeap);

#include "imports.h"

}
