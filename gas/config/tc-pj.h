/*-This file is tc-pj.h

   Copyright (C) 1999 Free Software Foundation, Inc.

   Contributed by Steve Chamberlain of Transmeta, sac@pobox.com

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


/* Contributed by Steve Chamberlain, of Transmeta. sac@pobox.com. */

#define WORKING_DOT_WORD
#define IGNORE_NONSTANDARD_ESCAPES
#define TARGET_ARCH bfd_arch_pj

#ifndef TARGET_BYTES_BIG_ENDIAN
#define TARGET_BYTES_BIG_ENDIAN 0
#endif

#define TARGET_FORMAT (target_big_endian ? "elf32-pj" : "elf32-pjl")
#define LISTING_HEADER                    				\
  (target_big_endian                      				\
   ? "Pico Java GAS Big Endian"           				\
   : "Pico Java GAS Little Endian")

#define LISTING_LHS_WIDTH 1
#define LISTING_WORD_SIZE 4

void pj_cons_fix_new_pj PARAMS ((struct frag *, int, int, expressionS *));

#define md_section_align(SE,SZ)             (SZ)
#define md_undefined_symbol(NAME)           0


/* PC relative operands are relative to the start of the opcode, and the operand
   is always one byte into the opcode. */

#define md_pcrel_from(FIXP) 						\
 	((FIXP)->fx_where + (FIXP)->fx_frag->fr_address - 1)

#define TC_CONS_FIX_NEW(FRAG, WHERE, NBYTES, EXP) \
	pj_cons_fix_new_pj(FRAG, WHERE, NBYTES, EXP)


#define TC_FIX_TYPE valueT

/* hi16 and lo16 operations never overflow. */
#define TC_INIT_FIX_DATA(fixP) \
      if (fixP->fx_r_type == BFD_RELOC_PJ_CODE_HI16 \
	  || fixP->fx_r_type == BFD_RELOC_PJ_CODE_LO16)  \
          fixP->fx_no_overflow = 1;

extern struct relax_type md_relax_table[];
#define TC_GENERIC_RELAX_TABLE md_relax_table

#define obj_fix_adjustable(fixP)                          \
  (fixP->fx_addsy == NULL                                 \
   ||  (fixP->fx_r_type != BFD_RELOC_VTABLE_INHERIT       \
	&& fixP->fx_r_type != BFD_RELOC_VTABLE_ENTRY      \
	&& fixP->fx_r_type != BFD_RELOC_PJ_WORDS_PUSHED))

#define TC_FORCE_RELOCATION(fixP)                         \
  (fixP->fx_r_type == BFD_RELOC_VTABLE_ENTRY              \
   || fixP->fx_r_type == BFD_RELOC_VTABLE_INHERIT)




