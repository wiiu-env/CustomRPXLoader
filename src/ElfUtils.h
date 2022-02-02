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

#pragma once

#include "common/relocation_defines.h"
#include <cstddef>
#include <cstdint>

#ifdef __cplusplus
extern "C" {
#endif

// clang-format off
#define R_PPC_NONE              0
#define R_PPC_ADDR32            1
#define R_PPC_ADDR16_LO         4
#define R_PPC_ADDR16_HI         5
#define R_PPC_ADDR16_HA         6
#define R_PPC_REL24             10
#define R_PPC_REL14             11
#define R_PPC_DTPMOD32          68
#define R_PPC_DTPREL32          78
#define R_PPC_EMB_SDA21         109
#define R_PPC_EMB_RELSDA        116
#define R_PPC_DIAB_SDA21_LO     180
#define R_PPC_DIAB_SDA21_HI     181
#define R_PPC_DIAB_SDA21_HA     182
#define R_PPC_DIAB_RELSDA_LO    183
#define R_PPC_DIAB_RELSDA_HI    184
#define R_PPC_DIAB_RELSDA_HA    185
#define R_PPC_GHS_REL16_HA      251
#define R_PPC_GHS_REL16_HI      252
#define R_PPC_GHS_REL16_LO      253

// Masks for manipulating Power PC relocation targets
#define PPC_WORD32              0xFFFFFFFF
#define PPC_WORD30              0xFFFFFFFC
#define PPC_LOW24               0x03FFFFFC
#define PPC_LOW14               0x0020FFFC
#define PPC_HALF16              0xFFFF
// clang-format on

#ifdef __cplusplus
}
#endif

class ElfUtils {

public:
    static bool elfLinkOne(char type, size_t offset, int32_t addend, uint32_t destination, uint32_t symbol_addr, relocation_trampolin_entry_t *trampolin_data, uint32_t trampolin_data_length,
                           RelocationType reloc_type);
};
