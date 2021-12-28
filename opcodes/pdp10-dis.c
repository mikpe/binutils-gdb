/* Disassembly support for PDP-10.
   Copyright (C) 2023-2025 Free Software Foundation, Inc.
   Contributed by Mikael Pettersson <mikpelinux@gmail.com>.

   This file is part of the GNU opcodes library.

   This library is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   It is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING3.  If not,
   see <http://www.gnu.org/licenses/>.  */

#include "sysdep.h"
#include "disassemble.h"
#include "opcode/pdp10.h"
#include "opintl.h"
#include "libiberty.h"
#include <stdint.h>

struct pdp10_option
{
    const char *name;
    const char *description;
};

static const struct pdp10_option options[] =
{
  { "dummy", "no effect" }  /* TODO: allow to override CPU and possibly section */
};

void
disassemble_init_pdp10 (struct disassemble_info *info)
{
  /* TODO: use low 16 bits of info->flags */
  /* TODO: use info->private_data for a quick lookup table */
  /* TODO: parse info->disassembler_options */
  info->bytes_per_line = 4;
  info->octets_per_byte = 2;
}

int
print_insn_pdp10 (bfd_vma memaddr, struct disassemble_info *info)
{
  int status;
  bfd_byte octets[8];
  unsigned int i;
  uint64_t word;

  status = info->read_memory_func (memaddr, octets, 8, info);
  if (status != 0)
    {
      info->memory_error_func (status, memaddr, info);
      return -1;
    }

  /* reassemble a 36-bit big-endian word stored sparsely in 8 octets */
  word = 0;
  for (i = 0; i < 4; ++i)
    {
      unsigned char hi = octets[2*i];
      unsigned char lo = octets[2*i+1];
      unsigned int nonet = ((hi & 1) << 9) + lo;
      word = (word << 9) + nonet;
    }

  /* TODO: use the opcode table here */
  info->fprintf_func (info->stream, ".long\t%0*" PRIo64, 12, word);
  return 8;
}

void
print_pdp10_disassembler_options (FILE *stream)
{
  unsigned int i, max_len;

  fprintf (stream, _("\n\
The following PDP-10 disassembler options are supported for use\n\
with the -M switch (multiple options should be separated by commas):\n"));

  max_len = 0;
  for (i = 0; i < ARRAY_SIZE (options); ++i)
    {
	unsigned int len = strlen (options[i].name);
	if (len > max_len)
	  max_len = len;
    }

  for (i = 0; i < ARRAY_SIZE (options); ++i)
    fprintf (stream,  "  %s%*c  %s\n",
	     options[i].name,
	     (int)(max_len - strlen (options[i].name)), ' ',
	     _(options[i].description));
}
