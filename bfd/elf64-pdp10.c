/* ELF support for PDP-10.
   Copyright (C) 2025 Free Software Foundation, Inc.
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

static unsigned int
pdp10_bfd_get_9 (bfd *abfd ATTRIBUTE_UNUSED, bfd_byte *addr)
{
  unsigned char hi, lo;

  hi = bfd_get_8 (abfd, addr);
  lo = bfd_get_8 (abfd, addr + 1);
  return ((hi & 1) << 8) | lo;
}

static void
pdp10_bfd_put_9 (bfd *abfd ATTRIBUTE_UNUSED, bfd_vma val, bfd_byte *addr)
{
  bfd_put_8 (abfd, (val >> 8) & 1, addr);
  bfd_put_8 (abfd, val & 0xff, addr + 1);
}

static uint32_t
pdp10_bfd_get_18 (bfd *abfd, bfd_byte *addr)
{
  unsigned int hi, lo;

  hi = pdp10_bfd_get_9 (abfd, addr);
  lo = pdp10_bfd_get_9 (abfd, addr + 2);
  return ((uint32_t) hi << 9) | lo;
}

static void
pdp10_bfd_put_18 (bfd *abfd, bfd_vma val, bfd_byte *addr)
{
  pdp10_bfd_put_9 (abfd, val >> 9, addr);
  pdp10_bfd_put_9 (abfd, val, addr + 2);
}

static uint64_t
pdp10_bfd_get_36 (bfd *abfd, bfd_byte *addr)
{
  uint32_t hi, lo;

  hi = pdp10_bfd_get_18 (abfd, addr);
  lo = pdp10_bfd_get_18 (abfd, addr + 4);
  return ((uint64_t) hi << 18) | lo;
}

static void
pdp10_bfd_put_36 (bfd *abfd, bfd_vma val, bfd_byte *addr)
{
  pdp10_bfd_put_18 (abfd, val >> 18, addr);
  pdp10_bfd_put_18 (abfd, val, addr + 4);
}

static bfd_vma
pdp10_read_reloc (bfd *abfd, bfd_byte *addr, reloc_howto_type *howto)
{
  switch (bfd_get_reloc_size (howto))
    {
    case 4:
      return pdp10_bfd_get_18 (abfd, addr);
    case 8:
      return pdp10_bfd_get_36 (abfd, addr);
    default:
      abort ();
    }
}

static void
pdp10_write_reloc (bfd *abfd, bfd_vma val, bfd_byte *addr, reloc_howto_type *howto)
{
  switch (bfd_get_reloc_size (howto))
    {
    case 4:
      pdp10_bfd_put_18 (abfd, val, addr);
      break;
    case 8:
      pdp10_bfd_put_36 (abfd, val, addr);
      break;
    default:
      abort ();
    }
}

static bfd_vma
pdp10_read_bptr (bfd *abfd, bfd_byte *addr, reloc_howto_type *howto ATTRIBUTE_UNUSED)
{
  uint64_t word;
  uint32_t y;
  unsigned int ps;

  word = pdp10_bfd_get_36 (abfd, addr);
  y = word & ((1UL << 30) - 1);
  ps = (word >> 30) & ((1U << 6) - 1);
  BFD_ASSERT ((ps & 074) == 070);
  return (uint64_t) y * 8 + (ps & 3) * 2;
}

static void
pdp10_write_bptr (bfd *abfd, bfd_vma val, bfd_byte *addr, reloc_howto_type *howto ATTRIBUTE_UNUSED)
{
  uint64_t word;
  uint32_t y;
  unsigned int ps;

  /* A valid nonet address is 33 bits and even.  */
  BFD_ASSERT ((val & ~0x1fffffffeULL) == 0);

  ps = 070 + ((val >> 1) & 3);
  y = val >> 3;
  word = ((uint64_t) ps << 30) | y;
  pdp10_bfd_put_36 (abfd, word, addr);
}

static bfd_vma
pdp10_read_hptr (bfd *abfd, bfd_byte *addr, reloc_howto_type *howto ATTRIBUTE_UNUSED)
{
  uint64_t word;
  uint32_t y;
  unsigned int ps;

  word = pdp10_bfd_get_36 (abfd, addr);
  y = word & ((1UL << 30) - 1);
  ps = (word >> 30) & ((1U << 6) - 1);
  BFD_ASSERT (ps == 075 || ps == 076);
  return (uint64_t) y * 8 + (ps & 2) * 2;
}

static void
pdp10_write_hptr (bfd *abfd, bfd_vma val, bfd_byte *addr, reloc_howto_type *howto ATTRIBUTE_UNUSED)
{
  uint64_t word;
  uint32_t y;
  unsigned int ps;

  /* A valid half-word address is 33 bits and a multiple of 4.  */
  BFD_ASSERT ((val & ~0x1fffffffcULL) == 0);

  ps = 075 + ((val >> 2) & 1);
  y = val >> 3;
  word = ((uint64_t) ps << 30) | y;
  pdp10_bfd_put_36 (abfd, word, addr);
}

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
	 8,			/* size */
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
	 8,			/* size */
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

  /* An 8 bit absolute relocation.  */
  HOWTO (R_PDP10_8,		/* type */
	 0,			/* rightshift */
	 1,			/* size */
	 8,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield,	/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_PDP10_8",		/* name */
	 false,			/* partial_inplace */
	 0x00,			/* src_mask */
	 0xff,			/* dst_mask */
	 false),		/* pcrel_offset */

  /* A 16 bit absolute relocation.  */
  HOWTO (R_PDP10_16,		/* type */
	 0,			/* rightshift */
	 2,			/* size */
	 16,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield,	/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_PDP10_16",		/* name */
	 false,			/* partial_inplace */
	 0x0000,		/* src_mask */
	 0xffff,		/* dst_mask */
	 false),		/* pcrel_offset */

  /* A 32 bit absolute relocation.  */
  HOWTO (R_PDP10_32,		/* type */
	 0,			/* rightshift */
	 4,			/* size */
	 32,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield,	/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_PDP10_32",		/* name */
	 false,			/* partial_inplace */
	 0x00000000,		/* src_mask */
	 0xffffffff,		/* dst_mask */
	 false),		/* pcrel_offset */

  /* A 64 bit absolute relocation.  */
  HOWTO (R_PDP10_64,		/* type */
	 0,			/* rightshift */
	 8,			/* size */
	 64,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield,	/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_PDP10_64",		/* name */
	 false,			/* partial_inplace */
	 0x0000000000000000,	/* src_mask */
	 0xffffffffffffffff,	/* dst_mask */
	 false),		/* pcrel_offset */

  /* A 9 bit in two octets absolute relocation.  */
  HOWTO15 (R_PDP10_9,		/* type */
	 0,			/* rightshift */
	 2,			/* size */
	 16,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield,	/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_PDP10_9",		/* name */
	 false,			/* partial_inplace */
	 0x0000,		/* src_mask */
	 0xffff,		/* dst_mask */
	 false,			/* pcrel_offset */
	 pdp10_read_reloc,	/* read_reloc */
	 pdp10_write_reloc),	/* write_reloc */

  /* An 18 bit in four octets absolute relocation.  */
  HOWTO15 (R_PDP10_18,		/* type */
	 0,			/* rightshift */
	 4,			/* size */
	 32,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield,	/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_PDP10_18",		/* name */
	 false,			/* partial_inplace */
	 0x00000000,		/* src_mask */
	 0xffffffff,		/* dst_mask */
	 false,			/* pcrel_offset */
	 pdp10_read_reloc,	/* read_reloc */
	 pdp10_write_reloc),	/* write_reloc */

  /* A 36 bit in eight octets absolute relocation.  */
  HOWTO15 (R_PDP10_36,		/* type */
	 0,			/* rightshift */
	 8,			/* size */
	 64,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield,	/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_PDP10_18",		/* name */
	 false,			/* partial_inplace */
	 0x0000000000000000,	/* src_mask */
	 0xffffffffffffffff,	/* dst_mask */
	 false,			/* pcrel_offset */
	 pdp10_read_reloc,	/* read_reloc */
	 pdp10_write_reloc),	/* write_reloc */

  /* A local word adress in an "instruction format indirect word".  */
  HOWTO15 (R_PDP10_IFIW,	/* type */
	 3,			/* rightshift */
	 4,			/* size */
	 32,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_PDP10_IFIW",	/* name */
	 false,			/* partial_inplace */
	 0x00000000,		/* src_mask */
	 0xffffffff,		/* dst_mask */
	 false,			/* pcrel_offset */
	 pdp10_read_reloc,	/* read_reloc */
	 pdp10_write_reloc),	/* write_reloc */

  /* A global word adress in an "extended format indirect word".  */
  HOWTO15 (R_PDP10_EFIW,	/* type */
	 3,			/* rightshift */
	 8,			/* size */
	 64,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_PDP10_EFIW",	/* name */
	 false,			/* partial_inplace */
	 0x0000000000000000,	/* src_mask */
	 0xffffffffffffffff,	/* dst_mask */
	 false,			/* pcrel_offset */
	 pdp10_read_reloc,	/* read_reloc */
	 pdp10_write_reloc),	/* write_reloc */

  /* A global 9-bit byte pointer.  */
  HOWTO15 (R_PDP10_BPTR,	/* type */
	 0,			/* rightshift */
	 8,			/* size */
	 64,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_PDP10_BPTR",	/* name */
	 false,			/* partial_inplace */
	 0x0000000000000000,	/* src_mask */
	 0xffffffffffffffff,	/* dst_mask */
	 false,			/* pcrel_offset */
	 pdp10_read_bptr,	/* read_reloc */
	 pdp10_write_bptr),	/* write_reloc */

  /* A global half-word pointer (18-bit byte pointer).  */
  HOWTO15 (R_PDP10_HPTR,	/* type */
	 0,			/* rightshift */
	 8,			/* size */
	 64,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_PDP10_HPTR",	/* name */
	 false,			/* partial_inplace */
	 0x0000000000000000,	/* src_mask */
	 0xffffffffffffffff,	/* dst_mask */
	 false,			/* pcrel_offset */
	 pdp10_read_hptr,	/* read_reloc */
	 pdp10_write_hptr),	/* write_reloc */
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
    case BFD_RELOC_8:
      r = R_PDP10_8;
      break;
    case BFD_RELOC_16:
      r = R_PDP10_16;
      break;
    case BFD_RELOC_32:
      r = R_PDP10_32;
      break;
    case BFD_RELOC_64:
      r = R_PDP10_64;
      break;
    case BFD_RELOC_PDP10_9:
      r = R_PDP10_9;
      break;
    case BFD_RELOC_PDP10_18:
      r = R_PDP10_18;
      break;
    case BFD_RELOC_PDP10_36:
      r = R_PDP10_36;
      break;
    case BFD_RELOC_PDP10_IFIW:
      r = R_PDP10_IFIW;
      break;
    case BFD_RELOC_PDP10_EFIW:
      r = R_PDP10_EFIW;
      break;
    case BFD_RELOC_PDP10_BPTR:
      r = R_PDP10_BPTR;
      break;
    case BFD_RELOC_PDP10_HPTR:
      r = R_PDP10_HPTR;
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
pdp10_info_to_howto_rela (bfd *abfd ATTRIBUTE_UNUSED, arelent *cache_ptr, Elf_Internal_Rela * dst)
{
  unsigned int r_type;

  r_type = ELF64_R_TYPE (dst->r_info);
  if (r_type >= (unsigned int) R_PDP10_max)
    {
      /* xgettext:c-format */
      _bfd_error_handler (_("%pB: unsupported relocation type %#x"),
			  abfd, r_type);
      bfd_set_error (bfd_error_bad_value);
      return false;
    }
  cache_ptr->howto = &pdp10_elf_howto_table[r_type];
  return true;
}

/* Return the section that should be marked against GC for a given
   relocation.  */

static asection *
pdp10_gc_mark_hook (asection *sec,
		    struct bfd_link_info *info,
		    struct elf_reloc_cookie *cookie,
		    struct elf_link_hash_entry *h,
		    unsigned int symndx)
{
  if (h != NULL)
    switch (ELF64_R_TYPE (cookie->rel->r_info))
      {
      case R_PDP10_GNU_VTINHERIT:
      case R_PDP10_GNU_VTENTRY:
	return NULL;
      }

  return _bfd_elf_gc_mark_hook (sec, info, cookie, h, symndx);
}

/* Look through the relocs for a section during the first phase.
   Since we don't do .gots or .plts, we just need to consider the
   virtual table relocs for gc.  */

static bool
pdp10_check_relocs (bfd *abfd,
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

      r_symndx = ELF64_R_SYM (rel->r_info);

      if (r_symndx < symtab_hdr->sh_info)
	h = NULL;
      else
	{
	  h = sym_hashes[r_symndx - symtab_hdr->sh_info];
	  while (h->root.type == bfd_link_hash_indirect
		 || h->root.type == bfd_link_hash_warning)
	    h = (struct elf_link_hash_entry *) h->root.u.i.link;
	}

      switch (ELF64_R_TYPE (rel->r_info))
	{
	/* This relocation describes the C++ object vtable hierarchy.
	   Reconstruct it for later use during GC.  */
	case R_PDP10_GNU_VTINHERIT:
	  if (!bfd_elf_gc_record_vtinherit (abfd, sec, h, rel->r_offset))
	    return false;
	  break;

	/* This relocation describes which C++ vtable entries are actually
	   used.  Record for later use during GC.  */
	case R_PDP10_GNU_VTENTRY:
	  if (!bfd_elf_gc_record_vtentry (abfd, sec, h, rel->r_offset))
	    return false;
	  break;
	}
    }

  return true;
}

/* Relocate a PDP-10 ELF section.  */

static int
pdp10_relocate_section (bfd *output_bfd,
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

      r_symndx = ELF64_R_SYM (rel->r_info);
      r_type = ELF64_R_TYPE (rel->r_info);

      if (r_type == R_PDP10_GNU_VTENTRY
	  || r_type == R_PDP10_GNU_VTINHERIT)
	continue;

      howto = pdp10_elf_howto_table + r_type;
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
					 rel, 1, relend, R_PDP10_NONE,
					 howto, 0, contents);

      if (bfd_link_relocatable (info))
	{
#if 0 /* TODO: some targets do this, some do not. */
	  /* This is a relocatable link.  We don't have to change
	     anything, unless the reloc is against a section symbol,
	     in which case we have to adjust according to where the
	     section symbol winds up in the output section.  */
	  if (sym != NULL && ELF_ST_TYPE (sym->st_info) == STT_SECTION)
	    rel->r_addend += sec->output_offset;
#endif
	  continue;
	}

      r = _bfd_final_link_relocate (howto, input_bfd, input_section,
				    contents, rel->r_offset,
				    relocation, rel->r_addend);

      if (r != bfd_reloc_ok)
	{
	  const char *name;
	  const char *msg = NULL;

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

#define ELF_ARCH			bfd_arch_pdp10
#define ELF_MACHINE_CODE		EM_PDP10
#define ELF_MAXPAGESIZE			(512 * 4 * 2)	/* 512 words, 4 nonets per word, two octets per nonet */

#define TARGET_BIG_NAME			"elf64-pdp10"
#define TARGET_BIG_SYM			pdp10_elf64_vec

#define elf_backend_can_gc_sections	1
#define elf_backend_check_relocs	pdp10_check_relocs
#define elf_backend_gc_mark_hook	pdp10_gc_mark_hook
#define elf_backend_rela_normal		1
#define elf_backend_relocate_section	pdp10_relocate_section

#define elf_info_to_howto		pdp10_info_to_howto_rela

#define bfd_elf64_bfd_reloc_type_lookup	pdp10_reloc_type_lookup
#define bfd_elf64_bfd_reloc_name_lookup pdp10_reloc_name_lookup

#include "elf64-target.h"
