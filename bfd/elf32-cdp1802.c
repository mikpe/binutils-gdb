/* ELF support for the RCA CDP1802 processor.
   Copyright (C) 2024 Free Software Foundation, Inc.
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
#include "elf/cdp1802.h"
#include "libiberty.h"

/* The relocation "howto" table.  The elements must have the same
   indices as their R_CDP1802_ numbers in include/elf/cdp1802.h.  */
static reloc_howto_type cdp1802_elf_howto_table[] =
{
  /* No relocation.  */
  HOWTO (R_CDP1802_NONE,	/* type */
	 0,			/* rightshift */
	 0,			/* size (0 = byte, 1 = 2 byte, 2 = 4 byte) */
	 0,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_CDP1802_NONE",	/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0,			/* dst_mask */
	 false),		/* pcrel_offset */

  /* An 8 bit absolute relocation.  */
  HOWTO (R_CDP1802_8,		/* type */
	 0,			/* rightshift */
	 1,			/* size */
	 8,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield,	/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_CDP1802_8",		/* name */
	 false,			/* partial_inplace */
	 0x00,			/* src_mask */
	 0xff,			/* dst_mask */
	 false),		/* pcrel_offset */

  /* An 8 bit absolute relocation (upper byte).  */
  HOWTO (R_CDP1802_HI8,		/* type */
	 8,			/* rightshift */
	 1,			/* size */
	 8,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield,	/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_CDP1802_HI8",	/* name */
	 false,			/* partial_inplace */
	 0x0000,		/* src_mask */
	 0x00ff,		/* dst_mask */
	 false),		/* pcrel_offset */

  /* An 8 bit absolute relocation (lower byte).  */
  HOWTO (R_CDP1802_LO8,		/* type */
	 0,			/* rightshift */
	 1,			/* size */
	 8,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_CDP1802_LO8",	/* name */
	 false,			/* partial_inplace */
	 0x0000,		/* src_mask */
	 0x00ff,		/* dst_mask */
	 false),		/* pcrel_offset */

  /* A 16 bit absolute relocation.  */
  HOWTO (R_CDP1802_16,		/* type */
	 0,			/* rightshift */
	 2,			/* size */
	 16,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield,	/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_CDP1802_16",	/* name */
	 false,			/* partial_inplace */
	 0x0000,		/* src_mask */
	 0xffff,		/* dst_mask */
	 false),		/* pcrel_offset */

  /* A 32 bit absolute relocation.  */
  HOWTO (R_CDP1802_32,		/* type */
	 0,			/* rightshift */
	 4,			/* size */
	 32,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield,	/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_CDP1802_32",	/* name */
	 false,			/* partial_inplace */
	 0x00000000,		/* src_mask */
	 0xffffffff,		/* dst_mask */
	 false),		/* pcrel_offset */

  /* GNU extension to record C++ vtable hierarchy.  */
  HOWTO (R_CDP1802_GNU_VTINHERIT,	/* type */
	 0,			/* rightshift */
	 4,			/* size */
	 0,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 NULL,			/* special_function */
	 "R_CDP1802_GNU_VTINHERIT",	/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0,			/* dst_mask */
	 false),		/* pcrel_offset */

  /* GNU extension to record C++ vtable member usage.  */
  HOWTO (R_CDP1802_GNU_VTENTRY,	/* type */
	 0,			/* rightshift */
	 4,			/* size */
	 0,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 _bfd_elf_rel_vtable_reloc_fn,  /* special_function */
	 "R_CDP1802_GNU_VTENTRY",	/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0,			/* dst_mask */
	 false),		/* pcrel_offset */
};

static reloc_howto_type *
cdp1802_reloc_type_lookup (bfd *abfd ATTRIBUTE_UNUSED, bfd_reloc_code_real_type code)
{
  enum elf_cdp1802_reloc_type r = R_CDP1802_NONE;

  switch (code)
    {
    case BFD_RELOC_NONE:
      r = R_CDP1802_NONE;
      break;
    case BFD_RELOC_8:
      r = R_CDP1802_8;
      break;
    case BFD_RELOC_CDP1802_HI8:
      r = R_CDP1802_HI8;
      break;
    case BFD_RELOC_CDP1802_LO8:
      r = R_CDP1802_LO8;
      break;
    case BFD_RELOC_16:
      r = R_CDP1802_16;
      break;
    case BFD_RELOC_32:
      r = R_CDP1802_32;
      break;
    case BFD_RELOC_VTABLE_INHERIT:
      r = R_CDP1802_GNU_VTINHERIT;
      break;
    case BFD_RELOC_VTABLE_ENTRY:
      r = R_CDP1802_GNU_VTENTRY;
      break;
    default:
      return NULL;
    }

  return &cdp1802_elf_howto_table[r];
}

static reloc_howto_type *
cdp1802_reloc_name_lookup (bfd *abfd ATTRIBUTE_UNUSED, const char *r_name)
{
  unsigned int i;

  for (i = 0; i < ARRAY_SIZE (cdp1802_elf_howto_table); i++)
    if (cdp1802_elf_howto_table[i].name != NULL
	&& strcasecmp (cdp1802_elf_howto_table[i].name, r_name) == 0)
      return &cdp1802_elf_howto_table[i];

  return NULL;
}

static bool
cdp1802_info_to_howto_rela (bfd *abfd, arelent *cache_ptr, Elf_Internal_Rela *dst)
{
  unsigned int r_type;

  r_type = ELF32_R_TYPE (dst->r_info);
  if (r_type >= (unsigned int) R_CDP1802_max)
    {
      /* xgettext:c-format */
      _bfd_error_handler (_("%pB: unsupported relocation type %#x"),
			  abfd, r_type);
      bfd_set_error (bfd_error_bad_value);
      return false;
    }
  cache_ptr->howto = &cdp1802_elf_howto_table[r_type];
  return true;
}

/* Return the section that should be marked against GC for a given
   relocation.  */

static asection *
cdp1802_gc_mark_hook (asection *sec,
		      struct bfd_link_info *info,
		      struct elf_reloc_cookie *cookie,
		      struct elf_link_hash_entry *h,
		      unsigned int symndx)
{
  if (h != NULL)
    switch (ELF32_R_TYPE (cookie->rel->r_info))
      {
      case R_CDP1802_GNU_VTINHERIT:
      case R_CDP1802_GNU_VTENTRY:
	return NULL;
      }

  return _bfd_elf_gc_mark_hook (sec, info, cookie, h, symndx);
}

/* Look through the relocs for a section during the first phase.
   Since we don't do .gots or .plts, we just need to consider the
   virtual table relocs for gc.  */

static bool
cdp1802_check_relocs (bfd *abfd,
		      struct bfd_link_info *info,
		      asection *sec,
		      const Elf_Internal_Rela *relocs)
{
  Elf_Internal_Shdr *symtab_hdr;
  struct elf_link_hash_entry **sym_hashes;
  const Elf_Internal_Rela *rel;
  const Elf_Internal_Rela *rel_end;

  if (bfd_link_relocatable (info))
    return true;

  symtab_hdr = &elf_tdata (abfd)->symtab_hdr;
  sym_hashes = elf_sym_hashes (abfd);
  rel_end = relocs + sec->reloc_count;

  for (rel = relocs; rel < rel_end; rel++)
    {
      struct elf_link_hash_entry *h;
      unsigned long r_symndx;

      r_symndx = ELF32_R_SYM (rel->r_info);

      if (r_symndx < symtab_hdr->sh_info)
	h = NULL;
      else
	{
	  h = sym_hashes[r_symndx - symtab_hdr->sh_info];
	  while (h->root.type == bfd_link_hash_indirect
		 || h->root.type == bfd_link_hash_warning)
	    h = (struct elf_link_hash_entry *) h->root.u.i.link;
	}

      switch (ELF32_R_TYPE (rel->r_info))
	{
	/* This relocation describes the C++ object vtable hierarchy.
	   Reconstruct it for later use during GC.  */
	case R_CDP1802_GNU_VTINHERIT:
	  if (!bfd_elf_gc_record_vtinherit (abfd, sec, h, rel->r_offset))
	    return false;
	  break;

	/* This relocation describes which C++ vtable entries are actually
	   used.  Record for later use during GC.  */
	case R_CDP1802_GNU_VTENTRY:
	  if (!bfd_elf_gc_record_vtentry (abfd, sec, h, rel->r_offset))
	    return false;
	  break;
	}
    }

  return true;
}

/* Relocate a CDP1802 ELF section.  */

static int
cdp1802_relocate_section (bfd *output_bfd,
			  struct bfd_link_info *info,
			  bfd *input_bfd,
			  asection *input_section,
			  bfd_byte *contents,
			  Elf_Internal_Rela *relocs,
			  Elf_Internal_Sym *local_syms,
			  asection **local_sections)
{
  Elf_Internal_Shdr *symtab_hdr;
  struct elf_link_hash_entry **sym_hashes;
  Elf_Internal_Rela *rel, *relend;

  symtab_hdr = &elf_tdata (input_bfd)->symtab_hdr;
  sym_hashes = elf_sym_hashes (input_bfd);
  relend = relocs + input_section->reloc_count;

  for (rel = relocs; rel < relend; rel++)
    {
      int r_type;
      reloc_howto_type *howto;
      unsigned long r_symndx;
      Elf_Internal_Sym *sym;
      asection *sec;
      struct elf_link_hash_entry *h;
      bfd_vma relocation;
      bfd_reloc_status_type r;

      r_symndx = ELF32_R_SYM (rel->r_info);
      r_type = ELF32_R_TYPE (rel->r_info);

      if (r_type == R_CDP1802_GNU_VTENTRY
	  || r_type == R_CDP1802_GNU_VTINHERIT)
	continue;

      howto = cdp1802_elf_howto_table + r_type;
      h = NULL;
      sym = NULL;
      sec = NULL;

      if (r_symndx < symtab_hdr->sh_info)
	{
	  sym = local_syms + r_symndx;
	  sec = local_sections[r_symndx];
	  relocation = _bfd_elf_rela_local_sym (output_bfd, sym, &sec, rel);
	}
      else
	{
	  bool unresolved_reloc, warned, ignored;

	  RELOC_FOR_GLOBAL_SYMBOL (info, input_bfd, input_section, rel,
				   r_symndx, symtab_hdr, sym_hashes,
				   h, sec, relocation,
				   unresolved_reloc, warned, ignored);
	}

      if (sec != NULL && discarded_section (sec))
	RELOC_AGAINST_DISCARDED_SECTION (info, input_bfd, input_section,
					 rel, 1, relend, R_CDP1802_NONE,
					 howto, 0, contents);

      if (bfd_link_relocatable (info))
	continue;

      r = _bfd_final_link_relocate (howto, input_bfd, input_section,
				    contents, rel->r_offset,
				    relocation, rel->r_addend);

      if (r != bfd_reloc_ok)
	{
	  const char *name;
	  const char *msg = (const char *) 0;

	  if (h != NULL)
	    name = h->root.root.string;
	  else
	    {
	      name = bfd_elf_string_from_elf_section
		      (input_bfd, symtab_hdr->sh_link, sym->st_name);
	      if (name == NULL || *name == '\0')
		name = bfd_section_name (sec);
	    }

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
	      msg = _("internal error: dangerous error");
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

#define ELF_ARCH			bfd_arch_cdp1802
#define ELF_MACHINE_CODE		EM_CDP1802
#define ELF_MAXPAGESIZE			256

#define TARGET_BIG_NAME			"elf32-cdp1802"
#define TARGET_BIG_SYM			cdp1802_elf32_vec

#define elf_backend_can_gc_sections	1
#define elf_backend_check_relocs	cdp1802_check_relocs
#define elf_backend_gc_mark_hook	cdp1802_gc_mark_hook
#define elf_backend_rela_normal		1
#define elf_backend_relocate_section	cdp1802_relocate_section

#define elf_info_to_howto		cdp1802_info_to_howto_rela

#define bfd_elf32_bfd_reloc_type_lookup	cdp1802_reloc_type_lookup
#define bfd_elf32_bfd_reloc_name_lookup	cdp1802_reloc_name_lookup

#include "elf32-target.h"
