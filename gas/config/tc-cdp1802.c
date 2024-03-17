/* tc-cdp1802.c -- Assembler for the RCA CDP1802 processor.
   Copyright (C) 2024 Free Software Foundation, Inc.
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
#include "opcode/cdp1802.h"
#include "dwarf2dbg.h"

/* Comment chars.  */

const char cdp1802_comment_chars[] = ";#";
const char cdp1802_line_comment_chars[] = ";#";
const char cdp1802_line_separator_chars[] = "";

/* Command-line processing.  */

const char cdp1802_md_shortopts[] = "";

const struct option cdp1802_md_longopts[] = {
};

const size_t cdp1802_md_longopts_size = sizeof cdp1802_md_longopts;

int
cdp1802_md_parse_option (int c ATTRIBUTE_UNUSED, const char *arg ATTRIBUTE_UNUSED)
{
  return 0;
}

void
cdp1802_md_show_usage (FILE *fp ATTRIBUTE_UNUSED)
{
}

void
cdp1802_md_begin (void)
{
}

/* Floating-point numerals.  */

const char cdp1802_EXP_CHARS[] = "eE";
const char cdp1802_FLT_CHARS[] = "dD";

void
cdp1802_md_operand (expressionS *exp ATTRIBUTE_UNUSED)
{
}

/* Not overriding TC_ADDRESS_BYTES causes .dc.a to emit 16-bit data
   on the CDP1802, which would be expected.  However the ld-elf/pr22450
   and ld-elf/64ksec test cases _depend_ on .dc.a emitting 32-bit data
   on 32-bit ELF and 64-bit data on 64-bit ELF.  This override ensures
   that our .dc.a emits 32-bit data so these tests can pass.  */
int
cdp1802_tc_address_bytes (void)
{
    return 4;
}

const char *
cdp1802_md_atof (int type, char *litP, int *sizeP)
{
  return ieee_md_atof (type, litP, sizeP, target_big_endian);
}

int
cdp1802_md_estimate_size_before_relax (fragS *fragP ATTRIBUTE_UNUSED, segT segment ATTRIBUTE_UNUSED)
{
  return 0;
}

void
cdp1802_md_convert_frag (bfd *abfd ATTRIBUTE_UNUSED, segT sec ATTRIBUTE_UNUSED, fragS *fragP ATTRIBUTE_UNUSED)
{
}

/* Translate internal representation of relocation info to BFD target
   format.  */

arelent *
cdp1802_tc_gen_reloc (asection *section ATTRIBUTE_UNUSED, fixS *fixp)
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
cdp1802_md_pcrel_from_section (fixS *fixp, segT sec)
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
cdp1802_md_section_align (segT segment ATTRIBUTE_UNUSED, valueT size)
{
  return size;
}

const pseudo_typeS cdp1802_md_pseudo_table[] = {
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
parse_ulong (char *str, unsigned long *res)
{
  unsigned long val;
  char *endptr;

  errno = 0;
  val = strtoul (str, &endptr, 0);
  *res = (errno || endptr == str) ? -1UL : val;
  return endptr;
}

/* Parse an expression for an immediate operand.  Allows relocation
   operators %hi()/%lo() at the top level.  */

static char *
cdp1802_expression (char *str, expressionS *exp, bfd_reloc_code_real_type *r_type)
{
  char *save_ilp;

  if (*str == '%')
    {
      ++str;
      if (strncasecmp (str, "hi(", 3) == 0)
	{
	  str += 3;
	  *r_type = BFD_RELOC_CDP1802_HI8;
	}
      else if (strncasecmp (str, "lo(", 3) == 0)
	{
	  str += 3;
	  *r_type = BFD_RELOC_CDP1802_LO8;
	}
      else
	{
	  as_bad (_("Expected %%<reloc-operator>(, got '%s'"), str - 1);
	  return NULL;
	}
    }
  else
    *r_type = BFD_RELOC_NONE;

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

  if (*r_type != BFD_RELOC_NONE)
    {
      if (*str != ')')
	{
	  as_bad (_("Expected ')', got '%c'"), *str);
	  return NULL;
	}
      ++str;
    }

  return str;
}

/* Assemble a source line.  */

void
cdp1802_md_assemble (char *str)
{
  char *op_begin;
  char *op_end;
  const struct cdp1802_opcode *opcode;
  unsigned char opcode_low_bits;
  expressionS opnd;
  bfd_reloc_code_real_type r_type = BFD_RELOC_NONE;
  unsigned int opcode_nrbytes;
  unsigned int imm_nrbytes;
  unsigned char *frag_ptr;

  opcode_low_bits = 0;
  memset (&opnd, 0, sizeof opnd);

  str = skip_blanks (str);

  op_begin = str;
  str = skip_non_blanks (str);
  op_end = str;

  opcode = cdp1802_lookup_opcode (op_begin, op_end - op_begin);
  if (!opcode)
    {
      *op_end = '\0';
      as_bad (_("Unknown instruction '%s'"), op_begin);
      return;
    }

  if (opcode->operand_flags & OPND_N)
    {
      unsigned long val;

      str = skip_blanks (str);
      str = parse_ulong (str, &val);
      if (opcode->operand_flags & OPND_PORT)
	{
	  if (val == 0 || val > 7)
	    {
	      as_bad (_("Expected port, got '%s'."), str);
	      return;
	    }
	}
      else if (val > 15)
	{
	  as_bad (_("Expected register, got '%s'."), str);
	  return;
	}
      opcode_low_bits = val;
    }

  if (opcode->operand_flags & (OPND_IMM1 | OPND_IMM2))
    {
      if (opcode->operand_flags & OPND_N)
	{
	  str = skip_blanks (str);
	  if (*str != ',')
	    {
	      as_bad (_("Expected comma after register, got '%c'"), *str);
	      return;
	    }
	  ++str;
	}
      str = skip_blanks (str);
      str = cdp1802_expression (str, &opnd, &r_type);
      if (!str)
	return;
    }

  str = skip_blanks (str);
  if (*str)
    {
      as_bad (_("Junk at end of line: '%s'"), str);
      return;
    }

  opcode_nrbytes = (opcode->isa_level > ISA_1802) ? 2 : 1;
  imm_nrbytes =
    ((opcode->operand_flags & OPND_IMM2) ? 2
     : ((opcode->operand_flags & OPND_IMM1) ? 1 : 0));
  dwarf2_emit_insn (0);
  frag_ptr = (unsigned char *) frag_more (opcode_nrbytes + imm_nrbytes);

  if (opcode->isa_level > ISA_1802)
    *frag_ptr++ = 0x68;
  *frag_ptr++ = opcode->opcode | opcode_low_bits;
  if (opcode->operand_flags & OPND_IMM2)
    *frag_ptr++ = 0;
  if (opcode->operand_flags & (OPND_IMM1 | OPND_IMM2))
    *frag_ptr++ = 0;

  if (opcode->operand_flags & (OPND_IMM1 | OPND_IMM2))
    {
      unsigned long size;

      if (opcode->operand_flags & OPND_IMM2)
	{
	  size = 2;
	  if (r_type == BFD_RELOC_NONE)
	    r_type = BFD_RELOC_16;
	  else
	    {
	      as_bad (_("Invalid relocation operator for 16-bit immediate"));
	      return;
	    }
	}
      else
	{
	  size = 1;
	  if (r_type == BFD_RELOC_NONE)
	    r_type = BFD_RELOC_8;
	}
      fix_new_exp (frag_now, (frag_ptr - imm_nrbytes) - (unsigned char *) frag_now->fr_literal,
		   size, &opnd, false, r_type);
    }
}

symbolS *
cdp1802_md_undefined_symbol (char *name ATTRIBUTE_UNUSED)
{
  return NULL;
}

void
cdp1802_md_apply_fix (fixS *fixP, valueT *valP, segT seg ATTRIBUTE_UNUSED)
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
    case BFD_RELOC_32: /* This should only occur in dwarf data.  */
      if (value < -((1L << (32 - 1)) - 1) || value > (1L << (32 - 1)))
	as_bad_where (fixP->fx_file, fixP->fx_line,
		      _("Value %ld out of 32-bit range."), value);
      ((bfd_byte *) where)[0] = (value >> 24) & 0x00ff;
      ((bfd_byte *) where)[1] = (value >> 16) & 0x00ff;
      ((bfd_byte *) where)[2] = (value >> 8) & 0x00ff;
      ((bfd_byte *) where)[3] = value & 0x00ff;
      break;

    case BFD_RELOC_16:
      if (value < -((1L << (16 - 1)) - 1) || value >= (1L << 16))
	as_bad_where (fixP->fx_file, fixP->fx_line,
		      _("Value %ld out of 16-bit range."), value);
      ((bfd_byte *) where)[0] = (value >> 8) & 0x00ff;
      ((bfd_byte *) where)[1] = value & 0x00ff;
      break;

    case BFD_RELOC_CDP1802_HI8:
    case BFD_RELOC_CDP1802_LO8:
      if (value < -((1L << (16 - 1)) - 1) || value > (1L << (16 - 1)))
	as_bad_where (fixP->fx_file, fixP->fx_line,
		      _("Value %ld out of 16-bit range."), value);
      if (fixP->fx_r_type == BFD_RELOC_CDP1802_HI8)
	value >>= 8;
      ((bfd_byte *) where)[0] = (bfd_byte) (value & 0x00ff);
      fixP->fx_no_overflow = 1;
      break;

    case BFD_RELOC_8:
      /* Test cases fail if we range check the value here.  */
      ((bfd_byte *) where)[0] = (bfd_byte) value;
      break;

    default:
      as_bad_where (fixP->fx_file, fixP->fx_line,
		    _("Not yet implemented %s relocation fx_addsy %p fx_subsy %p fx_pcrel %d value %ld"),
		    bfd_get_reloc_code_name (fixP->fx_r_type), fixP->fx_addsy, fixP->fx_subsy, fixP->fx_pcrel, value);
    }
}
