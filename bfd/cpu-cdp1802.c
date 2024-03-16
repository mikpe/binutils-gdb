/* BFD support for the RCA CDP1802 processor.
   Copyright (C) 2024 Free Software Foundation, Inc.
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

const bfd_arch_info_type bfd_cdp1802_arch =
{
  16,				/* Bits in a word.  */
  16,				/* Bits in an address.  */
  8,				/* 8 bits in a byte.  */
  bfd_arch_cdp1802,		/* Architecture.  */
  0,				/* One one machine.  */
  "cdp1802",			/* Architecture name.  */
  "cdp1802",			/* Printable name.  */
  1,				/* Section align power.  */
  true, 			/* This is the default architecture.  */
  bfd_default_compatible,	/* Architecture comparison function.  */
  bfd_default_scan,		/* String to architecture conversion.  */
  bfd_arch_default_fill,	/* Default fill.  */
  NULL,				/* Next in list.  */
  0 /* Maximum offset of a reloc from the start of an insn.  */
};
