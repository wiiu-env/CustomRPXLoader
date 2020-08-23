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

#include "ModuleData.h"
#include "utils/StringTools.h"

std::string ModuleData::toString() const {
    std::string res = StringTools::strfmt("Entrypoint %08X, bss: %08X (%d), bss: %08X (%d)\n", getEntrypoint(), getBSSAddr(), getBSSSize(), getSBSSAddr(), getSBSSSize());
    for (auto const &reloc : relocation_data_list) {
        res += reloc.toString();
    }
    return res;
}