/* This file is tc-m68k.h
   Copyright (C) 1987-2025 Free Software Foundation, Inc.

   This file is part of GAS, the GNU Assembler.

   GAS is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   GAS is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GAS; see the file COPYING.  If not, write to the Free
   Software Foundation, 51 Franklin Street - Fifth Floor, Boston, MA
   02110-1301, USA.  */

#ifndef TC_M68K_H
#define TC_M68K_H

#define TC_M68K 1

struct fix;

#define TARGET_BYTES_BIG_ENDIAN 1

#define TARGET_FORMAT "elf32-m68k"
#define TARGET_ARCH bfd_arch_m68k

#define tc_comment_chars m68k_comment_chars
extern const char *m68k_comment_chars;

#define LISTING_WORD_SIZE 2	/* A word is 2 bytes */
#define LISTING_LHS_WIDTH 2	/* One word on the first line */
#define LISTING_LHS_WIDTH_SECOND 2	/* One word on the second line */
#define LISTING_LHS_CONT_LINES 4/* And 4 lines max */
#define LISTING_HEADER "68K GAS "

#ifndef REGISTER_PREFIX
#define REGISTER_PREFIX '%'
#endif

#ifndef REGISTER_PREFIX_OPTIONAL
#define REGISTER_PREFIX_OPTIONAL 0
#endif

extern void m68k_mri_mode_change (int);
#define MRI_MODE_CHANGE(i) m68k_mri_mode_change (i)

extern int m68k_conditional_pseudoop (const pseudo_typeS *);
#define tc_conditional_pseudoop(pop) m68k_conditional_pseudoop (pop)

extern void m68k_frob_label (symbolS *);
#define tc_frob_label(sym) m68k_frob_label (sym)

extern void m68k_flush_pending_output (void);
#define md_flush_pending_output() m68k_flush_pending_output ()

extern void m68k_frob_symbol (symbolS *);

#define tc_frob_symbol(sym,punt)				\
do								\
  {								\
    if (S_GET_SEGMENT (sym) == reg_section)			\
      punt = 1;							\
    m68k_frob_symbol (sym);					\
  }								\
while (0)

#define NO_RELOC BFD_RELOC_NONE
#define RELAX_RELOC_ABS8  BFD_RELOC_8
#define RELAX_RELOC_ABS16 BFD_RELOC_16
#define RELAX_RELOC_ABS32 BFD_RELOC_32
#define RELAX_RELOC_PC8   BFD_RELOC_8_PCREL
#define RELAX_RELOC_PC16  BFD_RELOC_16_PCREL
#define RELAX_RELOC_PC32  BFD_RELOC_32_PCREL

#define tc_fix_adjustable(X) tc_m68k_fix_adjustable(X)
extern int tc_m68k_fix_adjustable (struct fix *);

/* Target *-*-elf implies an embedded target.  No shared libs.
   *-*-uclinux also requires special casing to prevent GAS from
   generating unsupported R_68K_PC16 relocs.  */
#define EXTERN_FORCE_RELOC \
  ((strcmp (TARGET_OS, "elf") != 0) && (strcmp (TARGET_OS, "uclinux") != 0))

/* Values passed to md_apply_fix don't include symbol values.  */
#define MD_APPLY_SYM_VALUE(FIX) 0

#define elf_tc_final_processing m68k_elf_final_processing
extern void m68k_elf_final_processing (void);

#define DIFF_EXPR_OK

extern int m68k_parse_long_option (char *);
#define md_parse_long_option m68k_parse_long_option

#define md_operand(x)

extern struct relax_type md_relax_table[];
#define TC_GENERIC_RELAX_TABLE md_relax_table

/* We can't do a byte jump to the next instruction, so in that case
   force word mode by faking AIM.  */
#define md_prepare_relax_scan(fragP, address, aim, this_state, this_type) \
  do									  \
    {								 	  \
      if (aim == 0 && this_type->rlx_forward == 127)			  \
        aim = 128;							  \
    }									  \
  while (0)

#define DWARF2_LINE_MIN_INSN_LENGTH 2

/* We want .cfi_* pseudo-ops for generating unwind info.  */
#define TARGET_USE_CFIPOP 1

#define DWARF2_DEFAULT_RETURN_COLUMN 24
#define DWARF2_CIE_DATA_ALIGNMENT (-4)

#define tc_regname_to_dw2regnum tc_m68k_regname_to_dw2regnum
extern int tc_m68k_regname_to_dw2regnum (const char *regname);

#define tc_cfi_frame_initial_instructions tc_m68k_frame_initial_instructions
extern void tc_m68k_frame_initial_instructions (void);

#ifdef TE_UCLINUX
/* elf2flt does not honor PT_LOAD's from the executable.
   .text and .eh_frame sections will not end up in the same segment and so
   we cannot use PC-relative encoding for CFI.  */
# define CFI_DIFF_EXPR_OK 0

/* However, follow compiler's guidance when it specifies encoding for LSDA.  */
# define CFI_DIFF_LSDA_OK 1
#endif

struct broken_word;
#define TC_CHECK_ADJUSTED_BROKEN_DOT_WORD(new_offset, brokw) \
  tc_m68k_check_adjusted_broken_word ((offsetT) (new_offset), (brokw))
extern void tc_m68k_check_adjusted_broken_word (offsetT,
						struct broken_word *);

/* We want to warn if any text labels are misaligned.  In order to get
   the right line number, we need to record the line number for each
   label.  */
struct m68k_tc_sy
{
  const char *file;
  unsigned int line;
  int text;
};

#define TC_SYMFIELD_TYPE struct m68k_tc_sy

#endif /* TC_M68K_H */
