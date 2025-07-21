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

/* Floating-point numerals.  */

const char pdp10_EXP_CHARS[] = "eE";
const char pdp10_FLT_CHARS[] = ""; /* FIXME: the PDP-10 will need its own format.  */

void
pdp10_md_operand (expressionS *exp ATTRIBUTE_UNUSED)
{
}

const char *
pdp10_md_atof (int type, char *litP, int *sizeP)
{
  /* FIXME: we will need our own FP parser */
  return ieee_md_atof (type, litP, sizeP, target_big_endian);
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
  { "pdp10_ascii", stringer,         16+0 },
  { "pdp10_bptr",  pdp10_bptr_cons,  8 },
  { "pdp10_byte",  pdp10_byte_cons,  2 },
  { "pdp10_hptr",  pdp10_hptr_cons,  8 },
  { "pdp10_hword", pdp10_hword_cons, 4 },
  { "pdp10_word",  pdp10_word_cons,  8 },
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
	      /* DEV, 7 middle bits of high 13 bits */
	      if ((uint64_t) opnd.X_add_number > 127)
		{
		  as_bad (_("DEV out of range, got %lu"), (uint64_t) opnd.X_add_number);
		  return;
		}
	      word |= (uint64_t) (opnd.X_add_number & 127) << (36 - 10);
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
      if (fixP->fx_addsy != NULL)	/* an 18-bit same-segment word address */
	{
	  if (value & 7)
	    as_bad_where (fixP->fx_file, fixP->fx_line,
			  _("Value %lu is not word-aligned."), value);
	  if (value < -((1L << (18 + 3 - 1)) - 1) || value >= (1L << (18 + 3)))
	    as_bad_where (fixP->fx_file, fixP->fx_line,
			  _("Value %ld out of 21-bit range."), value);
	  value = (unsigned long) value >> 3;
	  pdp10_emit_hword (where, value);
	  break;
	}
      else				/* an 18-bit literal */
	{
	  fixP->fx_r_type = BFD_RELOC_PDP10_18;
	}
      /*FALLTHROUGH*/

    case BFD_RELOC_PDP10_18:
      if (value < -((1L << (18 - 1)) - 1) || value >= (1L << 18))
	as_bad_where (fixP->fx_file, fixP->fx_line,
		      _("Value %ld out of 18-bit range."), value);
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
