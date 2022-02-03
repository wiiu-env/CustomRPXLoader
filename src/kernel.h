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

#define KERN_SYSCALL_TBL_1 0xFFE84C70 // unknown
#define KERN_SYSCALL_TBL_2 0xFFE85070 // works with games
#define KERN_SYSCALL_TBL_3 0xFFE85470 // works with loader
#define KERN_SYSCALL_TBL_4 0xFFEAAA60 // works with home menu
#define KERN_SYSCALL_TBL_5 0xFFEAAE60 // works with browser (previously KERN_SYSCALL_TBL)

#ifdef __cplusplus
extern "C" {
#endif

void doKernelSetup();

void doKernelSetup2();

void revertMainHook();

#ifdef __cplusplus
}
#endif