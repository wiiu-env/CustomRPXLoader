#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <coreinit/cache.h>

#include "kernel.h"

/* assembly functions */
extern "C" void Syscall_0x36(void);
extern "C" void KernelPatches(void);
extern "C" void KernelPatchesFinal(void);
extern "C" void SaveAndResetDataBATs_And_SRs_hook(void);

void __attribute__ ((noinline)) kern_write(void *addr, uint32_t value);

void doKernelSetup() {
    kern_write((void *) (KERN_SYSCALL_TBL_1 + (0x36 * 4)), (unsigned int) KernelPatches);
    kern_write((void *) (KERN_SYSCALL_TBL_2 + (0x36 * 4)), (unsigned int) KernelPatches);
    kern_write((void *) (KERN_SYSCALL_TBL_3 + (0x36 * 4)), (unsigned int) KernelPatches);
    kern_write((void *) (KERN_SYSCALL_TBL_4 + (0x36 * 4)), (unsigned int) KernelPatches);
    kern_write((void *) (KERN_SYSCALL_TBL_5 + (0x36 * 4)), (unsigned int) KernelPatches);

    Syscall_0x36();
}

void doKernelSetup2() {
    memcpy((void *) 0x00FFF000, (void *) SaveAndResetDataBATs_And_SRs_hook, 0x1000);
    ICInvalidateRange((void *) 0x00FFF000, 0x1000);
    DCFlushRange((void *) 0x00FFF000, 0x1000);

    kern_write((void *) (KERN_SYSCALL_TBL_1 + (0x36 * 4)), (unsigned int) KernelPatchesFinal);
    kern_write((void *) (KERN_SYSCALL_TBL_2 + (0x36 * 4)), (unsigned int) KernelPatchesFinal);
    kern_write((void *) (KERN_SYSCALL_TBL_3 + (0x36 * 4)), (unsigned int) KernelPatchesFinal);
    kern_write((void *) (KERN_SYSCALL_TBL_4 + (0x36 * 4)), (unsigned int) KernelPatchesFinal);
    kern_write((void *) (KERN_SYSCALL_TBL_5 + (0x36 * 4)), (unsigned int) KernelPatchesFinal);

    Syscall_0x36();
}

/* Write a 32-bit word with kernel permissions */
void __attribute__ ((noinline)) kern_write(void *addr, uint32_t value) {
    asm volatile (
    "li 3,1\n"
    "li 4,0\n"
    "mr 5,%1\n"
    "li 6,0\n"
    "li 7,0\n"
    "lis 8,1\n"
    "mr 9,%0\n"
    "mr %1,1\n"
    "li 0,0x3500\n"
    "sc\n"
    "nop\n"
    "mr 1,%1\n"
    :
    :    "r"(addr), "r"(value)
    :    "memory", "ctr", "lr", "0", "3", "4", "5", "6", "7", "8", "9", "10",
    "11", "12"
    );
}
