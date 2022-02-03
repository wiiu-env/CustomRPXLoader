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

#include "utils/logger.h"
#include <optional>
#include <string>

class ImportRPLInformation {

public:
    explicit ImportRPLInformation(std::string name, bool isData = false) {
        this->name    = name;
        this->_isData = isData;
    }

    ~ImportRPLInformation() = default;

    static std::optional<ImportRPLInformation> createImportRPLInformation(std::string rawSectionName) {
        std::string fimport = ".fimport_";
        std::string dimport = ".dimport_";

        bool data = false;

        std::string rplName;

        if (rawSectionName.size() < fimport.size()) {
            return {};
        } else if (std::equal(fimport.begin(), fimport.end(), rawSectionName.begin())) {
            rplName = rawSectionName.substr(fimport.size());
        } else if (std::equal(dimport.begin(), dimport.end(), rawSectionName.begin())) {
            rplName = rawSectionName.substr(dimport.size());
            data    = true;
        } else {
            DEBUG_FUNCTION_LINE("invalid section name\n");
            return {};
        }
        return ImportRPLInformation(rplName, data);
    }

    [[nodiscard]] std::string getName() const {
        return name;
    }

    [[nodiscard]] bool isData() const {
        return _isData;
    }

private:
    std::string name;
    bool _isData = false;
};