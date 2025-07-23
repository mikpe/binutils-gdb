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
pdp10_md_atof (int type ATTRIBUTE_UNUSED, char *litP ATTRIBUTE_UNUSED, int *sizeP ATTRIBUTE_UNUSED)
{
  return _("Unrecognized or unsupported floating point constant");
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

/* This is a variant of number_to_chars_bigendian that expands
   each nonet to two octets.  This must be used for emission of
   all code and runtime data.  */

void
pdp10_number_to_chars_progbits (char *buf, valueT val, int n)
{
  if (n <= 0 || (n & 1) != 0)
    abort ();
  do {
    buf[n - 1] = val & 0xff;
    buf[n - 2] = (val >> 8) & 1;
    n -= 2;
    val >>= 9;
  } while (n > 0);
  if (val)
    abort ();
}

const pseudo_typeS pdp10_md_pseudo_table[] = {
  /* Override data emitting directives to handle us allocating
     two octets per nonet.  */
  /* For strings we accept octet strings and emit them as sequences
     of nonets, two octets per nonet.  For non-octet strings we'd
     need to reimplement stringer to DTRT.  */
  /* TODO: Check that values don't overflow 36/18/9 bits, as appropriate.  */
  /* TODO: Drop md_number_to_chars_progbits, reimplement cons properly.  */
  { "ascii", stringer, (8*2)+0 },
  { "asciz", stringer, (8*2)+1 },
  { "byte", cons, 1*2 },
  { "dc", cons, 2*2 },
  /* dc.a gets the correct nbytes from TC_ADDRESS_BYTES.  */
  { "dc.b", cons, 1*2} ,
  { "dc.l", cons, 4*2 },
  { "dc.w", cons, 2*2 },
  { "hword", cons, 2*2 },
  { "int", cons, 4*2 },
  { "long", cons, 4*2 },
  { "octa", cons, 16*2 },
  { "quad", cons, 8*2 },
  { "short", cons, 2*2 },
  { "string", stringer, (8*2)+1 },
  { "word", cons, 2*2 },
  { "2byte", cons, 2*2 },
  { "4byte", cons, 4*2 },
  { "8byte", cons, 8*2 },
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

static void
pdp10_emit_word (unsigned char *to, uint64_t word)
{
  pdp10_number_to_chars_progbits ((char *) to, word, 8);
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
      expressionS opndx;
      char *opstr = str;

      memset (&opndx, 0, sizeof opndx);
      str = pdp10_expression (str, &opndx);
      if (!str)
	return;
      if (opndx.X_op != O_constant)
	{
	  as_bad (_("Expected (<AC>), got '%s'"), opstr);
	  return;
	}
      if ((uint64_t) opndx.X_add_number > 15)
	{
	  as_bad (_("AC out of range, got %lu"), (uint64_t) opndx.X_add_number);
	  return;
	}
      word |= (uint64_t) (opndx.X_add_number & 15) << 18;
    }
  /* Otherwise, if previous operand looks like (X), then classify it so.  */
  else if (opnd.X_op == O_constant
	   && lparen
	   && (uint64_t) opnd.X_add_number < 16)
    {
      word |= (uint64_t) (opnd.X_add_number & 15) << 18;

      memset (&opnd, 0, sizeof opnd);
      opnd.X_op = O_absent;
      lparen = false;
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
      bfd_reloc_code_real_type r_type = BFD_RELOC_32;
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

  (void) where; /* FIXME: not yet used */

  if (fixP->fx_addsy == NULL)
    fixP->fx_done = 1;

  /* We don't actually support subtracting a symbol.  */
  if (fixP->fx_subsy != NULL)
    as_bad_subtract (fixP);

  switch (fixP->fx_r_type)
    {
    /* FIXME: incomplete */
    default:
      as_bad_where (fixP->fx_file, fixP->fx_line,
		    _("Not yet implemented %s relocation fx_addsy %p fx_subsy %p fx_pcrel %d value %ld"),
		    bfd_get_reloc_code_name (fixP->fx_r_type), fixP->fx_addsy, fixP->fx_subsy, fixP->fx_pcrel, value);
    }
}
