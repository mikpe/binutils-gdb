/* tc-pdp10.h -- Assembler for PDP-10.
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

#ifndef TC_PDP10
#define TC_PDP10 1

#include "write.h"

/* The target BFD format.  */
#define TARGET_FORMAT "elf64-pdp10"

/* The target BFD architecture.  */
#define TARGET_ARCH bfd_arch_pdp10

/* The target BFD machine number.  */
#define TARGET_MACH bfd_mach_pdp10

/* The PDP-10 is big endian.  */
#define TARGET_BYTES_BIG_ENDIAN 1

extern const char pdp10_md_shortopts[];
extern const struct option pdp10_md_longopts[];
extern const size_t pdp10_md_longopts_size;
extern int pdp10_md_parse_option (int, const char *);
extern void pdp10_md_show_usage (FILE *);
#define md_shortopts pdp10_md_shortopts
#define md_longopts pdp10_md_longopts
#define md_longopts_size pdp10_md_longopts_size
#define md_parse_option pdp10_md_parse_option
#define md_show_usage pdp10_md_show_usage

extern void pdp10_md_begin (void);
#define md_begin pdp10_md_begin

extern void pdp10_md_assemble (char *);
#define md_assemble pdp10_md_assemble

extern const pseudo_typeS pdp10_md_pseudo_table[];
#define md_pseudo_table pdp10_md_pseudo_table

extern const char pdp10_comment_chars[];
extern const char pdp10_line_comment_chars[];
extern const char pdp10_line_separator_chars[];
#define comment_chars pdp10_comment_chars
#define line_comment_chars pdp10_line_comment_chars
#define line_separator_chars pdp10_line_separator_chars

extern const char pdp10_EXP_CHARS[];
extern const char pdp10_FLT_CHARS[];
#define EXP_CHARS pdp10_EXP_CHARS
#define FLT_CHARS pdp10_FLT_CHARS

extern symbolS *pdp10_md_undefined_symbol (char *);
#define md_undefined_symbol pdp10_md_undefined_symbol

extern void pdp10_md_operand (expressionS *);
#define md_operand pdp10_md_operand

extern bfd_reloc_code_real_type pdp10_tc_parse_cons_expression (expressionS *, int);
#define TC_PARSE_CONS_EXPRESSION(EXP, NBYTES) pdp10_tc_parse_cons_expression ((EXP), (NBYTES))

#define md_number_to_chars number_to_chars_bigendian

extern const char *pdp10_md_atof (int, char *, int *);
#define md_atof pdp10_md_atof

/* We do not want broken .word processing.  */
#define WORKING_DOT_WORD

extern int pdp10_md_estimate_size_before_relax (fragS *, segT);
#define md_estimate_size_before_relax pdp10_md_estimate_size_before_relax

extern void pdp10_md_convert_frag (bfd *, segT, fragS *);
#define md_convert_frag pdp10_md_convert_frag

extern void pdp10_md_apply_fix (fixS *, valueT *, segT);
#define md_apply_fix pdp10_md_apply_fix

/* Values passed to md_apply_fix don't include the symbol value.  */
#define MD_APPLY_SYM_VALUE(FIX) 0

extern arelent *pdp10_tc_gen_reloc (asection *, fixS *);
#define tc_gen_reloc pdp10_tc_gen_reloc

/* Call md_pcrel_from_section(), not md_pcrel_from().  */
extern long pdp10_md_pcrel_from_section (struct fix *, segT);
#define md_pcrel_from_section pdp10_md_pcrel_from_section
#define MD_PCREL_FROM_SECTION(FIXP, SEC) md_pcrel_from_section (FIXP, SEC)

extern valueT pdp10_md_section_align (segT, valueT);
#define md_section_align pdp10_md_section_align

#define LISTING_HEADER "PDP-10 GAS"

/* The rest are not described in internals.texi.  */

/* Permit temporary numeric labels.  */
#define LOCAL_LABELS_FB 1

/* .-foo gets turned into PC relative relocs.  */
#define DIFF_EXPR_OK

/* Dwarf support.  */
#define DWARF2_CIE_DATA_ALIGNMENT	(-1)
#define DWARF2_DEFAULT_RETURN_COLUMN	18 /* TBD */

#endif /* TC_PDP10 */
