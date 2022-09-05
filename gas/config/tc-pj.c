/*-
   tc-pj.c -- Assemble code for picoJava
   Copyright (C) 1999 Free Software Foundation.

   This file is part of GAS, the GNU Assembler.

   GAS is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   GAS is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GAS; see the file COPYING.  If not, write to
   the Free Software Foundation, 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* Contributed by Steve Chamberlain of Transmeta, sac@pobox.com */

#include "as.h"
#include "opcode/pj.h"


extern const pj_opc_info_t pj_opc_info[512];

const char comment_chars[] = "!/";
const char line_separator_chars[] = ";";
const char line_comment_chars[] = "/!#";

static int pending_reloc;
static struct hash_control *opcode_hash_control;


static void
little (ignore)
     int ignore ATTRIBUTE_UNUSED;
{
  target_big_endian = 0;
}

static void
big (ignore)
     int ignore ATTRIBUTE_UNUSED;
{
  target_big_endian = 1;
}

static void
words_needed (varargs)
     int varargs;
{
  char *name;
  char c;
  symbolS *sym;
  int words;

  name = input_line_pointer;
  c = get_symbol_end ();
  sym = symbol_find_or_make (name);
  *input_line_pointer = c;

  SKIP_WHITESPACE ();
  if (*input_line_pointer == ',')
    ++input_line_pointer;

  SKIP_WHITESPACE ();
  words = get_absolute_expression();

  c = get_symbol_end ();

  *input_line_pointer = c;

  S_SET_OTHER (sym, words + 2 + (varargs ? 0x80 : 0));

  demand_empty_rest_of_line ();
}

#define C(what,length)          ((what)|(length))
#define GET_WHAT(x)             ((x) & ~7)

#define REL16_DISP 		001
#define REL32_DISP 		002
#define UNDEF_REL16_DISP 	000
#define UNDEF_REL32_DISP 	003

#define COND  			010
#define UNCOND 			020
#define END 			030

#define REL16_F 127
#define REL16_B -128
#define REL32_F 0
#define REL32_B 0

static bfd_byte revop [256];
relax_typeS md_relax_table[C (END, 0)];

/* The .check_call <name>,<pushed_size>,<arg_words>
   marks the next byte with a magic relocation which marks it for
   linker massaging  if the # of words pushed doesn't match what the
   callee expects. */

static void
check_call (ignore)
     int ignore ATTRIBUTE_UNUSED;
{
  expressionS exp;
  fixS *p;

  expression (&exp);

  if (*input_line_pointer == ',')
    ++input_line_pointer;

  SKIP_WHITESPACE ();
  p = fix_new_exp (frag_now, frag_more (0) - frag_now->fr_literal, 0, &exp, 0,
		    BFD_RELOC_PJ_WORDS_PUSHED);

  p->fx_addnumber = get_absolute_expression();
  SKIP_WHITESPACE ();
  demand_empty_rest_of_line ();
}



const pseudo_typeS md_pseudo_table[] = {
  {"little",    		little, 	0},
  {"big",       		big,    	0},
  {"words_needed",     		words_needed,   0},
  {"varargs_words_needed", 	words_needed,   1},
  {"check_call",     		check_call,     0},
  {0, 0, 0}
};


const char FLT_CHARS[] = "rRsSfFdDxXpP";
const char EXP_CHARS[] = "eE";

void
md_operand (op)
     expressionS *op;
{
  if (strncmp (input_line_pointer, "%hilo", 5) == 0)
    {
      if (pending_reloc)
	as_bad (_ ("confusing relocation expressions"));
      pending_reloc = BFD_RELOC_PJ_CODE_HILO;
      input_line_pointer += 5;
      expression (op);
    }
  if (strncmp (input_line_pointer, "%hi16", 5) == 0)
    {
      if (pending_reloc)
	as_bad (_ ("confusing relocation expressions"));
      pending_reloc = BFD_RELOC_PJ_CODE_HI16;
      input_line_pointer += 5;
      expression (op);
    }
  if (strncmp (input_line_pointer, "%lo16", 5) == 0)
    {
      if (pending_reloc)
	as_bad (_ ("confusing relocation expressions"));
      pending_reloc = BFD_RELOC_PJ_CODE_LO16;
      input_line_pointer += 5;
      expression (op);
    }
  if (strncmp (input_line_pointer, "%rel32", 6) == 0)
    {
      if (pending_reloc)
	as_bad (_ ("confusing relocation expressions"));
      pending_reloc = BFD_RELOC_PJ_CODE_REL32;
      input_line_pointer += 6;
      expression (op);
    }
}

/* Parse an expression and then restore the input line pointer. */

static char *
parse_exp_save_ilp (s, op)
     char *s;
     expressionS *op;
{
  char *save = input_line_pointer;
  input_line_pointer = s;
  expression (op);
  s = input_line_pointer;
  input_line_pointer = save;
  return s;
}

/* This is called by emit_expr via TC_CONS_FIX_NEW when creating a
   reloc for a cons.  We could use the definition there, except that
   we want to handle magic pending reloc expressions specially.  */

void
pj_cons_fix_new_pj (frag, where, nbytes, exp)
     fragS *frag;
     int where;
     int nbytes;
     expressionS *exp;
{
  static int rv[5][2] = { {0, 0},
  {BFD_RELOC_8, BFD_RELOC_8},
  {BFD_RELOC_PJ_CODE_DIR16, BFD_RELOC_16},
  {0, 0},
  {BFD_RELOC_PJ_CODE_DIR32, BFD_RELOC_32}
  };

  fix_new_exp (frag, where, nbytes, exp, 0,
	       pending_reloc ? pending_reloc
	       : rv[nbytes][(now_seg->flags & SEC_CODE) ? 0 : 1]);

  pending_reloc = 0;
}

/* Turn a reloc description character from the pj-opc.h table into
   code which BFD can handle. */

static int
c_to_r (x)
     char x;
{
  switch (x)
    {
    case O_R8:
      return BFD_RELOC_8_PCREL;
    case O_U8:
    case O_8:
      return BFD_RELOC_8;
    case O_R16:
      return BFD_RELOC_PJ_CODE_REL16;
    case O_U16:
    case O_16:
      return BFD_RELOC_PJ_CODE_DIR16;
    case O_R32:
      return BFD_RELOC_PJ_CODE_REL32;
    case O_32:
      return BFD_RELOC_PJ_CODE_DIR32;
    }
  abort ();
  return 0;
}

/* Handler for the ipush fake opcode,
   turns ipush <foo> into sipush lo16<foo>, sethi hi16<foo>. */

static void
ipush_code (opcode, str)
     pj_opc_info_t *opcode ATTRIBUTE_UNUSED;
     char *str;
{
  int mod = 0;
  char *b = frag_more (6);
  expressionS arg;
  b[0] = 0x11;
  b[3] = 0xed;
  parse_exp_save_ilp (str + 1, &arg, &mod);
  if (mod)
    as_bad (_("can't have relocation for ipush"));


  fix_new_exp (frag_now, b - frag_now->fr_literal + 1, 2, 
	       &arg,  0, BFD_RELOC_PJ_CODE_HILO);
}


/* Handler for mini macros. */

static void
expand_macro (opcode, str)
     pj_opc_info_t *opcode ATTRIBUTE_UNUSED;
     char *str;
{
  char *operands[10];
  unsigned int count;
  char *filled_in;
  char *s;
  const char *format;

  size_t max_operand_len = 0;
  int max_expanded_size = 0;
  size_t i;
  /* Gather the users operands into the operands array.  */
  count = 0;
  s = str;

  while (*s)
    {
      if (count >= sizeof operands / sizeof operands[0])
	break;
      operands[count++] = s;
      s = strchr (s, ',');
      if (s == (char *) NULL)
	break;
      *s++ = '\0';
    }

  for (i = 0; i < count; i++)
    if (strlen (operands[i]) > max_operand_len)
      max_operand_len = strlen (operands[i]);

  /* Work out the maximum size of the instructions built for assembly
     - can't be anly longer than the length of the largest operand * 
     half the size of the macro string - which is worst case if the
     macro is just operands. */

  max_expanded_size =
    strlen (opcode->name) + strlen (opcode->name) * max_operand_len / 2;

  /* Run though the macro, filling in the parameters and
     assembling. */

  filled_in = alloca (max_expanded_size + 1);

  format = opcode->name;
  while (*format)
    {
      char *dst = filled_in;

      while (1)
	{
	  if (*format == '@')
	    {
	      char *src = operands[format[1] - '0'];
	      while (*src)
		*dst++ = *src++;
	      format += 2;
	    }
	  else if (format[0] == ';' || format[0] == 0)
	    {
	      *dst++ = 0;
	      md_assemble (filled_in);
	      if (format[0] == 0)
		break;
	      dst = filled_in;
	      format++;
	    }
	  else
	    {
	      *dst++ = *format++;
	    }
	}
    }
}



/* Insert names into the opcode table which are really mini macros,
   not opcodes.  The fakeness is inidicated with an opcode of -2. */

static void
add_macro (name, def)
     const char *name;
     const char *def;
{
  pj_opc_info_t *fake = (pj_opc_info_t *) xmalloc (sizeof (pj_opc_info_t));

  fake->opcode = -2;
  fake->opcode_next = -2;
  fake->name = def;
  hash_insert (opcode_hash_control, name, (char *) fake);
}


/* Enter another entry into the opcode hash table so the same opcode
   can have another name. */
static void
alias (new, old)
     const char *new;
     const char *old;
{
  hash_insert (opcode_hash_control, new,
	       (char *) hash_find (opcode_hash_control, old));
}


/* This function is called once, at assembler startup time.  It sets
   up the hash table with all the opcodes in it, and also initializes
   some aliases for compatibility with other assemblers. */

void
md_begin ()
{
  register const pj_opc_info_t *opcode;
  register relax_typeS *table;
  opcode_hash_control = hash_new ();

  /* Insert names into hash table */
  for (opcode = pj_opc_info; opcode->name; opcode++)
    hash_insert (opcode_hash_control, opcode->name, (char *) opcode);

  add_macro ("ipush", "sipush %hilo(@0);sethi 0");

  /* Add some aliases for opcode names. */
  alias ("ifeq_s", "ifeq");
  alias ("ifne_s", "ifne");
  alias ("if_icmpge_s", "if_icmpge");
  alias ("if_icmpne_s", "if_icmpne");
  alias ("if_icmpeq_s", "if_icmpeq");
  alias ("if_icmpgt_s", "if_icmpgt");
  alias ("goto_s", "goto");

  bfd_set_arch_mach (stdoutput, TARGET_ARCH, 0);

  /* Initialize the relax table.  We use a local variable to avoid
     warnings about modifying a supposedly const data structure.  */

  table = (relax_typeS *) md_relax_table;

  table[C (COND, REL16_DISP)].rlx_forward = REL16_F;
  table[C (COND, REL16_DISP)].rlx_backward = REL16_B;
  table[C (COND, REL16_DISP)].rlx_length = 3;
  table[C (COND, REL16_DISP)].rlx_more = C (COND, REL32_DISP);

  table[C (COND, REL32_DISP)].rlx_forward = REL32_F;
  table[C (COND, REL32_DISP)].rlx_backward = REL32_B;
  table[C (COND, REL32_DISP)].rlx_length = 8;
  table[C (COND, REL32_DISP)].rlx_more = 0;

  table[C (UNCOND, REL16_DISP)].rlx_forward = REL16_F;
  table[C (UNCOND, REL16_DISP)].rlx_backward = REL16_B;
  table[C (UNCOND, REL16_DISP)].rlx_length = 3;
  table[C (UNCOND, REL16_DISP)].rlx_more = C (UNCOND, REL32_DISP);

  table[C (UNCOND, REL32_DISP)].rlx_forward = REL32_F;
  table[C (UNCOND, REL32_DISP)].rlx_backward = REL32_B;
  table[C (UNCOND, REL32_DISP)].rlx_length = 5;
  table[C (UNCOND, REL32_DISP)].rlx_more = 0;


  memcpy (&table[C (COND, UNDEF_REL16_DISP)],
	  &table[C (COND, REL16_DISP)], sizeof (table[0]));

  memcpy (&table[C (COND, UNDEF_REL32_DISP)],
	  &table[C (COND, REL32_DISP)], sizeof (table[0]));

  memcpy (&table[C (UNCOND, UNDEF_REL16_DISP)],
	  &table[C (UNCOND, REL16_DISP)], sizeof (table[0]));

  memcpy (&table[C (UNCOND, UNDEF_REL32_DISP)],
	  &table[C (UNCOND, REL32_DISP)], sizeof (table[0]));

  pj_opc_rev_init (revop);
}

/* This is the guts of the machine-dependent assembler.  STR points to a
   machine dependent instruction.  This function is supposed to emit
   the frags/bytes it assembles to.
 */

void
md_assemble (str)
     char *str;
{
  unsigned char *op_start;
  unsigned char *op_end;
  pj_opc_info_t *opcode;
  char *output;
  int idx = 0;
  char pend;
  int nlen = 0;

  /* Drop leading whitespace */
  while (*str == ' ')
    str++;

  /* find the op code end */
  for (op_start = op_end = (unsigned char *) (str);
       *op_end && !is_end_of_line[*op_end] && *op_end != ' '; op_end++)
    nlen++;

  pend = *op_end;
  *op_end = 0;

  if (nlen == 0)
    as_bad (_("can't find opcode "));

  opcode = (pj_opc_info_t *) hash_find (opcode_hash_control, op_start);
  *op_end = pend;

  if (opcode == NULL)
    {
      as_bad (_("unknown opcode %s"), op_start);
      return;
    }

  if (opcode->opcode == -1)
    {
      /* It's a fake opcode.. dig out the args and pretend that was
         what we were passed */
      ((void (*)()) opcode->name) (opcode, op_end);
    }
  else if (opcode->opcode == -2)
    {
      /* It's a fake opcode.. dig out the args and pretend that was
         what we were passed */
      expand_macro (opcode, op_end);
    }
  else
    {
      if (PCREL (opcode->arg[0]))
	{
	  expressionS arg;
	  int len =
	    ASIZE (opcode->arg[0]) == 4 ? UNDEF_REL32_DISP : UNDEF_REL16_DISP;
	  int what = revop[opcode->opcode] ? COND : UNCOND;
	  if (*op_end == ',')
	    op_end++;

	  if (*op_end == 0)
	    as_bad ("expected expresssion");

	  op_end = parse_exp_save_ilp (op_end, &arg);

	  /* Emit a variable sized chunk to hold any extra displacement and relax
	     opcode. */

	  output = frag_var (rs_machine_dependent,
			     (what == COND) ? 8 : 5,
			     3,
			     C (what, len),
			     arg.X_add_symbol, arg.X_add_number, output);
	  *output = opcode->opcode;
	}
      else
	{
	  int an;
	  output = frag_more (opcode->len);
	  output[idx++] = opcode->opcode;

	  if (opcode->opcode_next != -1)
	    output[idx++] = opcode->opcode_next;

	  for (an = 0; opcode->arg[an]; an++)
	    {
	      expressionS arg;

	      if (*op_end == ',' && an != 0)
		op_end++;

	      if (*op_end == 0)
		as_bad ("expected expresssion");

	      op_end = parse_exp_save_ilp (op_end, &arg);

	      fix_new_exp (frag_now,
			   output - frag_now->fr_literal + idx,
			   ASIZE (opcode->arg[an]), &arg, 0,
			   pending_reloc 
			   ? pending_reloc 
			   : c_to_r (opcode->arg[an]));

	      idx += ASIZE (opcode->arg[an]);
	      pending_reloc = 0;
	    }
	}
      while (isspace (*op_end))
	op_end++;

      if (*op_end != 0)
	as_warn ("extra stuff on line ignored");

    }

  if (pending_reloc)
    as_bad ("Something forgot to clean up\n");
}

/* Turn a string in input_line_pointer into a floating point constant
   of type type, and store the appropriate bytes in *litP.  The number
   of LITTLENUMS emitted is stored in *sizeP .  An error message is
   returned, or NULL on OK.  */

char *
md_atof (type, litP, sizeP)
     int type;
     char *litP;
     int *sizeP;
{
  int prec;
  LITTLENUM_TYPE words[4];
  char *t;
  int i;

  switch (type)
    {
    case 'f':
      prec = 2;
      break;

    case 'd':
      prec = 4;
      break;

    default:
      *sizeP = 0;
      return _("bad call to md_atof");
    }

  t = atof_ieee (input_line_pointer, type, words);
  if (t)
    input_line_pointer = t;

  *sizeP = prec * 2;

  if (!target_big_endian)
    {
      for (i = prec - 1; i >= 0; i--)
	{
	  md_number_to_chars (litP, (valueT) words[i], 2);
	  litP += 2;
	}
    }
  else
    {
      for (i = 0; i < prec; i++)
	{
	  md_number_to_chars (litP, (valueT) words[i], 2);
	  litP += 2;
	}
    }

  return NULL;
}


CONST char *md_shortopts = "";

struct option md_longopts[] = {

#define OPTION_LITTLE (OPTION_MD_BASE)
#define OPTION_BIG    (OPTION_LITTLE + 1)

  {"ml", no_argument, NULL, OPTION_LITTLE},
  {"mb", no_argument, NULL, OPTION_BIG},
  {NULL, no_argument, NULL, 0}
};
size_t md_longopts_size = sizeof (md_longopts);

int
md_parse_option (c, arg)
     int c;
     char *arg ATTRIBUTE_UNUSED;
{
  switch (c)
    {
    case OPTION_LITTLE:
      little ();
      break;
    case OPTION_BIG:
      big ();
      break;
    default:
      return 0;
    }
  return 1;
}

void
md_show_usage (stream)
     FILE *stream;
{
  fprintf (stream, _ ("\
PJ options:\n\
-little			generate little endian code\n\
-big			generate big endian code\n"));
}

/* Apply a fixup to the object file.  */

int
md_apply_fix (fixP, valp)
     fixS *fixP;
     valueT *valp;
{

  char *buf = fixP->fx_where + fixP->fx_frag->fr_literal;
  long val = *valp;
  long max, min;
  int shift;
  long addr = fixP->fx_frag->fr_address + fixP->fx_where;

  max = min = 0;
  shift = 0;

  /* adjust_reloc_syms won't convert a reloc against a weak symbol
     into a reloc against a section, but bfd_install_relocation will
     screw up if the symbol is defined, so we have to adjust val here
     to avoid the screw up later.  */
  if (fixP->fx_addsy != NULL && S_IS_WEAK (fixP->fx_addsy))
    val -= S_GET_VALUE (fixP->fx_addsy);

  switch (fixP->fx_r_type)
    {
    case BFD_RELOC_PJ_WORDS_PUSHED:
      fixP->fx_done = 0;
      break;

    case BFD_RELOC_VTABLE_INHERIT:
    case BFD_RELOC_VTABLE_ENTRY:
      fixP->fx_done = 0;
      break;

    case BFD_RELOC_PJ_CODE_REL16:
      if (val < -0x8000 || val >= 0x7fff)
	as_bad_where (fixP->fx_file, fixP->fx_line, _("pcrel too far"));

      buf[0] |= val >> 8;
      buf[1] = val & 0xff;
      break;

    case BFD_RELOC_PJ_CODE_HI16:
      fixP->fx_no_overflow = 1;
      *buf++ = val >> 24;
      *buf++ = val >> 16;
      break;

    case BFD_RELOC_PJ_CODE_LO16:
      fixP->fx_no_overflow = 1;
      *buf++ = val >> 8;
      *buf++ = val >> 0;
      break;


    case BFD_RELOC_PJ_CODE_HILO:
      fixP->fx_no_overflow = 1;
      *buf++ = val >> 8;
      *buf++ = val >> 0;
      buf++;
      *buf++ = val >> 24;
      *buf++ = val >> 16;
      break;

    case BFD_RELOC_PJ_CODE_DIR16:
      *buf++ = val >> 8;
      *buf++ = val >> 0;
      break;

    case BFD_RELOC_8:
      max = 0xff;
      min = -0xff;
      *buf++ = val;
      break;

    case BFD_RELOC_PJ_CODE_REL32:
    case BFD_RELOC_PJ_CODE_DIR32:
      *buf++ = val >> 24;
      *buf++ = val >> 16;
      *buf++ = val >> 8;
      *buf++ = val >> 0;
      break;

    case BFD_RELOC_32:
      if (target_big_endian)
	{
	  *buf++ = val >> 24;
	  *buf++ = val >> 16;
	  *buf++ = val >> 8;
	  *buf++ = val >> 0;
	}
      else
	{
	  *buf++ = val >> 0;
	  *buf++ = val >> 8;
	  *buf++ = val >> 16;
	  *buf++ = val >> 24;
	}
      break;

    case BFD_RELOC_16:
      if (target_big_endian)
	{
	  *buf++ = val >> 8;
	  *buf++ = val >> 0;
	}
      else
	{
	  *buf++ = val >> 0;
	  *buf++ = val >> 8;
	}
      break;


    default:
      abort ();
    }

  if (max != 0 && (val < min || val > max))
    as_bad_where (fixP->fx_file, fixP->fx_line, _("offset out of range"));

  return 0;
}

/* Put number into target byte order.  Always put values in an
   executable section into big endian order. */

void
md_number_to_chars (ptr, use, nbytes)
     char *ptr;
     valueT use;
     int nbytes;
{
  if (target_big_endian || now_seg->flags & SEC_CODE)
    number_to_chars_bigendian (ptr, use, nbytes);
  else
    number_to_chars_littleendian (ptr, use, nbytes);
}

/* Translate internal representation of relocation info to BFD target
   format.  */

arelent *
tc_gen_reloc (section, fixp)
     asection *section ATTRIBUTE_UNUSED;
     fixS *fixp;
{
  arelent *reloc = (arelent *) xmalloc (sizeof (arelent));
  reloc->sym_ptr_ptr = (asymbol **) xmalloc (sizeof (asymbol *));
  *reloc->sym_ptr_ptr = symbol_get_bfdsym (fixp->fx_addsy);
  reloc->address = fixp->fx_frag->fr_address + fixp->fx_where;
  reloc->howto = bfd_reloc_type_lookup (stdoutput, fixp->fx_r_type);
  reloc->addend = fixp->fx_addnumber;

  return reloc;
}

/* Called just before address relaxation.  Return the length
   by which a fragment must grow to reach it's destination.  */

int
md_estimate_size_before_relax (fragP, segment_type)
     register fragS *fragP;
     register segT segment_type;
{
  int what = GET_WHAT (fragP->fr_subtype);

  switch (fragP->fr_subtype)
    {
      /* Big things stay big. */
    case C (COND, UNDEF_REL32_DISP):
    case C (UNCOND, UNDEF_REL32_DISP):
      if (S_GET_SEGMENT (fragP->fr_symbol) == segment_type)
	fragP->fr_subtype = C (what, REL32_DISP);
      fragP->fr_var = md_relax_table[C (what, UNDEF_REL32_DISP)].rlx_length;
      break;
    default:
      abort ();
    case C (COND, UNDEF_REL16_DISP):
    case C (UNCOND, UNDEF_REL16_DISP):
      if (S_GET_SEGMENT (fragP->fr_symbol) == segment_type)
	{
	  /* Got a symbol and it's defined in this segment, become byte
	     sized - maybe it will fix up */
	  fragP->fr_subtype = C (what, REL16_DISP);
	  fragP->fr_var = md_relax_table[C (what, REL16_DISP)].rlx_length;
	}
      else
	{
	  fragP->fr_subtype = C (what, UNDEF_REL32_DISP);
	  fragP->fr_var = md_relax_table[C (what, REL32_DISP)].rlx_length;
	}
    }
  return fragP->fr_var;
}

/* Called after relaxing, change the frags from vars to fixes and fill
   in the relocation and instruction info so that md_apply_fix will do
   the right thing.  */

void
md_convert_frag (out, seg, fragP)
     bfd *out;
     segT seg;
     fragS *fragP;
{
  int target_address =  S_GET_VALUE (fragP->fr_symbol) 
    + fragP->fr_offset
    + symbol_get_frag (fragP->fr_symbol)->fr_address;
  int opcode_address = fragP->fr_address + fragP->fr_fix;
  int displacement_from_opcode_start = target_address - opcode_address;
  unsigned char *buffer = fragP->fr_fix + fragP->fr_literal;

  switch (fragP->fr_subtype)
    {
    case C (COND, REL16_DISP):
    case C (UNCOND, REL16_DISP):
      buffer[1] = displacement_from_opcode_start >> 8;
      buffer[2] = displacement_from_opcode_start;

      fragP->fr_fix += 3;
      fragP->fr_var = 0;
      break;

    case C (COND, UNDEF_REL32_DISP):
      fix_new (fragP, fragP->fr_fix + 4, 4, fragP->fr_symbol,
	       fragP->fr_offset, 0, BFD_RELOC_PJ_CODE_REL32);

      /* Fall through. */

    case C (COND, REL32_DISP):
      /* Turn a 16 bit conditional pc relative instruction into 32 bit
	 conditional pc relative instruction.  So invert the jump and
	 change the destination to point after a synthesized 32 bit
	 unconditional branch.  */

      displacement_from_opcode_start -= 3;

      buffer[0] = revop[buffer[0]];
      buffer[1] = 0;
      buffer[2] = 8;
      buffer[3] = GOTO_W;
      buffer[4] = displacement_from_opcode_start >> 24;
      buffer[5] = displacement_from_opcode_start >> 16;
      buffer[6] = displacement_from_opcode_start >> 8;
      buffer[7] = displacement_from_opcode_start >> 0;

      fragP->fr_fix += 8;
      fragP->fr_var = 0;
      break;

    case C (UNCOND, UNDEF_REL32_DISP):
      fix_new (fragP, fragP->fr_fix + 1, 4, fragP->fr_symbol,
	       fragP->fr_offset, 0, BFD_RELOC_PJ_CODE_REL32);

      /* Fall through.  */

    case C (UNCOND, REL32_DISP):
      /* Turn a 16 bit unconditional pc relative jump into a 32 bit pc relative jump. */

      buffer[0] = (buffer[0] == GOTO_W || buffer[0] == GOTO) ? GOTO_W : JSR_W;
      buffer[1] = displacement_from_opcode_start >> 24;
      buffer[2] = displacement_from_opcode_start >> 16;
      buffer[3] = displacement_from_opcode_start >> 8;
      buffer[4] = displacement_from_opcode_start >> 0;

      fragP->fr_fix += 5;
      fragP->fr_var = 0;
      break;

    default:
      abort ();
    }
}
