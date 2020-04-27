/****************************************************************************
 * Copyright (C) 2018 Maschell
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

#include <string>
#include <vector>
#include "RelocationData.h"

class ModuleData {
public:
    ModuleData() {
    }

    ~ModuleData() {
        for (auto const& reloc : relocation_data_list) {
            if(reloc != NULL) {
                delete reloc;
            }
        }
    }

    void setBSSLocation(uint32_t addr, uint32_t size) {
        this->bssAddr = addr;
        this->bssSize = size;
    }

    void setSBSSLocation(uint32_t addr, uint32_t size) {
        this->sbssAddr = addr;
        this->sbssSize = size;
    }

    void setEntrypoint(uint32_t addr) {
        this->entrypoint = addr;
    }

    void addRelocationData(RelocationData * relocation_data) {
        relocation_data_list.push_back(relocation_data);
    }

    std::vector<RelocationData *> getRelocationDataList() {
        return relocation_data_list;
    }

    uint32_t getBSSAddr() {
        return bssAddr;
    }

    uint32_t getBSSSize() {
        return bssSize;
    }

    uint32_t getSBSSAddr() {
        return sbssAddr;
    }

    uint32_t getSBSSSize() {
        return sbssSize;
    }

    uint32_t getEntrypoint() {
        return entrypoint;
    }

    std::string toString();
private:
    std::vector<RelocationData *> relocation_data_list;

    uint32_t bssAddr = 0;
    uint32_t bssSize = 0;
    uint32_t sbssAddr = 0;
    uint32_t sbssSize = 0;
    uint32_t entrypoint = 0;
};
