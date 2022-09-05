/*- pj-dis.c -- Disassemble picoJava instructions.
   Copyright (C) 1999 Free Software Foundation, Inc. 
   Contributed by Steve Chamberlain, of Transmeta (sac@pobox.com).

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */


#include "sysdep.h"
#include <stdio.h>
#include "opcode/pj.h"
#include "dis-asm.h"

extern const pj_opc_info_t pj_opc_info[512];

static int
get_int (memaddr, iptr, info)
     bfd_vma memaddr;
     int *iptr;
     struct disassemble_info *info;
{
  unsigned char b[4];

  int status = info->read_memory_func (memaddr, b, 4, info);

  *iptr = bfd_getb32 (b);

  return status;
}

int
print_insn_pj (addr, info)
     bfd_vma addr;
     struct disassemble_info *info;
{
  fprintf_ftype fprintf_fn = info->fprintf_func;
  void *stream = info->stream;
  unsigned char opcode;
  int status;
  info->bytes_per_line = 8;

  if ((status = info->read_memory_func (addr, &opcode, 1, info)))
    goto fail;

  if (opcode == 0xff)
    {
      unsigned char byte_2;
      if ((status = info->read_memory_func (addr + 1, &byte_2, 1, info)))
	goto fail;
      fprintf_fn (stream, "%s\t", pj_opc_info[opcode + byte_2].name);
      return 2;
    }
  else
    {
      char *sep = "\t";

      int insn_start = addr;
      const pj_opc_info_t *op = &pj_opc_info[opcode];
      int a;
      unsigned char disp[2];
      unsigned char next[5];
      unsigned char revop[256];


      addr++;

      /* Notice a sipush/sethi and print a nicer name. */

      if (op->opcode == SIPUSH
	  && !info->read_memory_func (addr, &next[0], 5, info)
	  && next[2] == SETHI)
	{
	  int val =
	    (next[0] << 8) | next[1] | (next[3] << 24) | (next[4] << 16);
	  fprintf_fn (stream, "ipush\t");
	  (*info->print_address_func) (val, info);
	  return 6;
	}
      /* Notice a short branch over a long branch, and print a nicer
         name. */

      if (op->arg[0] == O_R16
	  && !info->read_memory_func (addr, &disp[0], 2, info)
	  && disp[0] == 0 && disp[1] == 8
	  && (pj_opc_rev_init (revop), 1)
	  && revop[opcode]
	  && !info->read_memory_func (addr + 2, &next[0], 5, info)
	  && next[0] == GOTO_W)
	{
	  int i;
	  int val = 0;
	  op = &pj_opc_info[revop[opcode]];
	  for (i = 0; i < 4; i++)
	    val = (val << 8) | (next[i + 1] & 0xff);
	  fprintf_fn (stream, "%s\t", op->name);
	  (*info->print_address_func) (val + insn_start + 3, info);
	  return 8;
	}

      fprintf_fn (stream, "%s", op->name);

      /* The tableswitch instruction is followed by the default
         address, low value, high value and the destinations. */

      if (opcode == TABLESWITCH)
	{
	  int lowval;
	  int highval;
	  int defval;
	  int j;
	  int table_start = (addr + 4) & ~3;
	  int range;

	  if ((status = get_int (table_start + 4, &lowval, info)))
	    goto fail;

	  if ((status = get_int (table_start + 8, &highval, info)))
	    goto fail;

	  fprintf_fn (stream, " %d..%d default: ", lowval, highval);

	  range = highval - lowval + 1;

	  (*info->print_address_func) (defval + insn_start, info);

	  for (j = 0; j < range;)
	    {
	      int val_this;
	      int val_next;
	      int also_matches;
	      if (
		  (status =
		   get_int (table_start + j * 4 + 12, &val_this, info)))
		goto fail;
	      /* See if more than one entry goes to the same place. */

	      for (also_matches = 0;
		   (j + also_matches + 1 < range)
		   && (get_int (table_start + (j + also_matches + 1) * 4 + 12,
				&val_next, info) == 0)
		   && (val_this == val_next); also_matches++)
		;

	      if (also_matches)
		fprintf_fn (stream, "\n\tcase %5d...%5d:",
			    j + lowval, j + lowval + also_matches);
	      else
		fprintf_fn (stream, "\n\tcase %5d:", j + lowval);

	      fprintf_fn (stream, "\t[pc+%08x = ", val_this);
	      (*info->print_address_func) (val_this + insn_start, info);
	      fprintf_fn (stream, "]");
	      j += also_matches + 1;
	    }
	  return table_start + range * 4 + 12 - insn_start;
	}

      /* The lookupswitch instruction is followed by the default
         address, element count and pairs of values and
         addresses. */

      if (opcode == LOOKUPSWITCH)
	{
	  int count;
	  int val;

	  addr = (addr + 4) & ~3;
	  if ((status = get_int (addr, &val, info)))
	    goto fail;
	  addr += 4;

	  fprintf_fn (stream, " default: ");
	  (*info->print_address_func) (val + insn_start, info);

	  if ((status = get_int (addr, &count, info)))
	    goto fail;
	  addr += 4;

	  while (count--)
	    {
	      if ((status = get_int (addr, &val, info)))
		goto fail;
	      addr += 4;
	      fprintf_fn (stream, " %d:[", val);

	      if ((status = get_int (addr, &val, info)))
		goto fail;
	      addr += 4;

	      (*info->print_address_func) (val + insn_start, info);
	      fprintf_fn (stream, " ]");
	    }
	  return addr - insn_start;
	}

      for (a = 0; op->arg[a]; a++)
	{
	  unsigned char data[4];
	  int val = 0;
	  int i;
	  int size = ASIZE (op->arg[a]);

	  if ((status = info->read_memory_func (addr, data, size, info)))
	    goto fail;

	  val = (UNS (op->arg[a]) || ((data[0] & 0x80) == 0)) ? 0 : -1;

	  for (i = 0; i < size; i++)
	    val = (val << 8) | (data[i] & 0xff);

	  fprintf_fn (stream, "%s", sep);

	  if (PCREL (op->arg[a]))
	    (*info->print_address_func) (val + insn_start, info);
	  else
	    fprintf_fn (stream, "%d", val);

	  sep = ",";
	  addr += size;
	}
      return op->len;
    }

fail:
  info->memory_error_func (status, addr, info);
  return -1;
}
