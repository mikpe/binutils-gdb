/* Prologue value handling for GDB.
   Copyright (C) 2003-2025 Free Software Foundation, Inc.

   This file is part of GDB.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#include "prologue-value.h"
#include "regcache.h"


/* Constructors.  */

pv_t
pv_unknown (void)
{
  pv_t v = { pvk_unknown, 0, 0 };

  return v;
}


pv_t
pv_constant (CORE_ADDR k)
{
  pv_t v;

  v.kind = pvk_constant;
  v.reg = -1;                   /* for debugging */
  v.k = k;

  return v;
}


pv_t
pv_register (int reg, CORE_ADDR k)
{
  pv_t v;

  v.kind = pvk_register;
  v.reg = reg;
  v.k = k;

  return v;
}



/* Arithmetic operations.  */

/* If one of *A and *B is a constant, and the other isn't, swap the
   values as necessary to ensure that *B is the constant.  This can
   reduce the number of cases we need to analyze in the functions
   below.  */
static void
constant_last (pv_t *a, pv_t *b)
{
  if (a->kind == pvk_constant
      && b->kind != pvk_constant)
    {
      pv_t temp = *a;
      *a = *b;
      *b = temp;
    }
}


pv_t
pv_add (pv_t a, pv_t b)
{
  constant_last (&a, &b);

  /* We can add a constant to a register.  */
  if (a.kind == pvk_register
      && b.kind == pvk_constant)
    return pv_register (a.reg, a.k + b.k);

  /* We can add a constant to another constant.  */
  else if (a.kind == pvk_constant
	   && b.kind == pvk_constant)
    return pv_constant (a.k + b.k);

  /* Anything else we don't know how to add.  We don't have a
     representation for, say, the sum of two registers, or a multiple
     of a register's value (adding a register to itself).  */
  else
    return pv_unknown ();
}


pv_t
pv_add_constant (pv_t v, CORE_ADDR k)
{
  /* Rather than thinking of all the cases we can and can't handle,
     we'll just let pv_add take care of that for us.  */
  return pv_add (v, pv_constant (k));
}


pv_t
pv_subtract (pv_t a, pv_t b)
{
  /* This isn't quite the same as negating B and adding it to A, since
     we don't have a representation for the negation of anything but a
     constant.  For example, we can't negate { pvk_register, R1, 10 },
     but we do know that { pvk_register, R1, 10 } minus { pvk_register,
     R1, 5 } is { pvk_constant, <ignored>, 5 }.

     This means, for example, that we could subtract two stack
     addresses; they're both relative to the original SP.  Since the
     frame pointer is set based on the SP, its value will be the
     original SP plus some constant (probably zero), so we can use its
     value just fine, too.  */

  constant_last (&a, &b);

  /* We can subtract two constants.  */
  if (a.kind == pvk_constant
      && b.kind == pvk_constant)
    return pv_constant (a.k - b.k);

  /* We can subtract a constant from a register.  */
  else if (a.kind == pvk_register
	   && b.kind == pvk_constant)
    return pv_register (a.reg, a.k - b.k);

  /* We can subtract a register from itself, yielding a constant.  */
  else if (a.kind == pvk_register
	   && b.kind == pvk_register
	   && a.reg == b.reg)
    return pv_constant (a.k - b.k);

  /* We don't know how to subtract anything else.  */
  else
    return pv_unknown ();
}


pv_t
pv_logical_and (pv_t a, pv_t b)
{
  constant_last (&a, &b);

  /* We can 'and' two constants.  */
  if (a.kind == pvk_constant
      && b.kind == pvk_constant)
    return pv_constant (a.k & b.k);

  /* We can 'and' anything with the constant zero.  */
  else if (b.kind == pvk_constant
	   && b.k == 0)
    return pv_constant (0);

  /* We can 'and' anything with ~0.  */
  else if (b.kind == pvk_constant
	   && b.k == ~ (CORE_ADDR) 0)
    return a;

  /* We can 'and' a register with itself.  */
  else if (a.kind == pvk_register
	   && b.kind == pvk_register
	   && a.reg == b.reg
	   && a.k == b.k)
    return a;

  /* Otherwise, we don't know.  */
  else
    return pv_unknown ();
}



/* Examining prologue values.  */

int
pv_is_identical (pv_t a, pv_t b)
{
  if (a.kind != b.kind)
    return 0;

  switch (a.kind)
    {
    case pvk_unknown:
      return 1;
    case pvk_constant:
      return (a.k == b.k);
    case pvk_register:
      return (a.reg == b.reg && a.k == b.k);
    default:
      gdb_assert_not_reached ("unexpected prologue value kind");
    }
}


int
pv_is_constant (pv_t a)
{
  return (a.kind == pvk_constant);
}


int
pv_is_register (pv_t a, int r)
{
  return (a.kind == pvk_register
	  && a.reg == r);
}


int
pv_is_register_k (pv_t a, int r, CORE_ADDR k)
{
  return (a.kind == pvk_register
	  && a.reg == r
	  && a.k == k);
}


enum pv_boolean
pv_is_array_ref (pv_t addr, CORE_ADDR size,
		 pv_t array_addr, CORE_ADDR array_len,
		 CORE_ADDR elt_size,
		 int *i)
{
  /* Note that, since .k is a CORE_ADDR, and CORE_ADDR is unsigned, if
     addr is *before* the start of the array, then this isn't going to
     be negative...  */
  pv_t offset = pv_subtract (addr, array_addr);

  if (offset.kind == pvk_constant)
    {
      /* This is a rather odd test.  We want to know if the SIZE bytes
	 at ADDR don't overlap the array at all, so you'd expect it to
	 be an || expression: "if we're completely before || we're
	 completely after".  But with unsigned arithmetic, things are
	 different: since it's a number circle, not a number line, the
	 right values for offset.k are actually one contiguous range.  */
      if (offset.k <= -size
	  && offset.k >= array_len * elt_size)
	return pv_definite_no;
      else if (offset.k % elt_size != 0
	       || size != elt_size)
	return pv_maybe;
      else
	{
	  *i = offset.k / elt_size;
	  return pv_definite_yes;
	}
    }
  else
    return pv_maybe;
}



/* Areas.  */


/* A particular value known to be stored in an area.

   Entries form a ring, sorted by unsigned offset from the area's base
   register's value.  Since entries can straddle the wrap-around point,
   unsigned offsets form a circle, not a number line, so the list
   itself is structured the same way --- there is no inherent head.
   The entry with the lowest offset simply follows the entry with the
   highest offset.  Entries may abut, but never overlap.  The area's
   'entry' pointer points to an arbitrary node in the ring.  */
struct pv_area::area_entry
{
  /* Links in the doubly-linked ring.  */
  struct area_entry *prev, *next;

  /* Offset of this entry's address from the value of the base
     register.  */
  CORE_ADDR offset;

  /* The size of this entry.  Note that an entry may wrap around from
     the end of the address space to the beginning.  */
  CORE_ADDR size;

  /* The value stored here.  */
  pv_t value;
};


/* See prologue-value.h.  */

pv_area::pv_area (int base_reg, int addr_bit)
  : m_base_reg (base_reg),
    /* Remember that shift amounts equal to the type's width are
       undefined.  */
    m_addr_mask (((((CORE_ADDR) 1 << (addr_bit - 1)) - 1) << 1) | 1),
    m_entry (nullptr)
{
}

/* See prologue-value.h.  */

void
pv_area::clear_entries ()
{
  struct area_entry *e = m_entry;

  if (e)
    {
      /* This needs to be a do-while loop, in order to actually
	 process the node being checked for in the terminating
	 condition.  */
      do
	{
	  struct area_entry *next = e->next;

	  xfree (e);
	  e = next;
	}
      while (e != m_entry);

      m_entry = 0;
    }
}


pv_area::~pv_area ()
{
  clear_entries ();
}


/* See prologue-value.h.  */

bool
pv_area::store_would_trash (pv_t addr)
{
  /* It may seem odd that pvk_constant appears here --- after all,
     that's the case where we know the most about the address!  But
     pv_areas are always relative to a register, and we don't know the
     value of the register, so we can't compare entry addresses to
     constants.  */
  return (addr.kind == pvk_unknown
	  || addr.kind == pvk_constant
	  || (addr.kind == pvk_register && addr.reg != m_base_reg));
}


/* See prologue-value.h.  */

struct pv_area::area_entry *
pv_area::find_entry (CORE_ADDR offset)
{
  struct area_entry *e = m_entry;

  if (! e)
    return 0;

  /* If the next entry would be better than the current one, then scan
     forward.  Since we use '<' in this loop, it always terminates.

     Note that, even setting aside the addr_mask stuff, we must not
     simplify this, in high school algebra fashion, to
     (e->next->offset < e->offset), because of the way < interacts
     with wrap-around.  We have to subtract offset from both sides to
     make sure both things we're comparing are on the same side of the
     discontinuity.  */
  while (((e->next->offset - offset) & m_addr_mask)
	 < ((e->offset - offset) & m_addr_mask))
    e = e->next;

  /* If the previous entry would be better than the current one, then
     scan backwards.  */
  while (((e->prev->offset - offset) & m_addr_mask)
	 < ((e->offset - offset) & m_addr_mask))
    e = e->prev;

  /* In case there's some locality to the searches, set the area's
     pointer to the entry we've found.  */
  m_entry = e;

  return e;
}


/* See prologue-value.h.  */

int
pv_area::overlaps (struct area_entry *entry, CORE_ADDR offset, CORE_ADDR size)
{
  /* Think carefully about wrap-around before simplifying this.  */
  return (((entry->offset - offset) & m_addr_mask) < size
	  || ((offset - entry->offset) & m_addr_mask) < entry->size);
}


/* See prologue-value.h.  */

void
pv_area::store (pv_t addr, CORE_ADDR size, pv_t value)
{
  /* Remove any (potentially) overlapping entries.  */
  if (store_would_trash (addr))
    clear_entries ();
  else
    {
      CORE_ADDR offset = addr.k;
      struct area_entry *e = find_entry (offset);

      /* Delete all entries that we would overlap.  */
      while (e && overlaps (e, offset, size))
	{
	  struct area_entry *next = (e->next == e) ? 0 : e->next;

	  e->prev->next = e->next;
	  e->next->prev = e->prev;

	  xfree (e);
	  e = next;
	}

      /* Move the area's pointer to the next remaining entry.  This
	 will also zero the pointer if we've deleted all the entries.  */
      m_entry = e;
    }

  /* Now, there are no entries overlapping us, and m_entry is
     either zero or pointing at the closest entry after us.  We can
     just insert ourselves before that.

     But if we're storing an unknown value, don't bother --- that's
     the default.  */
  if (value.kind == pvk_unknown)
    return;
  else
    {
      CORE_ADDR offset = addr.k;
      struct area_entry *e = XNEW (struct area_entry);

      e->offset = offset;
      e->size = size;
      e->value = value;

      if (m_entry)
	{
	  e->prev = m_entry->prev;
	  e->next = m_entry;
	  e->prev->next = e->next->prev = e;
	}
      else
	{
	  e->prev = e->next = e;
	  m_entry = e;
	}
    }
}


/* See prologue-value.h.  */

pv_t
pv_area::fetch (pv_t addr, CORE_ADDR size)
{
  /* If we have no entries, or we can't decide how ADDR relates to the
     entries we do have, then the value is unknown.  */
  if (! m_entry
      || store_would_trash (addr))
    return pv_unknown ();
  else
    {
      CORE_ADDR offset = addr.k;
      struct area_entry *e = find_entry (offset);

      /* If this entry exactly matches what we're looking for, then
	 we're set.  Otherwise, say it's unknown.  */
      if (e->offset == offset && e->size == size)
	return e->value;
      else
	return pv_unknown ();
    }
}


/* See prologue-value.h.  */

bool
pv_area::find_reg (struct gdbarch *gdbarch, int reg, CORE_ADDR *offset_p)
{
  struct area_entry *e = m_entry;

  if (e)
    do
      {
	if (e->value.kind == pvk_register
	    && e->value.reg == reg
	    && e->value.k == 0
	    && e->size == register_size (gdbarch, reg))
	  {
	    if (offset_p)
	      *offset_p = e->offset;
	    return true;
	  }

	e = e->next;
      }
    while (e != m_entry);

  return false;
}


/* See prologue-value.h.  */

void
pv_area::scan (void (*func) (void *closure,
			     pv_t addr,
			     CORE_ADDR size,
			     pv_t value),
	       void *closure)
{
  struct area_entry *e = m_entry;
  pv_t addr;

  addr.kind = pvk_register;
  addr.reg = m_base_reg;

  if (e)
    do
      {
	addr.k = e->offset;
	func (closure, addr, e->size, e->value);
	e = e->next;
      }
    while (e != m_entry);
}
