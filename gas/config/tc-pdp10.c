/* tc-pdp10.c -- Assembler for the PDP-10.
   Copyright (C) 2025 Free Software Foundation, Inc.
   Contributed by Mikael Pettersson <mikpelinux@gmail.com>.

   This file is part of GAS.

   GAS is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the license, or
   (at your option) any later version.

   GAS is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING3.  If not,
   see <http://www.gnu.org/licenses/>.  */

#include "as.h"
#include "opcode/pdp10.h"
#include "dwarf2dbg.h"

/* Comment chars.  */

const char pdp10_comment_chars[] = ";#";
const char pdp10_line_comment_chars[] = ";#";
const char pdp10_line_separator_chars[] = "";

/* Command-line processing.  */

const char pdp10_md_shortopts[] = "";

const struct option pdp10_md_longopts[] = {
};

const size_t pdp10_md_longopts_size = sizeof pdp10_md_longopts;

int
pdp10_md_parse_option (int c ATTRIBUTE_UNUSED, const char *arg ATTRIBUTE_UNUSED)
{
  return 0;
}

void
pdp10_md_show_usage (FILE *fp ATTRIBUTE_UNUSED)
{
}

/* Currently selected CPU models.  */
static pdp10_cpu_models_t selected_cpu_models = PDP10_KL10_271up;

/* Map from insn name to array of pointers to insns with that name.
   Use selected CPU models to disambiguate.  */
static htab_t insn_hash = NULL;

void
pdp10_md_begin (void)
{
  insn_hash = str_htab_create ();

  for (int i = 0; i < pdp10_num_insns; ++i)
    {
      const struct pdp10_insn **arr;

      arr = str_hash_find (insn_hash, pdp10_insns[i].name);
      if (arr)
	{
	  size_t n = 0;

	  while (arr[n])
	    ++n;

	  arr = xrealloc (arr, (n + 2) * sizeof (struct pdp10_insn *));
	  arr[n + 0] = &pdp10_insns[i];
	  arr[n + 1] = NULL;
	}
      else
	{
	  arr = xmalloc (2 * sizeof (struct pdp10_insns *));
	  arr[0] = &pdp10_insns[i];
	  arr[1] = NULL;
	}
      str_hash_insert (insn_hash, pdp10_insns[i].name, arr, 1);
    }
}

static const struct pdp10_insn *
pdp10_lookup_insn_1 (const char *str)
{
  const struct pdp10_insn **arr, *insn;

  arr = str_hash_find (insn_hash, str);

  if (arr == NULL)
    {
      as_bad (_("Unknown instruction '%s'"), str);
      return NULL;
    }

  insn = NULL;
  for (; *arr != NULL; ++arr)
    if (((*arr)->models & selected_cpu_models) != 0)
      {
	if (insn != NULL)
	  {
	    as_bad (_("Ambiguous instruction '%s'"), str);
	    return NULL;
	  }
	insn = *arr;
      }

  if (insn == NULL)
    as_bad (_("Unavailable instruction '%s'"), str);
  return insn;
}

static const struct pdp10_insn *
pdp10_lookup_insn (char *op_begin, char *op_end)
{
  const struct pdp10_insn *insn;
  char c;

  c = *op_end;
  *op_end = '\0';
  insn = pdp10_lookup_insn_1 (op_begin);
  *op_end = c;
  return insn;
}

void
pdp10_md_operand (expressionS *exp ATTRIBUTE_UNUSED)
{
}

int
pdp10_md_estimate_size_before_relax (fragS *fragP ATTRIBUTE_UNUSED, segT segment ATTRIBUTE_UNUSED)
{
  return 0;
}

void
pdp10_md_convert_frag (bfd *abfd ATTRIBUTE_UNUSED, segT sec ATTRIBUTE_UNUSED, fragS *fragP ATTRIBUTE_UNUSED)
{
}

/* Translate internal representation of relocation info to BFD target
   format.  */

arelent *
pdp10_tc_gen_reloc (asection *section ATTRIBUTE_UNUSED, fixS *fixp)
{
  bfd_reloc_code_real_type code;
  reloc_howto_type *howto;
  arelent *reloc;

  code = fixp->fx_r_type;
  howto = bfd_reloc_type_lookup (stdoutput, code);
  if (howto == NULL)
    {
      as_bad_where (fixp->fx_file, fixp->fx_line,
		    _("cannot represent `%s' relocation in object file"),
		    bfd_get_reloc_code_name (code));
      return NULL;
    }

  reloc = notes_alloc (sizeof (arelent));
  reloc->sym_ptr_ptr = notes_alloc (sizeof (asymbol *));
  *reloc->sym_ptr_ptr = symbol_get_bfdsym (fixp->fx_addsy);
  reloc->address = fixp->fx_frag->fr_address + fixp->fx_where;
  reloc->addend = fixp->fx_offset;
  reloc->howto = howto;

  return reloc;
}

/* The location from which a PC relative jump should be calculated,
   given a PC relative jump reloc.  */
long
pdp10_md_pcrel_from_section (fixS *fixp, segT sec)
{
  if (fixp->fx_addsy != NULL
      && (!S_IS_DEFINED (fixp->fx_addsy)
	  || S_GET_SEGMENT (fixp->fx_addsy) != sec))
    {
      /* The symbol is undefined (or is defined but not in this section).
	 Let the linker figure it out.  */
      return 0;
    }

  /* Return the address of the instruction.  */
  return fixp->fx_where + fixp->fx_frag->fr_address;
}

/* Round up a section size to the appropriate boundary.  */

valueT
pdp10_md_section_align (segT segment ATTRIBUTE_UNUSED, valueT size)
{
  return size;
}

static bfd_reloc_code_real_type pdp10_cons_reloc = BFD_RELOC_NONE;

static void
pdp10_bptr_cons (int nbytes)
{
  pdp10_cons_reloc = BFD_RELOC_PDP10_BPTR;
  cons (nbytes);
  pdp10_cons_reloc = BFD_RELOC_NONE;
}

static void
pdp10_byte_cons (int nbytes)
{
  pdp10_cons_reloc = BFD_RELOC_PDP10_9;
  cons (nbytes);
  pdp10_cons_reloc = BFD_RELOC_NONE;
}

static void
pdp10_hptr_cons (int nbytes)
{
  pdp10_cons_reloc = BFD_RELOC_PDP10_HPTR;
  cons (nbytes);
  pdp10_cons_reloc = BFD_RELOC_NONE;
}

static void
pdp10_hword_cons (int nbytes)
{
  pdp10_cons_reloc = BFD_RELOC_PDP10_18;
  cons (nbytes);
  pdp10_cons_reloc = BFD_RELOC_NONE;
}

static void
pdp10_word_cons (int nbytes)
{
  pdp10_cons_reloc = BFD_RELOC_PDP10_36;
  cons (nbytes);
  pdp10_cons_reloc = BFD_RELOC_NONE;
}

bfd_reloc_code_real_type
pdp10_tc_parse_cons_expression (expressionS *exp, int nbytes ATTRIBUTE_UNUSED)
{
  expression (exp);
  return pdp10_cons_reloc;
}

/* We cannot override generic data emitting directives without breaking much
   of the test suite.  Therefore we accept them for data that only appears
   in generic test cases or as meta-data.  For loadable PROGBITS we require
   using PDP-10 specific directives that do the PDP-10 data reformatting.  */
const pseudo_typeS pdp10_md_pseudo_table[] = {
  { "pdp10_ascii",  stringer,         16+0 },
  { "pdp10_bptr",   pdp10_bptr_cons,  8 },
  { "pdp10_byte",   pdp10_byte_cons,  2 },
  { "pdp10_dfloat", float_cons,       'd' | 128 },
  { "pdp10_ffloat", float_cons,       'f' | 128 },
  { "pdp10_gfloat", float_cons,       'g' | 128 },
  { "pdp10_hptr",   pdp10_hptr_cons,  8 },
  { "pdp10_hword",  pdp10_hword_cons, 4 },
  { "pdp10_word",   pdp10_word_cons,  8 },
  { NULL, NULL, 0 }
};

static char *
skip_blanks (char *str)
{
  char c;

  while (c = *str, c == ' ' || c == '\t')
    ++str;

  return str;
}

static char *
skip_non_blanks (char *str)
{
  char c;

  while (c = *str, !(c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\0'))
    ++str;

  return str;
}

static char *
pdp10_expression (char *str, expressionS *exp)
{
  char *save_ilp;

  /* TODO: handle %<reloc>(...) */

  save_ilp = input_line_pointer;
  input_line_pointer = str;
  expression (exp);

  if (exp->X_op == O_illegal || exp->X_op == O_absent)
    {
      as_bad (_("Expected expression, got '%s'"), str);
      return NULL;
    }

  str = input_line_pointer;
  input_line_pointer = save_ilp;

  return str;
}

/* This is a variant of number_to_chars_bigendian that expands
   each nonet to two octets.  This must be used for emission of
   all code and runtime data.  */
static void
pdp10_emit_uint (unsigned char *buf, uint64_t val, unsigned int nroctets)
{
  if (nroctets & 1)
    abort ();
  do {
    buf[nroctets - 1] = val & 0xff;
    buf[nroctets - 2] = (val >> 8) & 1;
    nroctets -= 2;
    val >>= 9;
  } while (nroctets > 0);
  if (val)
    abort ();
}

static void
pdp10_emit_word (unsigned char *to, uint64_t word)
{
  pdp10_emit_uint (to, word & ((1UL << 36) - 1), 8);
}

static void
pdp10_emit_hword (unsigned char *to, uint32_t hword)
{
  pdp10_emit_uint (to, hword & ((1UL << 18) - 1), 4);
}

static void
pdp10_emit_nonet (unsigned char *to, uint64_t word)
{
  pdp10_emit_uint (to, word & ((1 << 9) - 1), 2);
}

/* Assemble a source line.  */

void
pdp10_md_assemble (char *str)
{
  expressionS opnd;
  bool lparen;
  char *op_begin;
  const struct pdp10_insn *insn;
  uint64_t word;
  unsigned char *frag_ptr;

  memset (&opnd, 0, sizeof opnd);
  opnd.X_op = O_absent;
  lparen = false;

  op_begin = str = skip_blanks (str);
  str = skip_non_blanks (str);
  insn = pdp10_lookup_insn (op_begin, str);
  if (!insn)
    return;
  /* TODO: handle extended instructions */
  if (insn->format & PDP10_FMT_EXTENDED)
    {
      as_bad (_("Extended instructions are not yet supported"));
      return;
    }

  word = (uint64_t) insn->high13 << (36 - 13);

  /* Do we have an operand?  We don't yet know if it's
     the AC/DEV, Y, or X field.  */
  str = skip_blanks (str);
  if (*str && *str != '@')
    {
      char *opstr = str;

      lparen = *str == '(';
      str = pdp10_expression (str, &opnd);
      if (!str)
	return;

      /* Was it the AC/DEV field?  */
      str = skip_blanks (str);
      if (*str == ',')
	{
	  ++str;
	  if (opnd.X_op != O_constant)
	    {
	      as_bad (_("Expected constant, got '%s'"), opstr);
	      return;
	    }
	  switch (insn->format & PDP10_FMT_MASK)
	    {
	    case PDP10_FMT_A_OPCODE:
	      as_bad (_("Instruction cannot have an AC/DEV operand"));
	      return;
	    case PDP10_FMT_IO:
	      /* DEV, 7 middle bits of high 13 bits.  Device codes are consistently
		 listed as 9-bit octal numbers with two excess low bits.  */
	      if ((opnd.X_add_number & 0774) != opnd.X_add_number)
		{
		  as_bad (_("DEV out of range, got %lu"), (uint64_t) opnd.X_add_number);
		  return;
		}
	      word |= (uint64_t) ((opnd.X_add_number >> 2) & 127) << (36 - 10);
	      break;
	    default:
	      /* AC, 4 low bits of high 13 bits */
	      if ((uint64_t) opnd.X_add_number > 15)
		{
		  as_bad (_("AC out of range, got %lu"), (uint64_t) opnd.X_add_number);
		  return;
		}
	      word |= (uint64_t) (opnd.X_add_number & 15) << (36 - 13);
	      break;
	    }
	  memset (&opnd, 0, sizeof opnd);
	  opnd.X_op = O_absent;
	  lparen = false;
	}
    }

  if (opnd.X_op == O_absent)
    {
      /* Should the indirect bit be set?  */
      str = skip_blanks (str);
      if (*str == '@')
	{
	  ++str;
	  word |= (uint64_t) 1 << (36 - 14);
	}

      /* Do we have a Y or X operand?  */
      str = skip_blanks (str);
      if (*str)
	{
	  lparen = *str == '(';
	  str = pdp10_expression (str, &opnd);
	  if (!str)
	    return;
	}
    }

  /* Do we have an (X) operand?  If so, previous operand, if any, was Y.  */
  str = skip_blanks (str);
  if (*str == '(')
    {
      expressionS xopnd;
      char *opstr = str;

      memset (&xopnd, 0, sizeof xopnd);
      str = pdp10_expression (str, &xopnd);
      if (!str)
	return;
      if (xopnd.X_op != O_constant)
	{
	  as_bad (_("Expected (<AC>), got '%s'"), opstr);
	  return;
	}
      if ((uint64_t) xopnd.X_add_number > 15)
	{
	  as_bad (_("AC out of range, got %lu"), (uint64_t) xopnd.X_add_number);
	  return;
	}
      word |= (uint64_t) (xopnd.X_add_number & 15) << 18;
    }
  /* Otherwise, if previous operand looks like (X), then classify it so.  */
  else if (opnd.X_op == O_constant
	   && lparen
	   && (uint64_t) opnd.X_add_number < 16)
    {
      word |= (uint64_t) (opnd.X_add_number & 15) << 18;
      memset (&opnd, 0, sizeof opnd);
      opnd.X_op = O_absent;
    }

  str = skip_blanks (str);
  if (*str)
    {
      as_bad (_("Junk at end of line: '%s'"), str);
      return;
    }

  dwarf2_emit_insn (0);
  frag_ptr = (unsigned char *) frag_more (8);

  pdp10_emit_word (frag_ptr, word);

  if (opnd.X_op != O_absent)
    {
      bfd_reloc_code_real_type r_type = BFD_RELOC_PDP10_IFIW;
      fix_new_exp (frag_now, (frag_ptr + 4) - (unsigned char *) frag_now->fr_literal,
		   4, &opnd, false, r_type);
    }
}

symbolS *
pdp10_md_undefined_symbol (char *name ATTRIBUTE_UNUSED)
{
  return NULL;
}

void
pdp10_md_apply_fix (fixS *fixP, valueT *valP, segT seg ATTRIBUTE_UNUSED)
{
  unsigned char *where = (unsigned char *) fixP->fx_frag->fr_literal + fixP->fx_where;
  long value = *valP;

  if (fixP->fx_addsy == NULL)
    fixP->fx_done = 1;

  /* We don't actually support subtracting a symbol.  */
  if (fixP->fx_subsy != NULL)
    as_bad_subtract (fixP);

  switch (fixP->fx_r_type)
    {
    case BFD_RELOC_64:
      where[0] = ((unsigned long) value >> 56) & 0xff;
      where[1] = ((unsigned long) value >> 48) & 0xff;
      where[2] = ((unsigned long) value >> 40) & 0xff;
      where[3] = ((unsigned long) value >> 32) & 0xff;
      where[4] = ((unsigned long) value >> 24) & 0xff;
      where[5] = ((unsigned long) value >> 16) & 0xff;
      where[6] = ((unsigned long) value >>  8) & 0xff;
      where[7] = value & 0xff;
      break;

    case BFD_RELOC_32:
      if (value < -((1L << (32 - 1)) - 1) || value >= (1L << 32))
	as_bad_where (fixP->fx_file, fixP->fx_line,
		      _("Value %ld out of 32-bit range."), value);
      where[0] = ((unsigned long) value >> 24) & 0xff;
      where[1] = ((unsigned long) value >> 16) & 0xff;
      where[2] = ((unsigned long) value >>  8) & 0xff;
      where[3] = value & 0xff;
      break;

    case BFD_RELOC_16:
      if (value < -((1L << (16 - 1)) - 1) || value >= (1L << 16))
	as_bad_where (fixP->fx_file, fixP->fx_line,
		      _("Value %ld out of 16-bit range."), value);
      where[0] = ((unsigned long) value >> 8) & 0xff;
      where[1] = value & 0xff;
      break;

    case BFD_RELOC_8:
      /* Test cases fail if we range check the value here.  */
      where[0] = value & 0xff;
      break;

    case BFD_RELOC_PDP10_9:
      if (value < -((1L << (9 - 1)) - 1) || value >= (1L << 9))
	as_bad_where (fixP->fx_file, fixP->fx_line,
		      _("Value %ld out of 9-bit range."), value);
      pdp10_emit_nonet (where, value);
      break;

    case BFD_RELOC_PDP10_IFIW:
    case BFD_RELOC_PDP10_18:
      if (fixP->fx_addsy != NULL)	/* an 18-bit same-segment word address */
	{
	  if (value & 7)
	    as_bad_where (fixP->fx_file, fixP->fx_line,
			  _("Value %lu is not word-aligned."), value);
	  if (value < -((1L << (18 + 3 - 1)) - 1) || value >= (1L << (18 + 3)))
	    as_bad_where (fixP->fx_file, fixP->fx_line,
			  _("Value %ld out of 21-bit range."), value);
	  fixP->fx_r_type = BFD_RELOC_PDP10_IFIW;
	  value = (unsigned long) value >> 3;
	}
      else				/* an 18-bit literal */
	{
	  if (value < -((1L << (18 - 1)) - 1) || value >= (1L << 18))
	    as_bad_where (fixP->fx_file, fixP->fx_line,
			  _("Value %ld out of 18-bit range."), value);
	  fixP->fx_r_type = BFD_RELOC_PDP10_18;
	}
      pdp10_emit_hword (where, value);
      break;

    case BFD_RELOC_PDP10_36:
      if (fixP->fx_addsy != NULL)	/* a 30-bit word address */
	{
	  if (value & 7)
	    as_bad_where (fixP->fx_file, fixP->fx_line,
			  _("Value %lu is not word-aligned."), value);
	  if (value < -((1L << (30 + 3 - 1)) - 1) || value >= (1L << (30 + 3)))
	    as_bad_where (fixP->fx_file, fixP->fx_line,
			  _("Value %ld out of 33-bit range."), value);
	  fixP->fx_r_type = BFD_RELOC_PDP10_EFIW;
	  value = (unsigned long) value >> 3;
	}
      else				/* a 36-bit literal */
	{
	  if (value < -((1L << (36 - 1)) - 1) || value >= (1L << 36))
	    as_bad_where (fixP->fx_file, fixP->fx_line,
			  _("Value %ld out of 36-bit range."), value);
	}
      pdp10_emit_word (where, value);
      break;

    case BFD_RELOC_PDP10_BPTR:
      {
	uint64_t word;
	uint32_t y;
	unsigned int ps;

	if ((value & ~0x1fffffffeULL) != 0)
	  as_bad_where (fixP->fx_file, fixP->fx_line,
			_("Value %ld invalid for 9-bit byte pointer."), value);
	ps = 070 + ((value >> 1) & 3);
	y = (value >> 3) & ((1UL << 30) - 1);
	word = ((uint64_t) ps << 30) | y;
	pdp10_emit_word (where, word);
	break;
      }

    case BFD_RELOC_PDP10_HPTR:
      {
	uint64_t word;
	uint32_t y;
	unsigned int ps;

	if ((value & ~0x1fffffffcULL) != 0)
	  as_bad_where (fixP->fx_file, fixP->fx_line,
			_("Value %ld invalid for 18-bit byte pointer."), value);
	ps = 075 + ((value >> 2) & 1);
	y = (value >> 3) & ((1UL << 30) - 1);
	word = ((uint64_t) ps << 30) | y;
	pdp10_emit_word (where, word);
	break;
      }

    default:
      as_bad_where (fixP->fx_file, fixP->fx_line,
		    _("Not yet implemented %s relocation fx_addsy %p fx_subsy %p fx_pcrel %d value %ld"),
		    bfd_get_reloc_code_name (fixP->fx_r_type), fixP->fx_addsy, fixP->fx_subsy, fixP->fx_pcrel, value);
    }
}

/* Floating-point numerals.  */

/* DEC PDP-10 floating-point number formats:

   F-format (36 bits, all PDP-6/10):
	1-bit sign
	8-bit exponent (excess-128)
	27-bit fraction

	No implicit leading 1 on the fraction.
	A zero is all bits 0.
	A negative number has a 1 in the sign bit, ones complement of the exponent,
	and twos complement of the fraction. It is formed by negating the word.

	+153_10 = +231_8 = +0.462_8 x 2^8
	= 0 10001000 100 110 010 000 000 000 000 000 000
	= 010 001 000 100 110 010 000 000 000 000 000 000
	= 444C80000 (hex)

	-153_10 = -231_8 = -0.462_8 x 2^8
	= 1 01110111 011 001 110 000 000 000 000 000 000
	= 101 110 111 011 001 110 000 000 000 000 000 000
	= BBB380000 (hex)

   D-format (72 bits, KI10, KL10, KS10, XKL-1):
	1-bit sign
	8-bit exponent (excess-128)
	27-bit fraction (most significant bits)
	1-bit garbage
	35-bit continued fraction (least significant bits)

	First word exactly as in the F-format.
	Second word extends the fraction with 35 bits, high bit of second word is garbage.

	+(2^18 + 2^-18)
	= 262144.0000038147
	= 0 10010011 100 000 000 000 000 000 000 000 000
	  0 00000000 010 000 000 000 000 000 000 000 000
	= 010 010 011 100 000 000 000 000 000 000 000 000
	  000 000 000 010 000 000 000 000 000 000 000 000

	-(2^18 + 2^-18)
	= 1 01101100 011 111 111 111 111 111 111 111 111
	  0 11111111 110 000 000 000 000 000 000 000 000
	= 101 101 100 011 111 111 111 111 111 111 111 111
	  011 111 111 110 000 000 000 000 000 000 000 000

	Note: The above bit-patterns are from the 1982 ProcRef.  Both gas
	and gcc make the low 6 bits of the positive number 030, which
	has a large effect on the negative number.

   KA10 D-format (72 bits, KA10 only, not supported):
	1-bit sign
	8-bit exponent (excess-128)
	27-bit fraction (most significant bits)
	1-bit garbage
	8-bit exponent minus 27
	27-bit continued fraction (least significant bits)

	+(2^18 + 2^-18)
	= 0 10010011 100 000 000 000 000 000 000 000 000
	  0 01111000 000 000 000 100 000 000 000 000 000

	-(2^18 + 2^-18)
	= 1 01101100 011 111 111 111 111 111 111 111 111
	  0 01111000 111 111 111 100 000 000 000 000 000

   G-format (72 bits, KL10B, KS10, XKL-1):
	1-bit sign
	11-bit exponent (excess-1024)
	24-bit fraction (most significant bits)
	1-bit garbage
	35-bit continued fraction (least significant bits)

	Like D-format, but with 3-bit wider exponent and 3-bit narrower fraction.

	+(2^18 + 2^-18)
	= 0 10000010011 100 000 000 000 000 000 000 000
	  0 00000000000 010 000 000 000 000 000 000 000
	= 010 000 010 011 100 000 000 000 000 000 000 000
	  000 000 000 000 010 000 000 000 000 000 000 000

	-(2^18 + 2^-18)
	= 1 01111101100 011 111 111 111 111 111 111 111
	  0 11111111111 110 000 000 000 000 000 000 000
	= 101 111 101 100 011 111 111 111 111 111 111 111
	  011 111 111 111 110 000 000 000 000 000 000 000

	Note 1: This example is from the Toad-1 manual, it differ from the 1982
	ProcRef, but that one seems to have missed to adjust the exponents for
	the different excess value.

	Note 2: Gas makes the low 3 bits of the positive number 03, which
	has a large effect on the negative number.  */

#define BITS_TO_LITTLENUMS(N) (((N) + LITTLENUM_NUMBER_OF_BITS - 1) / LITTLENUM_NUMBER_OF_BITS)

/* Format precision and exponent sizes in bits.  */
#define F_PRECISION_BITS	36
#define F_EXPONENT_BITS		8
#define D_PRECISION_BITS	71
#define D_EXPONENT_BITS		8
#define G_PRECISION_BITS	71
#define G_EXPONENT_BITS		11
#define MAX_PRECISION_BITS	G_PRECISION_BITS

/* Max precision in LittleNums.  */
#define MAX_PRECISION		BITS_TO_LITTLENUMS(MAX_PRECISION_BITS)

/* Length in LittleNums of guard bits.  */
#define GUARD		2

static unsigned long
mask (unsigned int bits)
{
  return ((uint64_t) 1 << bits) - 1;
}

struct next_bits
{
  int bits_avail; /* bits available in *pointer */
  int words_avail;
  LITTLENUM_TYPE *pointer;
};

static int
next_bits (struct next_bits *n, int number_of_bits)
{
  int return_value;

  if (!n->words_avail)
    return 0;

  if (number_of_bits >= n->bits_avail)
    {
      return_value = mask (n->bits_avail) & n->pointer[0];
      number_of_bits -= n->bits_avail;
      return_value <<= number_of_bits;

      if (--(n->words_avail))
	{
	  n->bits_avail = LITTLENUM_NUMBER_OF_BITS - number_of_bits;
	  --(n->pointer);
	  return_value |= (n->pointer[0] >> n->bits_avail) & mask (number_of_bits);
	}
    }
  else
    {
      n->bits_avail -= number_of_bits;
      return_value = mask (number_of_bits) & (n->pointer[0] >> n->bits_avail);
    }

  return return_value;
}

/* The PDP-10 doesn't have NaNs, so instead we construct a non-normalized
   number with a zero fraction and non-zero exponent.  */
static void
make_invalid_floating_point_number (LITTLENUM_TYPE *words)
{
  memset (words, 0, sizeof (LITTLENUM_TYPE) * MAX_PRECISION);
  words[0] = 0x7F80;
}

static void
flonum_gen2dec (int precision_bits, int exponent_bits, FLONUM_TYPE *f, LITTLENUM_TYPE *words)
{
  struct next_bits nb;
  LITTLENUM_TYPE *wp;
  unsigned int exponent_skippage;
  unsigned int exponent;
  LITTLENUM_TYPE word1;

  if (f->low > f->leader)
    {
      /* 0.0e0 seen.  */
      memset (words, 0, sizeof (LITTLENUM_TYPE) * BITS_TO_LITTLENUMS (precision_bits));
      return;
    }

  if (f->sign != '-' && f->sign != '+')
    {
      /* NaN or Inf.  */
      make_invalid_floating_point_number (words);
      return;
    }

  nb.bits_avail = LITTLENUM_NUMBER_OF_BITS;
  nb.pointer = f->leader;
  nb.words_avail = 1 + f->leader - f->low;

  /* Seek past leading significant bit.  */
  exponent_skippage = 0;
  while (!next_bits(&nb, 1))
    ++exponent_skippage;

  exponent = f->exponent + f->leader + 1 - f->low;
  exponent = exponent * LITTLENUM_NUMBER_OF_BITS;
  exponent = exponent - exponent_skippage;
  exponent = exponent + (1 << (exponent_bits - 1));

  if (exponent & ~mask (exponent_bits))
    {
      /* Exponent overflow.  Lose immediately.  */
      make_invalid_floating_point_number (words);
      return;
    }

  wp = words;

  /* Little word 1.  Sign, exponent, high mantissa bits.  */

  word1 = (0x0000	/* if the number is negative the sign is set later */
	   | ((exponent & mask (exponent_bits)) << (15 - exponent_bits))
	   | (1 << (15 - 1 - exponent_bits))	/* non-hidden leading 1 */
	   | next_bits (&nb, 15 - 1 - exponent_bits));
  *wp++ = word1;

  /* The rest of the little words are just mantissa bits.  */
  precision_bits -= LITTLENUM_NUMBER_OF_BITS;
  while (precision_bits >= LITTLENUM_NUMBER_OF_BITS)
    {
      *wp++ = next_bits (&nb, LITTLENUM_NUMBER_OF_BITS);
      precision_bits -= LITTLENUM_NUMBER_OF_BITS;
    }
  if (precision_bits > 0)
    *wp++ = next_bits (&nb, precision_bits) << (LITTLENUM_NUMBER_OF_BITS - precision_bits);

  /* Rounding needed?  */
  if (next_bits (&nb, 1))
    {
      LITTLENUM_TYPE *wp_save = wp;
      LITTLENUM_TYPE carry;

      /* Add 1 to the LSB, propagate up to the exponent if necessary.  */
      carry = 1 << (LITTLENUM_NUMBER_OF_BITS - precision_bits);
      while (carry && wp > words)
	{
	  unsigned long w = *(--wp);	/* must be wider than LITTLENUM_TYPE */
	  w += carry;
	  *wp = w;
	  carry = w >> LITTLENUM_NUMBER_OF_BITS;
	}

      /* If the carry propagated all the way, and the sign flipped, we lose.  */
      if ((wp == words) && ((word1 ^ *words) & (1 << (LITTLENUM_NUMBER_OF_BITS - 1))))
	{
	  make_invalid_floating_point_number (words);
	  return;
	}

      wp = wp_save;
    }

  /* If negative, 2-complement the whole thing.  */
  if (f->sign == '-')
    {
      LITTLENUM_TYPE carry;

      carry = 1;
      while (wp > words)
	{
	  unsigned long w = *(--wp);	/* must be wider than LITTLENUM_TYPE */
	  w = (~w & LITTLENUM_MASK) + carry;
	  *wp = w;
	  carry = w >> LITTLENUM_NUMBER_OF_BITS;
	}
    }
}

static char *
atof_dec (char *str, int precision_bits, int exponent_bits, LITTLENUM_TYPE *words)
{
  FLONUM_TYPE f;
  LITTLENUM_TYPE bits[MAX_PRECISION + MAX_PRECISION + GUARD];
  char *return_value;

  return_value = str;

  f.low = bits + MAX_PRECISION;
  f.high = NULL;
  f.leader = NULL;
  f.exponent = 0;
  f.sign = '\0';

  memset (bits, 0, sizeof bits);

  f.high = f.low + BITS_TO_LITTLENUMS (precision_bits) - 1 + GUARD;

  if (atof_generic (&return_value, ".", "eE", &f))
    {
      make_invalid_floating_point_number (words);
      return NULL;
    }

  flonum_gen2dec (precision_bits, exponent_bits, &f, words);

  return return_value;
}

static const char *
dec_md_atof (int type, char *litP, int *sizeP)
{
  LITTLENUM_TYPE littlewords[MAX_PRECISION];
  uint64_t word36;
  int precision_bits, exponent_bits;

  switch (type)
    {
    case 'F':
    case 'f':
      precision_bits = F_PRECISION_BITS;
      exponent_bits = F_EXPONENT_BITS;
      break;

    case 'D':
    case 'd':
      precision_bits = D_PRECISION_BITS;
      exponent_bits = D_EXPONENT_BITS;
      break;

    case 'G':
    case 'g':
      precision_bits = G_PRECISION_BITS;
      exponent_bits = G_EXPONENT_BITS;
      break;

    default:
      *sizeP = 0;
      return _("Unrecognized or unsupported floating point constant");
    }

  memset (littlewords, 0, sizeof littlewords);

  input_line_pointer = atof_dec (input_line_pointer, precision_bits, exponent_bits, littlewords);

  /* Convert from left-justifed array of LITTLENUMs to one or two 36-bit words.  */

  /* First word: sign, exponent, initial part of mantissa.  */
  word36 = (((uint64_t) littlewords[0] << 20)
	    | (littlewords[1] << 4)
	    | (littlewords[2] >> 12));
  pdp10_emit_word ((unsigned char *) litP, word36);

  if (precision_bits == 36)
    *sizeP = 8;
  else
    {
      /* Second word: 0 in MSB, then remaining 35 bits of mantissa.  */
      word36 = ((((uint64_t) littlewords[2] & ((1 << 12) - 1)) << 23)
		| (littlewords[3] << 7)
		| (littlewords[4] >> 9));
      pdp10_emit_word ((unsigned char *) (litP + 8), word36);
      *sizeP = 16;
    }

  return NULL;
}

const char pdp10_EXP_CHARS[] = "eE";
const char pdp10_FLT_CHARS[] = "";

const char *
pdp10_md_atof (int type, char *litP, int *sizeP)
{
  if (type & 128)	/* .pdp10_[dfg]float directive? */
    return dec_md_atof (type & 0x7f, litP, sizeP);
  return ieee_md_atof (type, litP, sizeP, target_big_endian);
}
