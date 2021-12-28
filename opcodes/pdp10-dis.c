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
#include <assert.h>
#include <stdint.h>

struct pdp10_private_data {
  /* ISA level.  */
  uint16_t isa_level;
};

static const struct {
  const char *name;
  uint16_t val;
  const char *description;
} pdp10_options[] = {
  { "dummy", 0, "no effect" }  /* TODO: allow to override CPU and possibly section */
};

static void
parse_pdp10_disassembler_option (struct pdp10_private_data *priv, const char *option, unsigned int len)
{
  unsigned int i;

  for (i = 0; i < ARRAY_SIZE (pdp10_options); ++i)
    if (strncmp (pdp10_options[i].name, option, len) == 0
	&& strlen (pdp10_options[i].name) == len)
      {
	priv->isa_level = pdp10_options[i].val;
	return;
      }

  opcodes_error_handler (_("unrecognized disassembler option: %.*s"), len, option);
}

static void
parse_pdp10_disassembler_options (struct disassemble_info *info)
{
  const char *options;
  struct pdp10_private_data *priv;

  options = info->disassembler_options;
  priv = (struct pdp10_private_data *) info->private_data;
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

      parse_pdp10_disassembler_option (priv, options, len);

      if (comma == NULL)
	break;

      options += len + 1;
    }

  /* To avoid repeated parsing of these options, we remove them here.  */
  info->disassembler_options = NULL;
}

void
print_pdp10_disassembler_options (FILE *stream)
{
  unsigned int i, max_len;

  fprintf (stream, _("\n\
The following PDP-10 specific disassembler options are supported for use\n\
with the -M switch (multiple options should be separated by commas):\n"));

  max_len = 0;
  for (i = 0; i < ARRAY_SIZE (pdp10_options); ++i)
    {
      unsigned int len = strlen (pdp10_options[i].name);
      if (len > max_len)
	max_len = len;
    }

  for (i = 0; i < ARRAY_SIZE (pdp10_options); ++i)
    fprintf (stream,  "  %s%*c  %s\n",
	     pdp10_options[i].name,
	     (int)(max_len - strlen (pdp10_options[i].name)), ' ',
	     _(pdp10_options[i].description));
}

void
disassemble_init_pdp10 (struct disassemble_info *info)
{
  struct pdp10_private_data *priv;

  if (info->private_data)
    return;

  info->bytes_per_line = 4;
  info->octets_per_byte = 2;

  priv = malloc (sizeof *priv);
  memset (priv, -1, sizeof *priv);
  priv->isa_level = 0;
  info->private_data = priv;

  parse_pdp10_disassembler_options (info);

  /* TODO: maybe use low 16 bits of info->flags */
  /* TODO: maybe use info->private_data for a quick lookup table */
}

static int
pdp10_read_word (struct disassemble_info *info, bfd_vma memaddr, uint64_t *dst)
{
  unsigned char octets[8];
  int status;
  uint64_t word;

  assert ((memaddr & 7) == 0);

  status = info->read_memory_func (memaddr, octets, 8, info);
  if (status != 0)
    {
      info->memory_error_func (status, memaddr, info);
      return status;
    }

  word = 0;
  for (int i = 0; i < 4; ++i)
    {
      unsigned char hi = octets[2 * i + 0];
      unsigned char lo = octets[2 * i + 1];
      assert ((hi >> 1) == 0);
      word = (word << 9) + ((hi & 1) << 8) + lo;;
    }

  *dst = word;
  return 0;
}

int
print_insn_pdp10 (bfd_vma memaddr, struct disassemble_info *info)
{
  uint64_t word;

  parse_pdp10_disassembler_options (info);

  if (pdp10_read_word (info, memaddr, &word))
    return 0;

  /* TODO: use the opcode table here */
  info->fprintf_func (info->stream, ".long\t%0*" PRIo64, 12, word);
  return 8;
}
