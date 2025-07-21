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

/* Map from insn name to array of pointers to insns with that name.
   Use selected cpu models to disambiguate.  */
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
  {"ascii", stringer, (8*2)+0},
  {"asciz", stringer, (8*2)+1},
  {"byte", cons, 1*2},
  {"dc", cons, 2*2},
  /* dc.a gets the correct nbytes from TC_ADDRESS_BYTES.  */
  {"dc.b", cons, 1*2},
  {"dc.l", cons, 4*2},
  {"dc.w", cons, 2*2},
  {"hword", cons, 2*2},
  {"int", cons, 4*2},
  {"long", cons, 4*2},
  {"octa", cons, 16*2},
  {"quad", cons, 8*2},
  {"short", cons, 2*2},
  {"string", stringer, (8*2)+1},
  {"word", cons, 2*2},
  {"2byte", cons, 2*2},
  {"4byte", cons, 4*2},
  {"8byte", cons, 8*2},
  {NULL, 0, 0}
};

static void
pdp10_emit_word (unsigned char *to, uint64_t word)
{
  pdp10_number_to_chars_progbits ((char *) to, word, 8);
}

/* Assemble a source line.  */

void
pdp10_md_assemble (char *str)
{
  /* FIXME: this is utterly incomplete  */
  /* TODO: use opcodes instead  */
  if (strcmp (str, "nop") == 0)
    {
      /* The preferred NOP is a JFCL with zeros in the A, I, X, and Y fields.  */
      uint64_t word = 0255000000000ULL;
      unsigned char *to = (unsigned char *) frag_more (8);
      pdp10_emit_word (to, word);
      return;
    }
  as_bad (_("Not yet implemented: `%s'"), str);
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
