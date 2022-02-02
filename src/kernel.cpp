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
#include <coreinit/memorymap.h>
#include <cstdint>
#include <cstdio>
#include <cstring>

#include "kernel.h"

/* assembly functions */
extern "C" void Syscall_0x36(void);
extern "C" void KernelPatches(void);
extern "C" void KernelPatchesFinal(void);
extern "C" void SaveAndResetDataBATs_And_SRs_hook(void);

void __attribute__((noinline)) kern_write(void *addr, uint32_t value);

extern "C" void SC_0x25_KernelCopyData(unsigned int addr, unsigned int src, unsigned int len);

extern "C" void SCKernelCopyData(unsigned int addr, unsigned int src, unsigned int len);

void KernelWriteU32(uint32_t addr, uint32_t value) {
    ICInvalidateRange(&value, 4);
    DCFlushRange(&value, 4);

    auto dst = (uint32_t) OSEffectiveToPhysical(addr);
    auto src = (uint32_t) OSEffectiveToPhysical((uint32_t) &value);

    SC_0x25_KernelCopyData(dst, src, 4);

    DCFlushRange((void *) addr, 4);
    ICInvalidateRange((void *) addr, 4);
}

void revertMainHook() {
    KernelWriteU32(0x0101c56c, 0x4E800421);
}

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

    kern_write((void *) (KERN_SYSCALL_TBL_1 + (0x25 * 4)), (unsigned int) SCKernelCopyData);
    kern_write((void *) (KERN_SYSCALL_TBL_2 + (0x25 * 4)), (unsigned int) SCKernelCopyData);
    kern_write((void *) (KERN_SYSCALL_TBL_3 + (0x25 * 4)), (unsigned int) SCKernelCopyData);
    kern_write((void *) (KERN_SYSCALL_TBL_4 + (0x25 * 4)), (unsigned int) SCKernelCopyData);
    kern_write((void *) (KERN_SYSCALL_TBL_5 + (0x25 * 4)), (unsigned int) SCKernelCopyData);
}

/* Write a 32-bit word with kernel permissions */
void __attribute__((noinline)) kern_write(void *addr, uint32_t value) {
    asm volatile(
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
            : "r"(addr), "r"(value)
            : "memory", "ctr", "lr", "0", "3", "4", "5", "6", "7", "8", "9", "10",
              "11", "12");
}
