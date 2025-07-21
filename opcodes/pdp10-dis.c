/* Disassembly support for PDP-10.
   Copyright (C) 2025 Free Software Foundation, Inc.
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
  pdp10_cpu_models_t models;

  /* Maps high 13 bits of instruction words to matching entry in pdp10_insns[].  */
  int high13_to_index[1 << 13];
};

/* For now the only options we recognize are the names of the CPU families.  */

static void
parse_pdp10_disassembler_option (struct pdp10_private_data *priv, const char *option, int len)
{
  pdp10_cpu_models_t models;

  models = pdp10_cpu_models_from_name (option, len);
  if (models)
    {
      priv->models = models;
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
      int len;

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
  fprintf (stream, _("\n\
The following PDP-10 specific disassembler options are supported for use\n\
with the -M switch (multiple options should be separated by commas):\n"));

  for (int i = 0; i < pdp10_num_cpu_models; ++i)
    fprintf (stream,  "  %s\n", pdp10_cpu_models[i].name);
}

void
disassemble_init_pdp10 (struct disassemble_info *info)
{
  struct pdp10_private_data *priv;

  if (info->private_data)
    return;

  info->bytes_per_line = 8;
  info->octets_per_byte = 1;

  priv = malloc (sizeof *priv);
  priv->models = PDP10_KL10_271up;
  memset (priv->high13_to_index, -1, sizeof (priv->high13_to_index));
  info->private_data = priv;

  parse_pdp10_disassembler_options (info);

  /* Populate priv->high13_to_index.  Uses selected CPU models.  */

  for (int i = 0; i < pdp10_num_insns; ++i)
    {
      int h13lo, h13hi, h13incr;

      /* Skip insns invalid for selected CPU models.  */
      if ((pdp10_insns[i].models & priv->models) == 0)
	continue;

      /* When disassembling we don't know if we're looking at the second word
	 of an extended insn or not.  Until we do, skip those insns.  */
      if (pdp10_insns[i].format & PDP10_FMT_EXTENDED)
	continue;

      /* Iterate over all high13 bit patterns for the insn.  */

      h13lo = pdp10_insns[i].high13;
      h13incr = 1;
      switch (pdp10_insns[i].format & PDP10_FMT_MASK)
	{
	case PDP10_FMT_BASIC:
	  /* vary AC in low 4 bits */
	  h13hi = h13lo + 16;	/* all ACs */
	  break;
	case PDP10_FMT_A_NONZERO:
	  /* vary AC in low 4 bits */
	  h13lo += 1;
	  h13hi = h13lo + 15;	/* all ACs except AC0 */
	  break;
	case PDP10_FMT_A_OPCODE:
	  h13hi = h13lo + 1;	/* no AC, just one pattern */
	  break;
	case PDP10_FMT_IO:
	  /* vary DEV in middle 7 bits */
	  h13hi = h13lo + (128 << 3);
	  h13incr = 1 << 3;
	  break;
	}
      for (int h13 = h13lo; h13 < h13hi; h13 += h13incr)
	{
	  int previ = priv->high13_to_index[h13];

	  /* If this is the first candidate for this pattern, take it.  */
	  if (previ == -1)
	    {
	      priv->high13_to_index[h13] = i;
	      continue;
	    }

	  /* If this is an alias keep initial entry.  */
	  if (pdp10_insns[previ].format == pdp10_insns[i].format)
	    continue;

	  /* If one is A_OPCODE and the other is BASIC or IO, keep the A_OPCODE one.  */
	  if ((pdp10_insns[previ].format & PDP10_FMT_MASK) == PDP10_FMT_A_OPCODE
	      && ((pdp10_insns[i].format & PDP10_FMT_MASK) == PDP10_FMT_BASIC
		  || (pdp10_insns[i].format & PDP10_FMT_MASK) == PDP10_FMT_IO))
	    continue;
	  if (((pdp10_insns[previ].format & PDP10_FMT_MASK) == PDP10_FMT_BASIC
	       || (pdp10_insns[previ].format & PDP10_FMT_MASK) == PDP10_FMT_IO)
	      && (pdp10_insns[i].format & PDP10_FMT_MASK) == PDP10_FMT_A_OPCODE)
	    {
	      priv->high13_to_index[h13] = i;
	      continue;
	    }

	  /* If one is A_NONZERO and the other is A_UNUSED, keep the A_NONZERO one.  */
	  if ((pdp10_insns[previ].format & PDP10_FMT_MASK) == PDP10_FMT_A_NONZERO
	      && (pdp10_insns[i].format & PDP10_FMT_A_UNUSED) != 0)
	    continue;
	  if ((pdp10_insns[previ].format & PDP10_FMT_A_UNUSED) != 0
	      && (pdp10_insns[i].format & PDP10_FMT_MASK) == PDP10_FMT_A_NONZERO)
	    {
	      priv->high13_to_index[h13] = i;
	      continue;
	    }

	  /* If one is IO and the other is not, keep the non-IO one.  */
	  if ((pdp10_insns[previ].format & PDP10_FMT_MASK) != PDP10_FMT_IO
	      && (pdp10_insns[i].format & PDP10_FMT_MASK) == PDP10_FMT_IO)
	    continue;
	  if ((pdp10_insns[previ].format & PDP10_FMT_MASK) == PDP10_FMT_IO
	      && (pdp10_insns[i].format & PDP10_FMT_MASK) != PDP10_FMT_IO)
	    {
	      priv->high13_to_index[h13] = i;
	      continue;
	    }

	  /* Warn about the ambiguity.  */
	  fprintf (stderr, "Opcode %05o is ambiguous for models %#x: %s vs %s\n",
		   h13 << 2, priv->models, pdp10_insns[previ].name, pdp10_insns[i].name);
	}
    }
}

/* Read a 1, 2, 4, or 8-octet big-endian unsigned integer.  */
static int
pdp10_read_uint (bfd_vma memaddr, uint64_t *res, unsigned int log2n, struct disassemble_info *info)
{
  unsigned char octets[8];
  unsigned int n;
  int status;
  uint64_t word;

  assert (log2n <= 3);
  n = 1 << log2n;

  status = info->read_memory_func (memaddr, octets, n, info);
  if (status != 0)
    {
      info->memory_error_func (status, memaddr, info);
      return status;
    }

  word = 0;
  for (unsigned int i = 0; i < n; ++i)
    word = (word << 8) | octets[i];

  *res = word;
  return 0;
}

/* Decode a 36-bit word that has been encoded to 64-bits.  */
static int
pdp10_decode_word (uint64_t *word)
{
  uint16_t u9[4];
  uint64_t encoded, decoded;
  int i;

  encoded = *word;
  for (i = 0; i < 4; ++i)
    {
      if (encoded & 0xfe00)
	return 1;
      u9[i] = encoded & 0x01ff;
      encoded >>= 16;
    }

  decoded = 0;
  for (i = 3; i >= 0; --i)
    decoded = (decoded << 9) | u9[i];

  *word = decoded;
  return 0;
}

int
print_insn_pdp10 (bfd_vma memaddr, struct disassemble_info *info)
{
  const struct pdp10_private_data *priv;
  uint64_t word;
  unsigned int high13;
  int i;
  unsigned int ac, x, dev;

  parse_pdp10_disassembler_options (info);
  priv = (struct pdp10_private_data *) info->private_data;

  /* Handle test cases that disassemble non-code.  */
  if (memaddr & 1)
    {
      if (pdp10_read_uint (memaddr, &word, 0, info))
	return 0;
      info->fprintf_func (info->stream, ".byte\t0x%02x ; raw", (unsigned char) word);
      return 1;
    }
  else if (memaddr & 2)
    {
      if (pdp10_read_uint (memaddr, &word, 1, info))
	return 0;
      info->fprintf_func (info->stream, ".short\t0x%04x ; raw", (uint16_t) word);
      return 2;
    }
  else if (memaddr & 4)
    {
      if (pdp10_read_uint (memaddr, &word, 2, info))
	return 0;
      info->fprintf_func (info->stream, ".long\t0x%08" PRIx32 " ; raw", (uint32_t) word);
      return 4;
    }

  if (pdp10_read_uint (memaddr, &word, 3, info))
    return 0;
  if (pdp10_decode_word (&word))
    {
      info->fprintf_func (info->stream, ".quad\t0x%016" PRIx64 " ; raw", word);
      return 8;
    }

  high13 = word >> (36 - 13);

  i = priv->high13_to_index[high13];

  if (i == -1)
    {
      info->fprintf_func (info->stream, ".quad\t%012" PRIo64 " ; invalid", word);
      return 8;
    }

  info->fprintf_func (info->stream, "%s ", pdp10_insns[i].name);

  ac = (word >> 23) & 0x0F;
  switch (pdp10_insns[i].format & PDP10_FMT_MASK)
    {
    case PDP10_FMT_BASIC:
      if (ac == 0 && (pdp10_insns[i].format & PDP10_FMT_A_UNUSED))
	break;
      /*FALLTHROUGH*/
    case PDP10_FMT_A_NONZERO:
      info->fprintf_func (info->stream, "%o,", ac);
      break;
    case PDP10_FMT_A_OPCODE:
      break;
    case PDP10_FMT_IO:
      dev = ((word >> 26) & 127) << 2;
      info->fprintf_func (info->stream, "%o,", dev);
      break;
    }

  if ((word & ((1UL << 23) - 1)) == 0
      && (pdp10_insns[i].format & PDP10_FMT_E_UNUSED))
    return 8;

  if (word & (1UL << 22))
    info->fprintf_func (info->stream, "@");

  info->fprintf_func (info->stream, "%lo", word & ((1UL << 18) - 1));

  x = (word >> 18) & 0x0F;
  if (x)
    info->fprintf_func (info->stream, "(%o)", x);

  return 8;
}
