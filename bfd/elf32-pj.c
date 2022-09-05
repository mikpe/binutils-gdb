/* picoJava specific support for 32-bit ELF
   Copyright 1999 Free Software Foundation, Inc.
   Contributed by Steve Chamberlan of Transmeta (sac@pobox.com).

This file is part of BFD, the Binary File Descriptor library.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */


#include "bfd.h"
#include "sysdep.h"
#include "bfdlink.h"
#include "libbfd.h"
#include "elf-bfd.h"
#include "elf/pj.h"
#include "opcode/pj.h"

/* Section used to accumulate thunk code.  */
static asection *thunk_section;

/* Number of thunks processed.  */
static int thunk_done;

/* Template for a single thunk.  */
static bfd_byte thunk_chunk[] =
{
  0xff, FF_WRITE_GLOBAL1, 
  0xff, FF_WRITE_GLOBAL2,
  BIPUSH,   0x00,
  0xff, FF_READ_OPTOP,
  IADD,
  0xff, FF_WRITE_OPTOP,
  0xff, FF_READ_GLOBAL2,
  0xff, FF_READ_GLOBAL1,
  GOTO_W, 0x00, 0x00, 0x00, 0x00
};

static reloc_howto_type *pj_elf_reloc_type_lookup
  PARAMS ((bfd *, bfd_reloc_code_real_type));

static void pj_elf_info_to_howto
  PARAMS ((bfd *, arelent *, Elf_Internal_Rela *));

static reloc_howto_type pj_elf_howto_table[] =
{
  /* No relocation.  */
  HOWTO (R_PJ_NONE,		/* type */
	 0,			/* rightshift */
	 0,			/* size (0 = byte, 1 = short, 2 = long) */
	 0,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 bfd_elf_generic_reloc, /* special_function */
	 "R_PJ_NONE",		/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0,			/* dst_mask */
	 false),		/* pcrel_offset */

  /* 32 bit absolute relocation. */
  HOWTO (R_PJ_DATA_DIR32,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 32,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_PJ_DATA_DIR32",	/* name */
	 false,			/* partial_inplace */
	 0xffffffff,		/* src_mask */
	 0xffffffff,		/* dst_mask */
	 false),		/* pcrel_offset */

  /* 32 bit PC relative relocation.  */
  HOWTO (R_PJ_CODE_REL32,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 32,			/* bitsize */
	 true,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_PJ_CODE_REL32",	/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0xffffffff,		/* dst_mask */
	 true),			/* pcrel_offset */

  /* 16 bit PC relative relocation.  */
  HOWTO (R_PJ_CODE_REL16,	/* type */
	 0,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 true,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_PJ_CODE_REL16",	/* name */
	 false,			/* partial_inplace */
	 0xffff,		/* src_mask */
	 0xffff,		/* dst_mask */
	 true),			/* pcrel_offset */

  HOWTO (R_PJ_CODE_DIR8,	/* type */
	 0,			/* rightshift */
	 0,			/* size (0 = byte, 1 = short, 2 = long) */
	 8,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_PJ_CODE_DIR8",	/* name */
	 false,			/* partial_inplace */
	 0xff,			/* src_mask */
	 0xff,			/* dst_mask */
	 false),		/* pcrel_offset */

  EMPTY_HOWTO (5),

  HOWTO (R_PJ_CODE_DIR32,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 32,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_PJ_CODE_DIR32",	/* name */
	 true,			/* partial_inplace */
	 0xffffffff,		/* src_mask */
	 0xffffffff,		/* dst_mask */
	 false),		/* pcrel_offset */

  EMPTY_HOWTO (7),
  EMPTY_HOWTO (8),
  EMPTY_HOWTO (9),
  EMPTY_HOWTO (10),
  EMPTY_HOWTO (11),
  EMPTY_HOWTO (12),

  /* Low 16 bits of a 32 bit word relocation.  Most often used for sipush. */
  HOWTO (R_PJ_CODE_LO16,	/* type */
	 0,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_unsigned, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_PJ_CODE_LO16",	/* name */
	 false,			/* partial_inplace */
	 0xffff,		/* src_mask */
	 0xffff,		/* dst_mask */
	 true),			/* pcrel_offset */

  /* High 16 bits of a 32 bit word relocation.  Most often used for sethi. */
  HOWTO (R_PJ_CODE_HI16,	/* type */
	 16,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_unsigned, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_PJ_CODE_HI16",	/* name */
	 false,			/* partial_inplace */
	 0xffff,		/* src_mask */
	 0xffff,		/* dst_mask */
	 true),			/* pcrel_offset */

  /* GNU extension to record C++ vtable hierarchy */
  HOWTO (R_PJ_GNU_VTINHERIT,    /* type */
         0,                     /* rightshift */
         2,                     /* size (0 = byte, 1 = short, 2 = long) */
         0,                     /* bitsize */
         false,                 /* pc_relative */
         0,                     /* bitpos */
         complain_overflow_dont, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
         "R_PJ_GNU_VTINHERIT",  /* name */
         false,                 /* partial_inplace */
         0,                     /* src_mask */
         0,                     /* dst_mask */
         false),                /* pcrel_offset */

  /* GNU extension to record C++ vtable member usage */
  HOWTO (R_PJ_GNU_VTENTRY,     /* type */
         0,                     /* rightshift */
         2,                     /* size (0 = byte, 1 = short, 2 = long) */
         0,                     /* bitsize */
         false,                 /* pc_relative */
         0,                     /* bitpos */
         complain_overflow_dont, /* complain_on_overflow */
         _bfd_elf_rel_vtable_reloc_fn,  /* special_function */
         "R_PJ_GNU_VTENTRY",   	/* name */
         false,                 /* partial_inplace */
         0,                     /* src_mask */
         0,                     /* dst_mask */
         false),                /* pcrel_offset */

  /* GNU extension to handle relaxing safe linkage. */
  HOWTO (R_PJ_WORDS_PUSHED,     /* type */
         0,                     /* rightshift */
         2,                     /* size (0 = byte, 1 = short, 2 = long) */
         0,                     /* bitsize */
         false,                 /* pc_relative */
         0,                     /* bitpos */
         complain_overflow_dont, /* complain_on_overflow */
         bfd_elf_generic_reloc,  /* special_function */
         "R_PJ_WORDS_PUSHED",   /* name */
         false,                 /* partial_inplace */
         0,                     /* src_mask */
         0,                     /* dst_mask */
         false),                /* pcrel_offset */

  /*  Relocation for a sipush/sethi pair. */
  HOWTO (R_PJ_CODE_HILO,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 0,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_unsigned, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_PJ_CODE_HILO",	/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0,			/* dst_mask */
	 true),			/* pcrel_offset */
};


struct elf_reloc_map
{
  bfd_reloc_code_real_type bfd_reloc_val;
  unsigned char elf_reloc_val;
};

/* An array mapping BFD reloc codes to PJ ELF relocs.  */

static const struct elf_reloc_map pj_reloc_map[] =
{
    { BFD_RELOC_NONE, 		R_PJ_NONE          },
    { BFD_RELOC_32, 		R_PJ_DATA_DIR32    },
    { BFD_RELOC_PJ_CODE_DIR16, 	R_PJ_CODE_DIR16    },
    { BFD_RELOC_8,	 	R_PJ_CODE_DIR8     },
    { BFD_RELOC_PJ_CODE_DIR32, 	R_PJ_CODE_DIR32    },
    { BFD_RELOC_PJ_CODE_LO16, 	R_PJ_CODE_LO16     },
    { BFD_RELOC_PJ_CODE_HI16, 	R_PJ_CODE_HI16     },
    { BFD_RELOC_PJ_CODE_REL32,  R_PJ_CODE_REL32    },
    { BFD_RELOC_PJ_CODE_REL16,  R_PJ_CODE_REL16    },
    { BFD_RELOC_VTABLE_INHERIT, R_PJ_GNU_VTINHERIT },
    { BFD_RELOC_VTABLE_ENTRY,   R_PJ_GNU_VTENTRY   },
    { BFD_RELOC_PJ_WORDS_PUSHED,R_PJ_WORDS_PUSHED  },
    { BFD_RELOC_PJ_CODE_HILO,   R_PJ_CODE_HILO     },
};

/* Given a BFD reloc code, return the howto structure for the
   corresponding PJ ELf reloc.  */

static reloc_howto_type *
bfd_elf_generic_reloc_type_lookup (abfd, code)
     bfd *abfd ATTRIBUTE_UNUSED;
     bfd_reloc_code_real_type code;
{
  unsigned int i;

  for (i = 0; i < sizeof (pj_reloc_map) / sizeof (struct elf_reloc_map); i++)
    {
      if (pj_reloc_map[i].bfd_reloc_val == code)
	return &pj_elf_howto_table[(int) pj_reloc_map[i].elf_reloc_val];
    }

  return NULL;
}

/* Given an ELF reloc, fill in the howto field of a relent.  */

static void
pj_elf_info_to_howto (abfd, cache_ptr, dst)
     bfd *abfd ATTRIBUTE_UNUSED;
     arelent *cache_ptr;
     Elf_Internal_Rela *dst;
{
  unsigned int r = ELF32_R_TYPE (dst->r_info);

  BFD_ASSERT (r < (unsigned int) R_PJ_max);

  cache_ptr->howto = &pj_elf_howto_table[r];
}

/* Take this moment to fill in the special picoJava bits in the
   e_flags field. */

static void
pj_elf_final_write_processing (abfd, linker)
     bfd *abfd;
     boolean linker ATTRIBUTE_UNUSED;
{
  elf_elfheader (abfd)->e_flags |= EF_PICOJAVA_ARCH;
  elf_elfheader (abfd)->e_flags |= EF_PICOJAVA_GNUCALLS;
}

static const char *
symbol_name (h, abfd, sec, symtab_hdr, sym)
     struct elf_link_hash_entry *h;
     bfd *abfd;
     asection *sec;
     Elf_Internal_Shdr *symtab_hdr;
     Elf_Internal_Sym *sym;
{
  const char *name;
  if (h != NULL)
    name = h->root.root.string;
  else
    {
      name = bfd_elf_string_from_elf_section
	(abfd, symtab_hdr->sh_link, sym->st_name);
      if (name == NULL || *name == '\0')
	name = bfd_section_name (input_bfd, sec);
    }
  return name;
}

static boolean
pj_elf_relocate_section (output_bfd, info, input_bfd, input_section,
			 contents, rel, local_syms, local_sections)
     bfd *output_bfd;
     struct bfd_link_info *info;
     bfd *input_bfd;
     asection *input_section;
     bfd_byte *contents;
     Elf_Internal_Rela *rel;
     Elf_Internal_Sym *local_syms;
     asection **local_sections;
{
  static bfd_byte *thunk_target;
  Elf_Internal_Shdr *symtab_hdr;
  struct elf_link_hash_entry **sym_hashes;
  Elf_Internal_Rela *relend;

  symtab_hdr = &elf_tdata (input_bfd)->symtab_hdr;
  sym_hashes = (struct elf_link_hash_entry **) (elf_sym_hashes (input_bfd));

  if (thunk_section && !thunk_section->contents && thunk_section->_raw_size)
    thunk_section->contents
      = (bfd_byte *) bfd_malloc (thunk_section->_raw_size);

  relend = rel + input_section->reloc_count;
  for (; rel < relend; rel++)
    {
      int r_type;
      reloc_howto_type *howto;
      unsigned long r_symndx;
      Elf_Internal_Sym *sym;
      asection *sec;
      struct elf_link_hash_entry *h;
      bfd_vma relocation;
      bfd_reloc_status_type r = bfd_reloc_ok;
      int callee_consumes = 0;
      unsigned long insn;
      bfd_vma addr = rel->r_offset;
      bfd_vma addend = rel->r_addend;
      bfd_byte *hit_data = addr + (bfd_byte *) contents;

      r_symndx = ELF32_R_SYM (rel->r_info);
      r_type = ELF32_R_TYPE (rel->r_info);
      howto = pj_elf_howto_table + r_type;

      /* Just skip the vtable gc relocs and nulled out relocations.  */
      if (r_type == R_PJ_GNU_VTINHERIT
	  || r_type == R_PJ_GNU_VTENTRY || r_type == R_PJ_NONE)
	continue;

      if (info->relocateable)
	{
	  /* This is a relocateable link.  We don't have to change
	     anything, unless the reloc is against a section symbol,
	     in which case we have to adjust according to where the
	     section symbol winds up in the output section.  */
	  if (r_symndx < symtab_hdr->sh_info)
	    {
	      sym = local_syms + r_symndx;
	      if (ELF_ST_TYPE (sym->st_info) == STT_SECTION)
		{
		  sec = local_sections[r_symndx];
		  rel->r_addend = addend + sec->output_offset + sym->st_value;
		}
	    }
	  continue;
	}

      /* This is a final link.  */
      h = NULL;
      sym = NULL;
      sec = NULL;
      if (r_symndx < symtab_hdr->sh_info)
	{
	  sym = local_syms + r_symndx;
	  sec = local_sections[r_symndx];
	  relocation = (sec->output_section->vma
			+ sec->output_offset
			+ sym->st_value);
	  callee_consumes = sym->st_other;
	}
      else
	{
	  h = sym_hashes[r_symndx - symtab_hdr->sh_info];
	  while (h->root.type == bfd_link_hash_indirect
		 || h->root.type == bfd_link_hash_warning)
	    h = (struct elf_link_hash_entry *) h->root.u.i.link;
	  if (h->root.type == bfd_link_hash_defined
	      || h->root.type == bfd_link_hash_defweak)
	    {
	      sec = h->root.u.def.section;
	      relocation = (h->root.u.def.value
			    + sec->output_section->vma
			    + sec->output_offset);
	      callee_consumes = h->other;
	    }
	  else if (h->root.type == bfd_link_hash_undefweak)
	    relocation = 0;
	  else
	    {
	      if (!((*info->callbacks->undefined_symbol)
		    (info, h->root.root.string, input_bfd,
		     input_section, addr, true)))
		return false;
	      relocation = 0;
	    }
	}

      switch (r_type)
	{
	  /* Data relocations are endian dependent.  */
	case R_PJ_DATA_DIR32:
	  insn = bfd_get_32 (input_bfd, hit_data);
	  insn += relocation + addend;
	  bfd_put_32 (input_bfd, insn, hit_data);
	  break;

	  /* Relocations in code are always big endian, no matter what the
	     data endianness is. */

	case R_PJ_CODE_DIR32:
	  insn = bfd_getb32 (hit_data);
	  insn += relocation + addend;
	  bfd_putb32 (insn, hit_data);
	  break;

	case R_PJ_CODE_REL16:
	  insn = bfd_getb16 (hit_data);
	  insn += relocation + addend
	    - (input_section->output_section->vma
	       + input_section->output_offset + addr - 1);
	  bfd_putb16 (insn, hit_data);
	  if (insn < -0x7fff || insn > 0x7fff)
	    r = bfd_reloc_overflow;
	  break;

	case R_PJ_CODE_REL32:
	  insn = bfd_getb32 (hit_data);
	  insn += relocation + addend
	    - (input_section->output_section->vma
	       + input_section->output_offset + addr - 1);
	  bfd_putb32 (insn, hit_data);
	  break;

	case R_PJ_CODE_LO16:
	  insn = bfd_getb16 (hit_data);
	  insn += relocation + addend;
	  bfd_putb16 (insn, hit_data);
	  break;

	case R_PJ_CODE_HI16:
	  insn = bfd_getb16 (hit_data);
	  insn += (relocation + addend) >> 16;
	  bfd_putb16 (insn, hit_data);
	  break;

	case R_PJ_WORDS_PUSHED:
	  /* Check to make sure that the number of words pushed
	     matches the number of words expected by the callee.  The
	     push count is stored in the relocation's addend field.
	     The high bit of the callee_consumes byte signifies a
	     varargs callee, which doesn't need to get fixed up. */

	  if (sec && thunk_section
	      && callee_consumes != 0
	      && !(callee_consumes & 0x80)
	      && callee_consumes != addend)
	    {
	      bfd_vma offset_into_thunks = thunk_done * sizeof (thunk_chunk);
	      bfd_byte *pc = thunk_section->contents + offset_into_thunks;

	      bfd_vma thunk_addr
		= offset_into_thunks
		+ thunk_section->output_offset
		+ thunk_section->output_section->vma;

	      memcpy (pc, thunk_chunk, sizeof (thunk_chunk));
	      pc[5] = (addend - callee_consumes + 1) * 4;
	      bfd_putb32 (relocation - thunk_addr - 15, pc + 16);

	      /* Change the next hilo reloc. */
	      rel++;

	      bfd_putb16 (thunk_addr, rel->r_offset + contents);
	      bfd_putb16 (thunk_addr >> 16, rel->r_offset + contents + 3);

	      thunk_done++;
	    }
	  break;

	case R_PJ_CODE_HILO:
	  insn = (bfd_getb16 (hit_data) | (bfd_getb16 (hit_data + 3) << 16))
	    + relocation + addend;
	  bfd_putb16 (insn, hit_data);
	  bfd_putb16 (insn >> 16, hit_data + 3);
	  break;

	case R_PJ_NONE:
	  break;

	default:
	  r = bfd_reloc_notsupported;
	}


      if (r != bfd_reloc_ok)
	{
	  const char *name;
	  const char *msg = (const char *) NULL;
	  name = symbol_name (h, input_bfd, sec, symtab_hdr, sym);
	  switch (r)
	    {
	    case bfd_reloc_overflow:
	      if (!((*info->callbacks->reloc_overflow)
		    (info, name, howto->name, (bfd_vma) 0,
		     input_bfd, input_section, addr)))
		return false;
	      break;

	    case bfd_reloc_undefined:
	      if (!((*info->callbacks->undefined_symbol)
		    (info, name, input_bfd, input_section, addr, true)))
		return false;
	      break;

	    case bfd_reloc_outofrange:
	      msg = _("internal error: out of range error");
	      goto common_error;

	    case bfd_reloc_notsupported:
	      msg = _("internal error: unsupported relocation error");
	      goto common_error;

	    default:
	      msg = _("internal error: unknown error");
	      /* fall through */

	    common_error:
	      if (!((*info->callbacks->warning)
		    (info, msg, name, input_bfd, 0, addr)))
		return false;

	      break;
	    }
	}
    }

  return true;
}

/* Create a BFD to hold the sections used by the thunk code.
   This function is called from the linker scripts in
   ld/emulparam/pj[l]elf.sh, inserted into ld/emultempl/elf32.em. */

boolean
bfd_elf32_pj_create_thunk_bfd (abfd, info)
     bfd *abfd;
     struct bfd_link_info *info;

{
  /* If we are only performing a partial link do not bother
     getting a bfd to hold the glue.  */
  if (!thunk_section && !info->relocateable)
    {
      elf_linker_section_t *els;
      elf_linker_section_t defaults;

      memset (&defaults, 0, sizeof (defaults));

      defaults.name = ".text.thunk";
      defaults.alignment = 2;
      defaults.flags = SEC_CODE | SEC_ALLOC 
	| SEC_LOAD | SEC_HAS_CONTENTS 
	| SEC_IN_MEMORY | SEC_LINKER_CREATED;

      els = _bfd_elf_create_linker_section (abfd,
					    info,
					    LINKER_SECTION_THUNKS, 
					    &defaults);

      thunk_section = els->section;
      thunk_section->_raw_size = 0;
      thunk_section->_cooked_size = 0;
    }
  return true;
}



/* Called by the linker for each input bfd to add information into the
   thunk bfd.  The generates counts the number of calls which don't
   pass the right number of arguments to their callee and increments
   the size of what will become the thunk_section accordingly.  Each
   little thunk is given a name derived from that of the callee.  */

boolean
bfd_elf32_pj_process_thunk_bfd (abfd, link_info)
     bfd *abfd;
     struct bfd_link_info *link_info;
{
  Elf_Internal_Shdr *symtab_hdr;
  Elf_Internal_Rela *free_relocs = NULL;
  Elf_Internal_Rela *irel, *irelend;
  Elf32_External_Sym *extsyms = NULL;
  Elf32_External_Sym *free_extsyms = NULL;
  asection *sec;

  /* If we are only performing a partial link do not bother
     to construct any glue.  */
  if (link_info->relocateable)
    return true;

  sec = abfd->sections;

  if (sec == NULL)
    return true;

  for (; sec != NULL; sec = sec->next)
    {
      if (sec->reloc_count == 0)
	continue;

      symtab_hdr = &elf_tdata (abfd)->symtab_hdr;

      irel = (_bfd_elf32_link_read_relocs (abfd, sec, (PTR) NULL,
					   (Elf_Internal_Rela *) NULL, false));

      BFD_ASSERT (irel != 0);

      irelend = irel + sec->reloc_count;
      for (; irel < irelend; irel++)
	{
	  long r_type = ELF32_R_TYPE (irel->r_info);
	  unsigned long r_index;
	  int callee_consumes;
	  int caller_produces;
	  struct elf_link_hash_entry *h;

	  int diff;
	  int callee_varargs;
	  Elf_Internal_Sym isym;

	  /* For this pass we only care about one relocation. */
	  if (r_type != R_PJ_WORDS_PUSHED)
	    continue;

	  r_index = ELF32_R_SYM (irel->r_info);

	  /* Read this BFD's symbols if we haven't done so already.  */
	  if (extsyms == NULL)
	    {
	      /* Get cached copy if it exists.  */
	      if (symtab_hdr->contents != NULL)
		extsyms = (Elf32_External_Sym *) symtab_hdr->contents;
	      else
		{
		  /* Go get them off disk.  */
		  extsyms = ((Elf32_External_Sym *)
			     bfd_malloc (symtab_hdr->sh_size));
		  if (extsyms == NULL)
		    goto error_return;
		  free_extsyms = extsyms;
		  if (bfd_seek (abfd, symtab_hdr->sh_offset, SEEK_SET) != 0
		      || (bfd_read (extsyms, 1, symtab_hdr->sh_size, abfd)
			  != symtab_hdr->sh_size))
		    goto error_return;
		}
	    }

	  /* Get the callee_consumes value of the symbol referred to
             by the reloc.  */

	  if (r_index < symtab_hdr->sh_info)
	    {
	      /* A local symbol.  */
	      bfd_elf32_swap_symbol_in (abfd,
					extsyms + r_index,
					&isym);

	      callee_consumes = isym.st_other;
	    }
	  else
	    {
	      /* An external symbol.  */
	      h = elf_sym_hashes (abfd)[r_index - symtab_hdr->sh_info];

	      if (h->root.type != bfd_link_hash_defined
		  && h->root.type != bfd_link_hash_defweak)
		{
		  /* This appears to be a reference to an undefined
		     symbol.  Just ignore it--it will be caught by the
		     regular reloc processing.  */
		  continue;
		}

	      callee_consumes = h->other;
	    }

	  if (callee_consumes == 0)
	    {
	      /* The symbol doesn't have any consumes information.  */
	      const char *name =
		symbol_name (h, abfd, sec, symtab_hdr, &isym);
	      char *msg = _("no .words_needed info in %s");
	      char *buf = alloca (strlen (name) + strlen (msg) + 1);
	      sprintf (buf, msg, name);
	      (*link_info->callbacks->warning) (link_info, buf, name,
						abfd, 0, irel->r_offset);
	      break;
	    }
	  else if (!(callee_consumes & 0x80)
		   && irel->r_addend != callee_consumes)
	    {
	      /* The symbol isn't for a varargs function, nor does it 
		 consume the same number of words as the caller
		 provides, so it needs a thunk. 

		 Construct a symbol pointing to where the thunk chunk
		 will live in the output text section and remember how
		 big the output thunk chunk will be. */

	      const char *name =
		symbol_name (h, abfd, sec, symtab_hdr, &isym);

	      int l = strlen (name);
	      char *b = (char *) bfd_malloc (l + 8);
	      struct elf_link_hash_entry *nh;
	      strcpy (b, ".thunk.");
	      memcpy (b + 7, name, l + 1);

	      nh = (struct elf_link_hash_entry *)
		bfd_link_hash_lookup (link_info->hash, b, false, false,
				      false);

	      if ((nh == NULL || nh->root.type == bfd_link_hash_undefined))
		_bfd_generic_link_add_one_symbol
		  (link_info,
		   abfd,
		   b,
		   BSF_GLOBAL,
		   thunk_section,
		   thunk_section->_raw_size,
		   (const char *) NULL,
		   false,
		   get_elf_backend_data (abfd)->collect,
		   (struct bfd_link_hash_entry **) &nh);

	      thunk_section->_raw_size += sizeof (thunk_chunk);
	    }
	}
    }

  return true;

error_return:
  if (free_relocs != NULL)
    free (free_relocs);
  if (free_extsyms != NULL)
    free (free_extsyms);
  return false;
}

static boolean
pj_elf_always_size_sections (abfd, info)
     bfd *abfd;
     struct bfd_link_info *info;
{
  return true;
}

static boolean
pj_elf_finish_dynamic_sections (abfd, link_info)
     bfd *abfd;
     struct bfd_link_info *link_info;
{
  return true;
}


#define TARGET_BIG_SYM		bfd_elf32_pj_vec
#define TARGET_BIG_NAME		"elf32-pj"
#define TARGET_LITTLE_SYM	bfd_elf32_pjl_vec
#define TARGET_LITTLE_NAME	"elf32-pjl"
#define ELF_ARCH		bfd_arch_pj
#define ELF_MACHINE_CODE	EM_PJ
#define ELF_MAXPAGESIZE		0x1000

#define bfd_elf32_bfd_reloc_type_lookup	     bfd_elf_generic_reloc_type_lookup
#define elf_backend_final_write_processing   pj_elf_final_write_processing
#define elf_info_to_howto                    pj_elf_info_to_howto
#define elf_backend_relocate_section	     pj_elf_relocate_section
#define elf_backend_finish_dynamic_sections  pj_elf_finish_dynamic_sections
#define elf_backend_always_size_sections     pj_elf_always_size_sections


#include "elf32-target.h"
