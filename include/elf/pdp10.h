/* PDP-10 ELF support for BFD.
   Copyright (C) 2025 Free Software Foundation, Inc.
   Contributed by Mikael Pettersson <mikpelinux@gmail.com>

   This file is part of BFD, the Binary File Descriptor library.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING3.  If not,
   see <http://www.gnu.org/licenses/>.  */

#ifndef _ELF_PDP10_H
#define _ELF_PDP10_H

#include "elf/reloc-macros.h"

/* Relocations.  */
START_RELOC_NUMBERS (elf_pdp10_reloc_type)
  RELOC_NUMBER (R_PDP10_NONE, 0)
  RELOC_NUMBER (R_PDP10_GNU_VTINHERIT, 1)
  RELOC_NUMBER (R_PDP10_GNU_VTENTRY, 2)
  RELOC_NUMBER (R_PDP10_8, 3)
  RELOC_NUMBER (R_PDP10_16, 4)
  RELOC_NUMBER (R_PDP10_32, 5)
  RELOC_NUMBER (R_PDP10_64, 6)
  RELOC_NUMBER (R_PDP10_9, 7)
  RELOC_NUMBER (R_PDP10_18, 8)
  RELOC_NUMBER (R_PDP10_36, 9)
  RELOC_NUMBER (R_PDP10_IFIW, 10)
  RELOC_NUMBER (R_PDP10_EFIW, 11)
  RELOC_NUMBER (R_PDP10_BPTR, 12)
  RELOC_NUMBER (R_PDP10_HPTR, 13)
END_RELOC_NUMBERS (R_PDP10_max)

#endif /* _ELF_PDP10_H */
