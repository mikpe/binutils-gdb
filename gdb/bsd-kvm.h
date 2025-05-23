/* BSD Kernel Data Access Library (libkvm) interface.

   Copyright (C) 2004-2025 Free Software Foundation, Inc.

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

#ifndef GDB_BSD_KVM_H
#define GDB_BSD_KVM_H

struct pcb;
struct regcache;

/* Add the libkvm interface to the list of all possible targets and
   register CUPPLY_PCB as the architecture-specific process control
   block interpreter.  */

extern void
  bsd_kvm_add_target (int (*supply_pcb)(struct regcache *, struct pcb *));

#endif /* GDB_BSD_KVM_H */
