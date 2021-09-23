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

#include "RelocationData.h"
#include "utils/StringTools.h"

std::string RelocationData::toString() const {
    return StringTools::strfmt("%s destination: %08X offset: %08X type: %02X addend: %d rplName: %s isData: %d \n", name.c_str(), destination, offset, type, addend, rplInfo.getName().c_str(),
                               rplInfo.isData());
}
