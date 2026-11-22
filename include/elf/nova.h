/* DG NOVA ELF support for BFD.
   Copyright (C) 2026 Free Software Foundation, Inc.
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

#ifndef _ELF_NOVA_H
#define _ELF_NOVA_H

#include "elf/reloc-macros.h"

/* Relocations.  */
START_RELOC_NUMBERS (elf_nova_reloc_type)
  RELOC_NUMBER (R_NOVA_NONE, 0)
  RELOC_NUMBER (R_NOVA_GNU_VTINHERIT, 1)
  RELOC_NUMBER (R_NOVA_GNU_VTENTRY, 2)
  RELOC_NUMBER (R_NOVA_8, 3)
  RELOC_NUMBER (R_NOVA_HI8, 4)
  RELOC_NUMBER (R_NOVA_LO8, 5)
  RELOC_NUMBER (R_NOVA_16, 6)
  RELOC_NUMBER (R_NOVA_32, 7)
END_RELOC_NUMBERS (R_NOVA_max)

#endif /* _ELF_NOVA_H */
