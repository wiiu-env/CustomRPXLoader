#pragma once

#include "common/module_defines.h"
#include "ModuleData.h"

class ModuleDataPersistence {
public:
    static bool saveModuleData(module_information_t * moduleInformation, ModuleData * module);
    static std::vector<ModuleData*> loadModuleData(module_information_t * moduleInformation);
};
