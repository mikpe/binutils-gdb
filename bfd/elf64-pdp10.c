/* PDP-10-specific support for 64-bit ELF
   Copyright (C) 2023-2024 Free Software Foundation, Inc.
   Contributed by Mikael Pettersson <mikpelinux@gmail.com>

   This file is part of BFD, the Binary File Descriptor library.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING3.  If not,
   see <http://www.gnu.org/licenses/>.  */

#include "sysdep.h"
#include "bfd.h"
#include "libbfd.h"
#include "elf-bfd.h"
#include "elf/pdp10.h"
#include "libiberty.h"

/* The relocation "howto" table.  The elements must have the same
   indices as their R_PDP10_ numbers in include/elf/pdp10.h.  */
static reloc_howto_type pdp10_elf_howto_table[] =
{
  /* No relocation.  */
  HOWTO (R_PDP10_NONE,		/* type */
	 0,			/* rightshift */
	 0,			/* size (0 = byte, 1 = 2 byte, 2 = 4 byte) */
	 0,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_PDP10_NONE",	/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0,			/* dst_mask */
	 false),		/* pcrel_offset */

  /* GNU extension to record C++ vtable hierarchy.  */
  HOWTO (R_PDP10_GNU_VTINHERIT,	/* type */
	 0,			/* rightshift */
	 0,			/* size */
	 0,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 NULL,			/* special_function */
	 "R_PDP10_GNU_VTINHERIT", /* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0,			/* dst_mask */
	 false),		/* pcrel_offset */

  /* GNU extension to record C++ vtable member usage.  */
  HOWTO (R_PDP10_GNU_VTENTRY,	/* type */
	 0,			/* rightshift */
	 0,			/* size */
	 0,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 _bfd_elf_rel_vtable_reloc_fn, /* special_function */
	 "R_PDP10_GNU_VTENTRY",	/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0,			/* dst_mask */
	 false),		/* pcrel_offset */

  /* A 64 (really 36) bit reference to a symbol.  */
  HOWTO (R_PDP10_ABS64,		/* type */
	 0,			/* rightshift */
	 8,			/* size */
	 64,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_PDP10_ABS64",	/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0,			/* dst_mask */
	 false),		/* pcrel_offset */
};

static reloc_howto_type *
pdp10_reloc_type_lookup (bfd *abfd ATTRIBUTE_UNUSED, bfd_reloc_code_real_type code)
{
  enum elf_pdp10_reloc_type r = R_PDP10_NONE;

  switch (code)
    {
    case BFD_RELOC_NONE:
      r = R_PDP10_NONE;
      break;
    case BFD_RELOC_VTABLE_INHERIT:
      r = R_PDP10_GNU_VTINHERIT;
      break;
    case BFD_RELOC_VTABLE_ENTRY:
      r = R_PDP10_GNU_VTENTRY;
      break;
    case BFD_RELOC_64:
      r = R_PDP10_ABS64;
      break;
    default:
      return NULL;
    }

  return &pdp10_elf_howto_table[r];
}

static reloc_howto_type *
pdp10_reloc_name_lookup (bfd *abfd ATTRIBUTE_UNUSED, const char *r_name)
{
  unsigned int i;

  for (i = 0; i < ARRAY_SIZE (pdp10_elf_howto_table); i++)
    if (pdp10_elf_howto_table[i].name != NULL
	&& strcasecmp (pdp10_elf_howto_table[i].name, r_name) == 0)
      return &pdp10_elf_howto_table[i];

  return NULL;
}

static bool
pdp10_info_to_howto (bfd *abfd ATTRIBUTE_UNUSED, arelent *cache_ptr, Elf_Internal_Rela * dst)
{
  unsigned int r_type;

  r_type = ELF64_R_TYPE (dst->r_info);
  if (r_type >= R_PDP10_max)
    return false;
  cache_ptr->howto = &pdp10_elf_howto_table[r_type];
  return true;
}

/* Perform a single relocation.  By default we use the standard BFD
   routines, but a few relocs, we have to do them ourselves.  */

static bfd_reloc_status_type
pdp10_final_link_relocate (reloc_howto_type *howto,
			   bfd *input_bfd,
			   asection *input_section,
			   bfd_byte *contents,
			   Elf_Internal_Rela *rel,
			   bfd_vma relocation)
{
  bfd_reloc_status_type r = bfd_reloc_ok;

  switch (howto->type)
    {
    default:
      r = _bfd_final_link_relocate (howto, input_bfd, input_section,
				    contents, rel->r_offset,
				    relocation, rel->r_addend);
    }

  return r;
}

/* Relocate a PDP-10 ELF section.

   The RELOCATE_SECTION function is called by the new ELF backend linker
   to handle the relocations for a section.

   The relocs are always passed as Rela structures; if the section
   actually uses Rel structures, the r_addend field will always be
   zero.

   This function is responsible for adjusting the section contents as
   necessary, and (if using Rela relocs and generating a relocatable
   output file) adjusting the reloc addend as necessary.

   This function does not have to worry about setting the reloc
   address or the reloc symbol index.

   LOCAL_SYMS is a pointer to the swapped in local symbols.

   LOCAL_SECTIONS is an array giving the section in the input file
   corresponding to the st_shndx field of each local symbol.

   The global hash table entry for the global symbols can be found
   via elf_sym_hashes (input_bfd).

   When generating relocatable output, this function must handle
   STB_LOCAL/STT_SECTION symbols specially.  The output symbol is
   going to be the section symbol corresponding to the output
   section, which means that the addend must be adjusted
   accordingly.  */

static int
pdp10_elf_relocate_section (bfd *output_bfd, struct bfd_link_info *info,
			    bfd *input_bfd, asection *input_section,
			    bfd_byte *contents, Elf_Internal_Rela *relocs,
			    Elf_Internal_Sym *local_syms,
			    asection **local_sections)
{
  Elf_Internal_Shdr *symtab_hdr;
  struct elf_link_hash_entry **sym_hashes;
  Elf_Internal_Rela *rel;
  Elf_Internal_Rela *relend;

  symtab_hdr = &elf_tdata (input_bfd)->symtab_hdr;
  sym_hashes = elf_sym_hashes (input_bfd);

  relend = relocs + input_section->reloc_count;
  for (rel = relocs; rel < relend; rel++)
    {
      reloc_howto_type *howto;
      unsigned long r_symndx;
      Elf_Internal_Sym *sym;
      asection *sec;
      struct elf_link_hash_entry *h;
      bfd_vma relocation;
      bfd_reloc_status_type r;
      const char *name;
      int r_type;

      r_type = ELF64_R_TYPE (rel->r_info);
      if (r_type == (int) R_PDP10_GNU_VTINHERIT
	  || r_type == (int) R_PDP10_GNU_VTENTRY)
	continue;
      if ((unsigned int) r_type >= R_PDP10_max)
	{
	  bfd_set_error (bfd_error_bad_value);
	  return false;
	}
      howto = pdp10_elf_howto_table + r_type;
      r_symndx = ELF64_R_SYM (rel->r_info);

      /* This is a final link.  */
      h      = NULL;
      sym    = NULL;
      sec    = NULL;

      if (r_symndx < symtab_hdr->sh_info)
	{
	  /* Local symbol.  */
	  sym = local_syms + r_symndx;
	  sec = local_sections [r_symndx];
	  relocation = _bfd_elf_rela_local_sym (output_bfd, sym, &sec, rel);

	  name = bfd_elf_string_from_elf_section
	    (input_bfd, symtab_hdr->sh_link, sym->st_name);
	  if (! name || ! *name)
	    name = bfd_section_name (sec);
	}
      else
	{
	  bool unresolved_reloc, warned, ignored;

	  RELOC_FOR_GLOBAL_SYMBOL (info, input_bfd, input_section, rel,
				   r_symndx, symtab_hdr, sym_hashes,
				   h, sec, relocation,
				   unresolved_reloc, warned, ignored);

	  name = h->root.root.string;
	}

      if (sec != NULL && discarded_section (sec))
	RELOC_AGAINST_DISCARDED_SECTION (info, input_bfd, input_section,
					 rel, 1, relend, howto, 0, contents);

      if (bfd_link_relocatable (info))
	continue;

      r = pdp10_final_link_relocate (howto, input_bfd, input_section,
				     contents, rel, relocation);

      if (r != bfd_reloc_ok)
	{
	  const char *msg = NULL;

	  switch (r)
	    {
	    case bfd_reloc_overflow:
	      (*info->callbacks->reloc_overflow)
		(info, (h ? &h->root : NULL), name, howto->name,
		 (bfd_vma) 0, input_bfd, input_section, rel->r_offset);
	      break;

	    case bfd_reloc_undefined:
	      (*info->callbacks->undefined_symbol)
		(info, name, input_bfd, input_section, rel->r_offset, true);
	      break;

	    case bfd_reloc_outofrange:
	      msg = _("internal error: out of range error");
	      break;

	    case bfd_reloc_notsupported:
	      msg = _("internal error: unsupported relocation error");
	      break;

	    case bfd_reloc_dangerous:
	      msg = _("internal error: dangerous relocation");
	      break;

	    default:
	      msg = _("internal error: unknown error");
	      break;
	    }

	  if (msg)
	    (*info->callbacks->warning) (info, msg, name, input_bfd,
					 input_section, rel->r_offset);
	}
    }

  return true;
}

#define ELF_ARCH			bfd_arch_pdp10
#define ELF_MACHINE_CODE		EM_PDP10
#define ELF_MAXPAGESIZE			(512 * 4 * 2)	/* 512 words, 4 nonets per word, two octets per nonet */
#define TARGET_BIG_NAME			"elf64-pdp10"
#define TARGET_BIG_SYM			pdp10_elf64_vec

#define elf_backend_relocate_section	pdp10_elf_relocate_section
#define elf_info_to_howto		pdp10_info_to_howto
#define bfd_elf64_bfd_reloc_type_lookup	pdp10_reloc_type_lookup
#define bfd_elf64_bfd_reloc_name_lookup pdp10_reloc_name_lookup

#include "elf64-target.h"
