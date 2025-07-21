/* BFD support for PDP-10.
   Copyright (C) 2025 Free Software Foundation, Inc.
   Contributed by Mikael Pettersson <mikpelinux@gmail.com>.

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

#include "sysdep.h"
#include "bfd.h"
#include "libbfd.h"

/* Any 36-bit word that is to be interpreted by the PDP-10 as code
   or data is extended to 64 bits, by zero-extending each 9-bit byte
   (nonet) to 16 bits.  Addressing is octet based with aligned datums:
   a multipe of 2 for nonets, 4 for half-words, and 8 for words.
   Remaining data in object files, executables, and archive files
   have ordinary octet-based representation.  */

const bfd_arch_info_type bfd_pdp10_arch =
{
  64,				/* 64 bits in a word.  */
  64,				/* 64 bits in an address.  */
  8,				/* 8 bits in a byte.  */
  bfd_arch_pdp10,		/* Architecture.  */
  bfd_mach_pdp10,		/* Machine number.  */
  "pdp10",			/* Architecture name.  */
  "pdp10",			/* Printable name.  */
  3,				/* Section align power.  */ /* TBD */
  true, 			/* This is the default architecture.  */
  bfd_default_compatible,	/* Architecture comparison function.  */
  bfd_default_scan,		/* String to architecture conversion.  */
  bfd_arch_default_fill,	/* Default fill.  */
  NULL,				/* Next in list.  */
  0 /* Maximum offset of a reloc from the start of an insn.  */
};
