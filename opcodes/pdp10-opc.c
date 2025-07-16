/* Opcodes support for PDP-10.
   Copyright (C) 2023-2025 Free Software Foundation, Inc.
   Contributed by Mikael Pettersson <mikpelinux@gmail.com>.

   This file is part of the GNU opcodes library.

   This library is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   It is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING3.  If not,
   see <http://www.gnu.org/licenses/>.  */

#include "sysdep.h"
#include "opcode/pdp10.h"
#include "libiberty.h"
#include <strings.h>

const struct pdp10_cpu_model pdp10_cpu_models[] = {
  { "all",    PDP10_ALL },
  { "ka10",   PDP10_KA10 },
  { "ki10",   PDP10_KI10 },
  { "kl10",   PDP10_KL10 },
  { "kl10b",  PDP10_KL10_271 },
  { "pdp6",   PDP6 },
  { "xkl1",   PDP10_XKL1 },
};

const int pdp10_num_cpu_models = ARRAY_SIZE (pdp10_cpu_models);

pdp10_cpu_models_t
pdp10_cpu_models_from_name (const char *name, size_t len)
{
  unsigned int low, high, mid;
  int cmp;

  low = 0;
  high = ARRAY_SIZE (pdp10_cpu_models);

  while (low < high)
    {
      mid = (low + high) / 2;
      cmp = strncasecmp (name, pdp10_cpu_models[mid].name, len);
      if (cmp == 0)
	{
	  if (pdp10_cpu_models[mid].name[len] == '\0')
	    return pdp10_cpu_models[mid].models;
	  high = mid;
	}
      else if (cmp < 0)
	high = mid;
      else
	low = mid + 1;
    }

  return 0;
}

/* Internal device names for IO instructions.

   The convention in documentation is to list 7-bit device codes as
   three-digit octal numbers with two low excess zeros.  The D()
   macro corrects those values back to 7 bits.  */

static const struct {
  const char *name;
  pdp10_cpu_device_t device;
  pdp10_cpu_models_t models;
} cpu_devices[] = {
#define D(DEV9) ((DEV9) >> 2)
  { "apr", D(000), PDP10_KA10_to_KL10 },   /* Arithmetic processor.  */
  { "cca", D(014), PDP10_KL10any },        /* Cache.  */
  { "mtr", D(024), PDP10_KL10any },        /* Meters.  */
  { "pag", D(010), PDP10_KI10_to_KL10 },   /* Pager.  */
  { "pi",  D(004), PDP10_KA10_to_KL10 },   /* Priority interrupt.  */
  { "tim", D(020), PDP10_KL10any },        /* Timer.  */
#undef D
};

int
pdp10_cpu_device_from_name (const char *name, pdp10_cpu_models_t models)
{
  unsigned int low, high, mid;
  int cmp;

  low = 0;
  high = ARRAY_SIZE (cpu_devices);

  while (low < high)
    {
      mid = (low + high) / 2;
      cmp = strcasecmp (name, cpu_devices[mid].name);
      if (cmp == 0)
	{
	  if ((cpu_devices[mid].models & models) == models)
	    return cpu_devices[mid].device;
	  break;
	}
      else if (cmp < 0)
	high = mid;
      else
	low = mid + 1;
    }

  return -1;
}

const struct pdp10_insn pdp10_insns[] = {
};

const int pdp10_num_insns = 0;
