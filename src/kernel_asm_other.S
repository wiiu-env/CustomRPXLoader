#define BAT_SETUP_HOOK_ADDR         0xFFF1D624

#define BAT_SETUP_HOOK_ENTRY        0x00FFF000

    .globl KernelPatchesFinal
KernelPatchesFinal:
    # store the old DBAT0
    mfdbatu r5, 0
    mfdbatl r6, 0

    # memory barrier
    eieio
    isync

    # setup DBAT0 for access to kernel code memory
    lis r3, 0xFFF0
    ori r3, r3, 0x0002
    mtdbatu 0, r3
    lis r3, 0xFFF0
    ori r3, r3, 0x0032
    mtdbatl 0, r3

    # memory barrier
    eieio
    isync

    # SaveAndResetDataBATs_And_SRs hook setup, but could be any BAT function though
    # just chosen because its simple
    lis r3, BAT_SETUP_HOOK_ADDR@h
    ori r3, r3, BAT_SETUP_HOOK_ADDR@l

    lis r4, (BAT_SETUP_HOOK_ENTRY | 0x48000003)@h      #   bla BAT_SETUP_HOOK_ENTRY
    ori r4, r4, (BAT_SETUP_HOOK_ENTRY | 0x48000003)@l
    stw r4, 0x24(r3)

    # flush and invalidate the replaced instructions
    lis r3, (BAT_SETUP_HOOK_ADDR & ~31)@h
    ori r3, r3, (BAT_SETUP_HOOK_ADDR & ~31)@l
    dcbf 0, r3
    icbi 0, r3
    lis r3, ((BAT_SETUP_HOOK_ADDR + 0x40) & ~31)@h
    ori r3, r3, ((BAT_SETUP_HOOK_ADDR + 0x40) & ~31)@l
    dcbf 0, r3
    icbi 0, r3
    sync

    # memory barrier
    eieio
    isync

    # restore DBAT 0 and return from interrupt
    mtdbatu 0, r5
    mtdbatl 0, r6

    # memory barrier
    eieio
    isync

    blr

