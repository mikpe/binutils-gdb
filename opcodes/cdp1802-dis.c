/* Disassembly support for the RCA CDP1802 processor.
   Copyright (C) 2024 Free Software Foundation, Inc.
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
#include "opcode/cdp1802.h"
#include "opintl.h"
#include "libiberty.h"

struct cdp1802_private_data {
  /* ISA level, for filtering out invalid opcodes.  */
  unsigned char isa_level;

  /* Maps opcode bytes to indices in cdp1802_opcodes[].
     The first 256 entries are for the first opcode byte.
     The second 256 entries are for the second opcode byte, prefixed by 0x68.  */
  unsigned char index[256*2];
};

static const struct {
  const char *name;
  unsigned char val;
  const char *description;
} cdp1802_options[] = {
  {"1802", ISA_1802, "only recognize CDP1802 opcodes"},
  {"1804", ISA_1804, "only recognize CDP1804 (non-AC) opcodes"},
  {"1804AC", ISA_1804AC, "recognize all CDP1804AC opcodes (default)"},
};

static void
parse_cdp1802_disassembler_option (struct cdp1802_private_data *priv, const char *option, unsigned int len)
{
  unsigned int i;

  for (i = 0; i < ARRAY_SIZE (cdp1802_options); ++i)
    if (strncmp (cdp1802_options[i].name, option, len) == 0
	&& strlen (cdp1802_options[i].name) == len)
      {
	priv->isa_level = cdp1802_options[i].val;
	return;
      }

  opcodes_error_handler (_("unrecognized disassembler option: %.*s"), len, option);
}

static void
parse_cdp1802_disassembler_options (struct disassemble_info *info)
{
  const char *options;
  struct cdp1802_private_data *priv;

  options = info->disassembler_options;
  priv = (struct cdp1802_private_data *) info->private_data;
  if (options == NULL || priv == NULL)
    return;

  while (*options != '\0' )
    {
      char *comma;
      size_t len;

      comma = strchr (options, ',');
      if (comma)
	len = comma - options;
      else
	len = strlen (options);

      parse_cdp1802_disassembler_option (priv, options, len);

      if (comma == NULL)
	break;

      options += len + 1;
    }

  /* To avoid repeated parsing of these options, we remove them here.  */
  info->disassembler_options = NULL;
}

void
print_cdp1802_disassembler_options (FILE *stream)
{
  unsigned int i, max_len;

  fprintf (stream, _("\n\
The following CDP1802 specific disassembler options are supported for use\n\
with the -M switch (multiple options should be separated by commas):\n"));

  max_len = 0;
  for (i = 0; i < ARRAY_SIZE (cdp1802_options); ++i)
    {
      unsigned int len = strlen (cdp1802_options[i].name);
      if (len > max_len)
	max_len = len;
    }

  for (i = 0; i < ARRAY_SIZE (cdp1802_options); ++i)
    fprintf (stream,  "  %s%*c  %s\n",
	     cdp1802_options[i].name,
	     (int)(max_len - strlen (cdp1802_options[i].name)), ' ',
	     _(cdp1802_options[i].description));
}

void
disassemble_init_cdp1802 (struct disassemble_info *info)
{
  struct cdp1802_private_data *priv;
  unsigned int nr_opcodes, i;

  if (info->private_data)
    return;

  priv = malloc (sizeof *priv);
  memset (priv, -1, sizeof *priv);
  priv->isa_level = ISA_1804AC;
  info->private_data = priv;

  parse_cdp1802_disassembler_options (info);

  nr_opcodes = cdp1802_nr_opcodes;
  for (i = 0; i < nr_opcodes; ++i)
    {
      unsigned char opcode;
      unsigned int offset;

      opcode = cdp1802_opcodes[i].opcode;

      if (cdp1802_opcodes[i].isa_level > ISA_1802)
	offset = 256;
      else
	offset = 0;

      if (cdp1802_opcodes[i].operand_flags & OPND_N)
	{
	  unsigned char imm, immhi;

	  if (cdp1802_opcodes[i].operand_flags & OPND_PORT)
	    {
	      imm = 1;
	      immhi = 7;
	    }
	  else
	    {
	      imm = 0;
	      immhi = 15;
	    }
	  for (; imm <= immhi; ++imm)
	    priv->index[offset + opcode + imm] = i;
	}
      else
	priv->index[offset + opcode] = i;
    }
}

static int
cdp1802_read_byte (struct disassemble_info *info, bfd_vma memaddr, bfd_byte *dst)
{
  int status;

  status = info->read_memory_func (memaddr, dst, 1, info);
  if (status != 0)
    info->memory_error_func (status, memaddr, info);
  return status;
}

int
print_insn_cdp1802 (bfd_vma memaddr, struct disassemble_info *info)
{
  const struct cdp1802_private_data *priv;
  const struct cdp1802_opcode *opcode;
  int nr_bytes;
  unsigned char opcode_byte;
  unsigned char opcode_ix;
  bfd_byte insn[2], imm[2];

  parse_cdp1802_disassembler_options (info);

  if (cdp1802_read_byte (info, memaddr + 0, &insn[0]))
    return 0;

  priv = (struct cdp1802_private_data *) info->private_data;
  if (insn[0] == 0x68)
    {
      if (priv->isa_level == ISA_1802)
	{
	  info->fprintf_func (info->stream, ".byte\t0x%02x", insn[0]);
	  return 1;
	}
      if (cdp1802_read_byte (info, memaddr + 1, &insn[1]))
	return 0;
      opcode_byte = insn[1];
      opcode_ix = priv->index[256 + opcode_byte];
      if (opcode_ix == 0xff)
	{
	  info->fprintf_func (info->stream, ".byte\t0x%02x, 0x%02x", insn[0], insn[1]);
	  return 2;
	}
      nr_bytes = 2;
    }
  else
    {
      opcode_byte = insn[0];
      opcode_ix = priv->index[opcode_byte];
      nr_bytes = 1;
    }

  opcode = &cdp1802_opcodes[opcode_ix];

  if (opcode->operand_flags & (OPND_IMM1 | OPND_IMM2))
    {
      if (cdp1802_read_byte (info, memaddr + nr_bytes, &imm[0]))
	return 0;
      ++nr_bytes;
    }
  if (opcode->operand_flags & OPND_IMM2)
    {
      if (cdp1802_read_byte (info, memaddr + nr_bytes, &imm[1]))
	return 0;
      ++nr_bytes;
    }

  info->fprintf_func (info->stream, "%.*s", 4, opcode->name);

  if (opcode->operand_flags & (OPND_N | OPND_PORT))
    {
      unsigned char mask;

      mask = (opcode->operand_flags & OPND_PORT) ? 7 : 15;
      info->fprintf_func (info->stream, "\t%u", opcode_byte & mask);
    }

  if (opcode->operand_flags & (OPND_IMM1 | OPND_IMM2))
    {
      char pfx;
      unsigned short val;

      pfx = (opcode->operand_flags & (OPND_N | OPND_PORT)) ? ',' : '\t';

      if (opcode->operand_flags & OPND_IMM2)
	val = (imm[0] << 8) | imm[1];
      else
	val = imm[0];

      info->fprintf_func (info->stream, "%c%#x", pfx, val);
    }

  return nr_bytes;
}
