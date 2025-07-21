/* Opcodes support for PDP-10.
   Copyright (C) 2025 Free Software Foundation, Inc.
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

#ifndef _OPCODE_PDP10_H_
#define _OPCODE_PDP10_H_

#include <stdint.h>

/* Known PDP10 CPU models, each represented by a distinct bit value.

   These are combined with bit-wise 'and', 'or', and 'not' operations
   to form sets of CPU models, used to check if a given mnemonic or
   opcode is available for a selected set of CPUs.  */

/* DEC processors.  */

#define PDP6            (1 << 0)        /* PDP-6 Type 166 Arithmetic Processor.  */
#define PDP10_KA10      (1 << 1)        /* KA10.  */
#define PDP10_KA10_ITS  (1 << 2)        /* KA10, ITS microcode.  */
#define PDP10_KI10      (1 << 3)        /* KI10.  */
#define PDP10_KL10      (1 << 4)        /* KL10, early non-extended.  */
#define PDP10_KL10_ITS  (1 << 5)        /* KL10, ITS microcode.  */
#define PDP10_KL10_271  (1 << 6)        /* KL10B, microcode >= 271, extended.  */
#define PDP10_KS10      (1 << 7)        /* KS10.  */
#define PDP10_KS10_ITS  (1 << 8)        /* KS10, ITS microcode.  */

/* XKL Processors.  */

#define PDP10_XKL1      (1 << 9)        /* XKL-1 / TOAD-1.  */

#define PDP10_LAST      PDP10_XKL1

/* Others, not yet supported:

   DEC KC10 (Jupiter, KL10B successor with full extended addressing, not built).
   DEC KD10 (Minnow, KS10 successor with full extended addressing, not built).

   XKL-2 (XKL-1 successor, no documentation available).

   System Concepts SC-20, SC-25, SC-30M, SC-40 (KC10-like).

   Foonly F-1, F-2, F-3, F-4 (KI10/KL10-hybrid).

   Xerox PARC MAXC (KI10-like?).  */

/* Convenience constants for combinations of CPU models.  */

#define PDP10_ALL (PDP10_LAST | (PDP10_LAST - 1))

#define PDP10_KL10_271up (PDP10_KL10_271 | PDP10_XKL1)
#define PDP10_KL10any (PDP10_KL10 | PDP10_KL10_ITS | PDP10_KL10_271up)
#define PDP10_KL10up (PDP10_KL10any | PDP10_KS10)

#define PDP10_KI10_to_KL10 (PDP10_KI10 | PDP10_KL10any)
#define PDP10_KI10up (PDP10_KI10 | PDP10_KL10up)

#define PDP10_ITS (PDP10_KA10_ITS | PDP10_KL10_ITS | PDP10_KS10_ITS)

#define PDP10_KA10any (PDP10_KA10 | PDP10_KA10_ITS)
#define PDP10_KA10up (PDP10_KA10any | PDP10_KI10up)
#define PDP10_KA10up_not_ITS (PDP10_KA10up & ~PDP10_ITS)
#define PDP10_KA10_to_KI10 (PDP10_KA10 | PDP10_KI10) /* FIXME: should that be KA10any? */
#define PDP10_KA10_to_KL10 (PDP10_KA10_to_KI10 | PDP10_KL10any)

#define PDP10_not_KS10_or_XKL1 (PDP10_ALL & ~(PDP10_KS10 | PDP10_XKL1)) /* FIXME: should that be KS10any? */

#define PDP6_to_KI10 (PDP6 | PDP10_KA10_to_KI10)

typedef uint16_t pdp10_cpu_models_t;

/* This provides names for a subset of the CPU combinations above.  */

struct pdp10_cpu_model {
  const char *name;
  pdp10_cpu_models_t models;
};

extern const struct pdp10_cpu_model pdp10_cpu_models[];
extern const int pdp10_num_cpu_models;

/* Device fields in IO instructions.  */

typedef uint8_t pdp10_cpu_device_t;     /* [0,127] */

/* Instruction Representation
   ==========================

   Basic instructions are stored in 36-bit words with the following format:

	       111 1 1111 112222222222333333
    012345678 9012 3 4567 890123456789012345
   +---------+----+-+----+------------------+
   |  opcode |  A |I|  X |         Y        |
   +---------+----+-+----+------------------+
      9 bits    4  1   4        18 bits

   Bits are numbered 0 to 35 in left-to-right order, with bit 0 being the
   most significant and bit 35 the least significant.

   The high 9 bits contains a 9-bit opcode.
   A is a 4-bit field specifying the accumulator (a register).
   I is a 1-bit field specifying indirect addressing.
   X is a 4-bit field specifying the index register.
   Y is an 18-bit field specifying an address or offset.

   E, the effective addreess, is computed from I, X, and Y.

   In some instructions A contains further opcode bits,
   signalled by the PDP10_A_OPCODE flag.

   In some instructions A is unused and should be zero,
   signalled by the PDP10_A_UNUSED flag.

   In some instructions A must be non-zero,
   signalled by the PDP10_A_NONZERO flag.

   Instructions that not compute an effective address E
   should have I, X, and Y set to zero,
   signalled by the PDP10_E_UNUSED flag.

   IO instructions have a slightly different format:

		111 1 1111 112222222222333333
    012 3456789 012 3 4567 890123456789012345
   +---+-------+---+-+----+------------------+
   |op1| device|op2|I|  X |         Y        |
   +---+-------+---+-+----+------------------+
     3   7 bits  3  1   4       18 bits

   The op1 field is all-bits-one (7), the device field addresses the selected
   device, and the op2 field specifies the operation.  Both devices internal to
   the processor and devices attached via external buses can be accessed.

   Some non-IO instructions also have a 7 in their high three bits.

   Extended instructions consist of two separate instruction words:

   A:
	       111 1 1111 112222222222333333
    012345678 9012 3 4567 890123456789012345
   +---------+----+-+----+------------------+
   |    0123 |  A |I|  X |         Y        |
   +---------+----+-+----+------------------+
      9 bits    4  1   4        18 bits

   E0:
	       111 1 1111 112222222222333333
    012345678 9012 3 4567 890123456789012345
   +---------+----+-+----+------------------+
   | xopcode |0000|I|  X |         Y        |
   +---------+----+-+----+------------------+
      9 bits    4  1   4        18 bits

   The first word is stored at address A in the instruction stream, in the basic
   format with opcode 0123.  The second word is stored at the effective address
   E0 specified by the the first word.  Its accumulator field is unused and must
   be zero for compatibility with future extensions.  */

/* Each instruction belongs to exactly one of these primary formats,
   which determine how the high 13 bits are to be interpreted.  */

#define PDP10_FMT_BASIC         0x00
#define PDP10_FMT_A_OPCODE      0x01
#define PDP10_FMT_A_NONZERO     0x02
#define PDP10_FMT_IO            0x03

#define PDP10_FMT_MASK          0x03

/* Additional flags beyond the primary format.  */

#define PDP10_FMT_A_UNUSED      0x04
#define PDP10_FMT_E_UNUSED      0x08
#define PDP10_FMT_EXTENDED      0x10

struct pdp10_insn {
  const char *name;
  /* The high13 field is 13 bits, formatted as:
     <9 bit opcode><0000>       BASIC, A_NONZERO
     <9 + 4 bit opcode>         A_OPCODE
     <111><0000000><3 bit op>   IO

     An extended instruction uses the BASIC format with opcode 0123 for
     the first word, and the A_OPCODE(00) EXTENDED format for the second word.  */
  uint16_t high13;
  uint16_t format;
  pdp10_cpu_models_t models;
};

extern const struct pdp10_insn pdp10_insns[];
extern const int pdp10_num_insns;

extern pdp10_cpu_models_t pdp10_cpu_models_from_name (const char *name, size_t len);
extern int pdp10_cpu_device_from_name (const char *name, pdp10_cpu_models_t models);

#endif /* _OPCODE_PDP10_H_ */
