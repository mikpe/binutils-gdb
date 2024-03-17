/* Opcodes support for the RCA CDP1802 processor.
   Copyright (C) 2024 Free Software Foundation, Inc.
   Contributed by Mikael Pettersson <mikpelinux@gmail.com>.

   This file is part of GNU Binutils.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the license, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING3.  If not,
   see <http://www.gnu.org/licenses/>.  */

#ifndef _OPCODE_CDP1802_H_
#define _OPCODE_CDP1802_H_

enum cdp1802_isa_level {
  ISA_1802 = 0,
  ISA_1804 = 1,		/* Superset of 1802, 22 added opcodes, all with 0x68 prefix.  */
  ISA_1804AC = 2,	/* Superset of 1804, 10 added opcodes, all with 0x68 prefix.  */
};

enum cdp1802_operand_flags {
  OPND_NONE = 0,	/* Opcode has no operands.  */
  OPND_N = 1,		/* Opcode contains a small immediate, N.  */
  OPND_PORT = 2,	/* N is a 3-bit port not a 4-bit register.  */
  OPND_IMM1 = 4,	/* Opcode is followed by a 1-byte immediate.  */
  OPND_IMM2 = 8,	/* Opcode is followed by a 2-byte big-endian immediate.  */
  OPND_LAB = 16,	/* The immediate is a code label.  */
};

struct cdp1802_opcode {
  char name[4] ATTRIBUTE_NONSTRING;	/* 4-character names are NOT terminated by a '\0'.  */
  unsigned char opcode;
  unsigned int isa_level:2;
  unsigned int operand_flags:5;
};

extern const struct cdp1802_opcode cdp1802_opcodes[];
extern const unsigned int cdp1802_nr_opcodes;
extern const struct cdp1802_opcode *cdp1802_lookup_opcode (const char *name, size_t len);

#endif /* _OPCODE_CDP1802_H_ */
