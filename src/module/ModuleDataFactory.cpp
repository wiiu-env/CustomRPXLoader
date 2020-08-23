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

#include <string>
#include <vector>
#include <map>
#include <coreinit/cache.h>
#include "ModuleDataFactory.h"
#include "elfio/elfio.hpp"
#include "utils/utils.h"
#include "../ElfUtils.h"

using namespace ELFIO;

ModuleData *ModuleDataFactory::load(std::string path, uint32_t destination_address, uint32_t maximum_size, relocation_trampolin_entry_t *trampolin_data, uint32_t trampolin_data_length) {
    elfio reader;
    ModuleData *moduleData = new ModuleData();
    if (moduleData == NULL) {
        return NULL;
    }

    // Load ELF data
    if (!reader.load(path)) {
        DEBUG_FUNCTION_LINE("Can't find or process ELF file");
        delete moduleData;
        return NULL;
    }

    uint32_t sec_num = reader.sections.size();

    uint8_t **destinations = (uint8_t **) malloc(sizeof(uint8_t *) * sec_num);


    uint32_t sizeOfModule = 0;
    for (uint32_t i = 0; i < sec_num; ++i) {
        section *psec = reader.sections[i];
        if (psec->get_type() == 0x80000002) {
            continue;
        }

        if ((psec->get_type() == SHT_PROGBITS || psec->get_type() == SHT_NOBITS) && (psec->get_flags() & SHF_ALLOC)) {
            sizeOfModule += psec->get_size() + 1;
        }
    }

    if (sizeOfModule > maximum_size) {
        DEBUG_FUNCTION_LINE("Module is too big.");
        return NULL;
    }

    uint32_t baseOffset = (destination_address - sizeOfModule) & 0xFFFFFF00;

    uint32_t offset_text = baseOffset;
    uint32_t offset_data = offset_text;

    uint32_t entrypoint = offset_text + (uint32_t) reader.get_entry() - 0x02000000;

    uint32_t totalSize = 0;

    for (uint32_t i = 0; i < sec_num; ++i) {
        section *psec = reader.sections[i];
        if (psec->get_type() == 0x80000002) {
            continue;
        }

        if ((psec->get_type() == SHT_PROGBITS || psec->get_type() == SHT_NOBITS) && (psec->get_flags() & SHF_ALLOC)) {
            uint32_t sectionSize = psec->get_size();
            uint32_t address = (uint32_t) psec->get_address();

            destinations[psec->get_index()] = (uint8_t *) baseOffset;

            uint32_t destination = baseOffset + address;
            if ((address >= 0x02000000) && address < 0x10000000) {
                destination -= 0x02000000;
                destinations[psec->get_index()] -= 0x02000000;
                baseOffset += sectionSize;
                offset_data += sectionSize;
            } else if ((address >= 0x10000000) && address < 0xC0000000) {
                destination -= 0x10000000;
                destinations[psec->get_index()] -= 0x10000000;
            } else if (address >= 0xC0000000) {
                destination -= 0xC0000000;
                destinations[psec->get_index()] -= 0xC0000000;
            } else {
                DEBUG_FUNCTION_LINE("Unhandled case");
                free(destinations);
                delete moduleData;
                return NULL;
            }

            const char *p = reader.sections[i]->get_data();

            if (psec->get_type() == SHT_NOBITS) {
                DEBUG_FUNCTION_LINE("memset section %s %08X [%08X] to 0 (%d bytes)", psec->get_name().c_str(), destination, destination + sectionSize, sectionSize);
                memset((void *) destination, 0, sectionSize);
            } else if (psec->get_type() == SHT_PROGBITS) {
                DEBUG_FUNCTION_LINE("Copy section %s %08X -> %08X [%08X] (%d bytes)", psec->get_name().c_str(), p, destination, destination + sectionSize, sectionSize);
                memcpy((void *) destination, p, sectionSize);
            }

            //nextAddress = ROUNDUP(destination + sectionSize,0x100);
            if (psec->get_name().compare(".bss") == 0) {
                moduleData->setBSSLocation(destination, sectionSize);
                DEBUG_FUNCTION_LINE("Saved %s section info. Location: %08X size: %08X", psec->get_name().c_str(), destination, sectionSize);
            } else if (psec->get_name().compare(".sbss") == 0) {
                moduleData->setSBSSLocation(destination, sectionSize);
                DEBUG_FUNCTION_LINE("Saved %s section info. Location: %08X size: %08X", psec->get_name().c_str(), destination, sectionSize);
            }
            totalSize += sectionSize;

            DCFlushRange((void *) destination, sectionSize);
            ICInvalidateRange((void *) destination, sectionSize);
        }
    }

    for (uint32_t i = 0; i < sec_num; ++i) {
        section *psec = reader.sections[i];
        if ((psec->get_type() == SHT_PROGBITS || psec->get_type() == SHT_NOBITS) && (psec->get_flags() & SHF_ALLOC)) {
            DEBUG_FUNCTION_LINE("Linking (%d)... %s", i, psec->get_name().c_str());
            if (!linkSection(reader, psec->get_index(), (uint32_t) destinations[psec->get_index()], offset_text, offset_data, trampolin_data, trampolin_data_length)) {
                DEBUG_FUNCTION_LINE("elfLink failed");
                free(destinations);
                delete moduleData;
                return NULL;
            }
        }
    }
    std::vector<RelocationData *> relocationData = getImportRelocationData(reader, destinations);

    for (auto const &reloc : relocationData) {
        moduleData->addRelocationData(reloc);
    }

    DCFlushRange((void *) destination_address, totalSize);
    ICInvalidateRange((void *) destination_address, totalSize);

    free(destinations);

    moduleData->setEntrypoint(entrypoint);
    DEBUG_FUNCTION_LINE("Saved entrypoint as %08X", entrypoint);

    return moduleData;
}


std::vector<RelocationData *> ModuleDataFactory::getImportRelocationData(elfio &reader, uint8_t **destinations) {
    std::vector<RelocationData *> result;
    std::map<uint32_t, std::string> infoMap;

    uint32_t sec_num = reader.sections.size();

    for (uint32_t i = 0; i < sec_num; ++i) {
        section *psec = reader.sections[i];
        if (psec->get_type() == 0x80000002) {
            infoMap[i] = psec->get_name();
        }
    }

    for (uint32_t i = 0; i < sec_num; ++i) {
        section *psec = reader.sections[i];
        if (psec->get_type() == SHT_RELA || psec->get_type() == SHT_REL) {
            DEBUG_FUNCTION_LINE("Found relocation section %s", psec->get_name().c_str());
            relocation_section_accessor rel(reader, psec);
            for (uint32_t j = 0; j < (uint32_t) rel.get_entries_num(); ++j) {
                Elf64_Addr offset;
                Elf_Word type;
                Elf_Sxword addend;
                std::string sym_name;
                Elf64_Addr sym_value;
                Elf_Half sym_section_index;

                if (!rel.get_entry(j, offset, sym_value, sym_name, type, addend, sym_section_index)) {
                    DEBUG_FUNCTION_LINE("Failed to get relocation");
                    break;
                }

                uint32_t adjusted_sym_value = (uint32_t) sym_value;
                if (adjusted_sym_value < 0xC0000000) {
                    continue;
                }
                ImportRPLInformation *rplInfo = ImportRPLInformation::createImportRPLInformation(infoMap[sym_section_index]);
                if (rplInfo == NULL) {
                    DEBUG_FUNCTION_LINE("Failed to create import information");
                    break;
                }

                uint32_t section_index = psec->get_info();

                // When these relocations are performed, we don't need the 0xC0000000 offset anymore.
                RelocationData *relocationData = new RelocationData(type, offset - 0x02000000, addend, (void *) (destinations[section_index] + 0x02000000), sym_name, rplInfo);
                //relocationData->printInformation();
                result.push_back(relocationData);
            }
        }
    }
    return result;
}

bool ModuleDataFactory::linkSection(elfio &reader, uint32_t section_index, uint32_t destination, uint32_t base_text, uint32_t base_data, relocation_trampolin_entry_t *trampolin_data, uint32_t trampolin_data_length) {
    uint32_t sec_num = reader.sections.size();

    for (uint32_t i = 0; i < sec_num; ++i) {
        section *psec = reader.sections[i];
        if (psec->get_info() == section_index) {
            DEBUG_FUNCTION_LINE("Found relocation section %s", psec->get_name().c_str());
            relocation_section_accessor rel(reader, psec);
            for (uint32_t j = 0; j < (uint32_t) rel.get_entries_num(); ++j) {
                Elf64_Addr offset;
                Elf_Word type;
                Elf_Sxword addend;
                std::string sym_name;
                Elf64_Addr sym_value;
                Elf_Half sym_section_index;

                if (!rel.get_entry(j, offset, sym_value, sym_name, type, addend, sym_section_index)) {
                    DEBUG_FUNCTION_LINE("Failed to get relocation");
                    break;
                }

                uint32_t adjusted_sym_value = (uint32_t) sym_value;
                if ((adjusted_sym_value >= 0x02000000) && adjusted_sym_value < 0x10000000) {
                    adjusted_sym_value -= 0x02000000;
                    adjusted_sym_value += base_text;
                } else if ((adjusted_sym_value >= 0x10000000) && adjusted_sym_value < 0xC0000000) {
                    adjusted_sym_value -= 0x10000000;
                    adjusted_sym_value += base_data;
                } else if (adjusted_sym_value >= 0xC0000000) {
                    // Skip imports
                    continue;
                } else if (adjusted_sym_value == 0x0) {
                    //
                } else {
                    DEBUG_FUNCTION_LINE("Unhandled case %08X", adjusted_sym_value);
                    return false;
                }

                if (sym_section_index == SHN_ABS) {
                    //
                } else if (sym_section_index > SHN_LORESERVE) {
                    DEBUG_FUNCTION_LINE("NOT IMPLEMENTED: %04X", sym_section_index);
                    return false;
                }

                if (!ElfUtils::elfLinkOne(type, offset, addend, destination, adjusted_sym_value, trampolin_data, trampolin_data_length, RELOC_TYPE_FIXED)) {
                    DEBUG_FUNCTION_LINE("Link failed");
                    return false;
                }
            }
            DEBUG_FUNCTION_LINE("done");
        }
    }
    return true;
}
