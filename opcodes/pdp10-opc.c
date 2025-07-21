/* Opcodes support for PDP-10.
   Copyright (C) 2025 Free Software Foundation, Inc.
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

/* Macros to initialize both high13 and format in instruction descriptors.
   The E_UNUSED flag may be set separately.

   The convention in documentation is to list opcodes as three or five-digit
   octal numbers, with zeros in IO device subfields.  Five-digit numbers have
   two excess bits, which are removed by the macros to produce 13 bits.

   Much of the contents of these tables originated from Lars Brinkhoff's
   pdp10-its-disassembler (GPLv2) in 2013, but it has been tweaked, extended,
   and rewritten since then.  */

#define BASIC(OPCODE9)     (OPCODE9)  << 4, PDP10_FMT_BASIC
#define A_NONZERO(OPCODE9) (OPCODE9)  << 4, PDP10_FMT_A_NONZERO
#define A_OPCODE(OPCODE15) (OPCODE15) >> 2, PDP10_FMT_A_OPCODE
#define A_UNUSED(OPCODE9)  (OPCODE9)  << 4, PDP10_FMT_BASIC | PDP10_FMT_A_UNUSED
#define IO(OPCODE15)       (OPCODE15) >> 2, PDP10_FMT_IO
#define EXTENDED(OPCODE9)  (OPCODE9)  << 4, PDP10_FMT_A_OPCODE | PDP10_FMT_EXTENDED

#define E_UNUSED           PDP10_FMT_E_UNUSED

const struct pdp10_insn pdp10_insns[] = {
  /* name,              high13, format,                 models */

  /* 000: ILLEGAL */
  /* 001-037: LUUOs */

  /* ITS MUUOs */

  { ".iot",             BASIC(040),                     PDP10_ITS },
  { ".open",            BASIC(041),                     PDP10_ITS },
  { ".oper",            BASIC(042),                     PDP10_ITS },
  { ".call",            A_OPCODE(04300),                PDP10_ITS },
  { ".dismis",          A_OPCODE(04304),                PDP10_ITS },
  { ".lose",            A_OPCODE(04310),                PDP10_ITS },
  { ".tranad",          A_OPCODE(04314),                PDP10_ITS },
  { ".value",           A_OPCODE(04320) | E_UNUSED,     PDP10_ITS },
  { ".utran",           A_OPCODE(04324),                PDP10_ITS },
  { ".core",            A_OPCODE(04330),                PDP10_ITS },
  { ".trand",           A_OPCODE(04334),                PDP10_ITS },
  { ".dstart",          A_OPCODE(04340),                PDP10_ITS },
  { ".fdele",           A_OPCODE(04344),                PDP10_ITS },
  { ".dstrtl",          A_OPCODE(04350),                PDP10_ITS },
  { ".suset",           A_OPCODE(04354),                PDP10_ITS },
  { ".ltpen",           A_OPCODE(04360),                PDP10_ITS },
  { ".vscan",           A_OPCODE(04364),                PDP10_ITS },
  { ".potset",          A_OPCODE(04370),                PDP10_ITS },
  { ".uset",            BASIC(044),                     PDP10_ITS },
  { ".break",           BASIC(045),                     PDP10_ITS },
  { ".status",          BASIC(046),                     PDP10_ITS },
  { ".access",          BASIC(047),                     PDP10_ITS },

  /* TOPS-10 MUUOs (formats and models guesstimates) */
  { ".call",            BASIC(040),                     PDP10_KA10up_not_ITS },
  { ".init",            BASIC(041),                     PDP10_KA10up_not_ITS },
  /* 042-046: reserved MUUOs */
  { ".calli",           BASIC(047),                     PDP10_KA10up_not_ITS },
  { ".open",            BASIC(050),                     PDP10_KA10up_not_ITS },
  { ".ttcall",          BASIC(051),                     PDP10_KA10up_not_ITS },
  { ".rename",          BASIC(055),                     PDP10_KA10up_not_ITS },
  { ".in",              BASIC(056),                     PDP10_KA10up_not_ITS },
  { ".out",             BASIC(057),                     PDP10_KA10up_not_ITS },
  { ".setsts",          BASIC(060),                     PDP10_KA10up_not_ITS },
  { ".stato",           BASIC(061),                     PDP10_KA10up_not_ITS },
  { ".status",          BASIC(062),                     PDP10_KA10up_not_ITS },
  { ".getsts",          BASIC(062),                     PDP10_KA10up_not_ITS }, /* FIXME: alias for .status? */
  { ".statz",           BASIC(063),                     PDP10_KA10up_not_ITS },
  { ".inbuf",           BASIC(064),                     PDP10_KA10up_not_ITS },
  { ".outbuf",          BASIC(065),                     PDP10_KA10up_not_ITS },
  { ".input",           BASIC(066),                     PDP10_KA10up_not_ITS },
  { ".output",          BASIC(067),                     PDP10_KA10up_not_ITS },
  { ".close",           BASIC(070),                     PDP10_KA10up_not_ITS },
  { ".releas",          BASIC(071),                     PDP10_KA10up_not_ITS },
  { ".mtape",           BASIC(072),                     PDP10_KA10up_not_ITS },
  { ".ugetf",           BASIC(073),                     PDP10_KA10up_not_ITS },
  { ".useti",           BASIC(074),                     PDP10_KA10up_not_ITS },
  { ".useto",           BASIC(075),                     PDP10_KA10up_not_ITS },
  { ".lookup",          BASIC(076),                     PDP10_KA10up_not_ITS },
  { ".enter",           BASIC(077),                     PDP10_KA10up_not_ITS },
  { ".ujen",            BASIC(0100),                    PDP10_KA10 }, /* FIXME: or KI10up? + not ITS */
  /* 101: unassigned */
  { "gfad",             BASIC(0102),                    PDP10_KL10_271 }, /* FIXME: or 271up? */
  { "gfsb",             BASIC(0103),                    PDP10_KL10_271 }, /* FIXME: or 271up? */

  /* TOPS-20 MUUO */
  { "jsys",             BASIC(0104),                    PDP10_KI10up & ~PDP10_ITS },

  { "adjsp",            BASIC(0105),                    PDP10_KL10up },
  { "gfmp",             BASIC(0106),                    PDP10_KL10_271 }, /* FIXME: or 271up? */
  { "gfdv",             BASIC(0107),                    PDP10_KL10_271 }, /* FIXME: or 271up? */
  { "dfad",             BASIC(0110),                    PDP10_KI10up },
  { "dfsb",             BASIC(0111),                    PDP10_KI10up },
  { "dfmp",             BASIC(0112),                    PDP10_KI10up },
  { "dfdv",             BASIC(0113),                    PDP10_KI10up },
  { "dadd",             BASIC(0114),                    PDP10_KL10up },
  { "dsub",             BASIC(0115),                    PDP10_KL10up },
  { "dmul",             BASIC(0116),                    PDP10_KL10up },
  { "ddiv",             BASIC(0117),                    PDP10_KL10up },
  { "dmove",            BASIC(0120),                    PDP10_KI10up },
  { "dmovn",            BASIC(0121),                    PDP10_KI10up },
  { "fix",              BASIC(0122),                    PDP10_KI10up },
  { "extend",           BASIC(0123),                    PDP10_KL10up },
  { "dmovem",           BASIC(0124),                    PDP10_KI10up },
  { "dmovnm",           BASIC(0125),                    PDP10_KI10up },
  { "fixr",             BASIC(0126),                    PDP10_KI10up },
  { "fltr",             BASIC(0127),                    PDP10_KI10up },
  { "ufa",              BASIC(0130),                    PDP10_KA10_to_KI10 }, /* FIXME: and TOPS-10 KL10 */
  { "dfn",              BASIC(0131),                    PDP10_KA10_to_KI10 }, /* FIXME: and TOPS-10 KL10 */
  { "fsc",              BASIC(0132),                    PDP10_ALL },
  { "ibp",              A_OPCODE(013300),               PDP10_ALL },
  { "adjbp",            A_NONZERO(0133),                PDP10_KL10up },
  { "ildb",             BASIC(0134),                    PDP10_ALL },
  { "ldb",              BASIC(0135),                    PDP10_ALL },
  { "idpb",             BASIC(0136),                    PDP10_ALL },
  { "dpb",              BASIC(0137),                    PDP10_ALL },
  { "fad",              BASIC(0140),                    PDP10_ALL },
  { "fadl",             BASIC(0141),                    PDP6_to_KI10 }, /* FIXME: and TOPS-10 KL10?, not PDP6? */
  { "fadm",             BASIC(0142),                    PDP10_ALL },
  { "fadb",             BASIC(0143),                    PDP10_ALL },
  { "fadr",             BASIC(0144),                    PDP10_ALL },
  { "fadri",            BASIC(0145),                    PDP10_KA10up },
  { "fadrl",            BASIC(0145),                    PDP6 },
  { "fadrm",            BASIC(0146),                    PDP10_ALL },
  { "fadrb",            BASIC(0147),                    PDP10_ALL },
  { "fsb",              BASIC(0150),                    PDP10_ALL },
  { "fsbl",             BASIC(0151),                    PDP6_to_KI10 }, /* FIXME: and TOPS-10 KL10 */
  { "fsbm",             BASIC(0152),                    PDP10_ALL },
  { "fsbb",             BASIC(0153),                    PDP10_ALL },
  { "fsbr",             BASIC(0154),                    PDP10_ALL },
  { "fsbri",            BASIC(0155),                    PDP10_KA10up },
  { "fsbrl",            BASIC(0155),                    PDP6 },
  { "fsbrm",            BASIC(0156),                    PDP10_ALL },
  { "fsbrb",            BASIC(0157),                    PDP10_ALL },
  { "fmp",              BASIC(0160),                    PDP10_ALL },
  { "fmpl",             BASIC(0161),                    PDP6_to_KI10 }, /* FIXME: and TOPS-10 KL10 */
  { "fmpm",             BASIC(0162),                    PDP10_ALL },
  { "fmpb",             BASIC(0163),                    PDP10_ALL },
  { "fmpr",             BASIC(0164),                    PDP10_ALL },
  { "fmpri",            BASIC(0165),                    PDP10_KA10up },
  { "fmprl",            BASIC(0165),                    PDP6 },
  { "fmprm",            BASIC(0166),                    PDP10_ALL },
  { "fmprb",            BASIC(0167),                    PDP10_ALL },
  { "fdv",              BASIC(0170),                    PDP10_ALL },
  { "fdvl",             BASIC(0171),                    PDP6_to_KI10 }, /* FIXME: and TOPS-10 KL10 */
  { "fdvm",             BASIC(0172),                    PDP10_ALL },
  { "fdvb",             BASIC(0173),                    PDP10_ALL },
  { "fdvr",             BASIC(0174),                    PDP10_ALL },
  { "fdvri",            BASIC(0175),                    PDP10_KA10up },
  { "fdvrl",            BASIC(0175),                    PDP6 },
  { "fdvrm",            BASIC(0176),                    PDP10_ALL },
  { "fdvrb",            BASIC(0177),                    PDP10_ALL },
  { "move",             BASIC(0200),                    PDP10_ALL },
  { "movei",            BASIC(0201),                    PDP10_ALL },
  { "movem",            BASIC(0202),                    PDP10_ALL },
  { "moves",            BASIC(0203),                    PDP10_ALL },
  { "movs",             BASIC(0204),                    PDP10_ALL },
  { "movsi",            BASIC(0205),                    PDP10_ALL },
  { "movsm",            BASIC(0206),                    PDP10_ALL },
  { "movss",            BASIC(0207),                    PDP10_ALL },
  { "movn",             BASIC(0210),                    PDP10_ALL },
  { "movni",            BASIC(0211),                    PDP10_ALL },
  { "movnm",            BASIC(0212),                    PDP10_ALL },
  { "movns",            BASIC(0213),                    PDP10_ALL },
  { "movm",             BASIC(0214),                    PDP10_ALL },
  { "movmi",            BASIC(0215),                    PDP10_ALL },
  { "movmm",            BASIC(0216),                    PDP10_ALL },
  { "movms",            BASIC(0217),                    PDP10_ALL },
  { "imul",             BASIC(0220),                    PDP10_ALL },
  { "imuli",            BASIC(0221),                    PDP10_ALL },
  { "imulm",            BASIC(0222),                    PDP10_ALL },
  { "imulb",            BASIC(0223),                    PDP10_ALL },
  { "mul",              BASIC(0224),                    PDP10_ALL },
  { "muli",             BASIC(0225),                    PDP10_ALL },
  { "mulm",             BASIC(0226),                    PDP10_ALL },
  { "mulb",             BASIC(0227),                    PDP10_ALL },
  { "idiv",             BASIC(0230),                    PDP10_ALL },
  { "idivi",            BASIC(0231),                    PDP10_ALL },
  { "idivm",            BASIC(0232),                    PDP10_ALL },
  { "idivb",            BASIC(0233),                    PDP10_ALL },
  { "div",              BASIC(0234),                    PDP10_ALL },
  { "divi",             BASIC(0235),                    PDP10_ALL },
  { "divm",             BASIC(0236),                    PDP10_ALL },
  { "divb",             BASIC(0237),                    PDP10_ALL },
  { "ash",              BASIC(0240),                    PDP10_ALL },
  { "rot",              BASIC(0241),                    PDP10_ALL },
  { "lsh",              BASIC(0242),                    PDP10_ALL },
  { "jffo",             BASIC(0243),                    PDP10_KA10up },
  { "ashc",             BASIC(0244),                    PDP10_ALL },
  { "rotc",             BASIC(0245),                    PDP10_ALL },
  { "lshc",             BASIC(0246),                    PDP10_ALL },
  /* 247: MUUO (XKL-1, KD10, KC10), unassigned/trapping (KI10, KL10, KS10?), unassigned/nop (KA10, PDP-6?) */
  { "exch",             BASIC(0250),                    PDP10_ALL },
  { "blt",              BASIC(0251),                    PDP10_ALL },
  { "aobjp",            BASIC(0252),                    PDP10_ALL },
  { "aobjn",            BASIC(0253),                    PDP10_ALL },

  /* 254: JRST instruction family.
     Special cases first, followed by the generic entry.
     25414, 25440 (jrstil?), 25444, 25454, 25464, 25470, 25470: unassigned */
  { "portal",           A_OPCODE(025404),               PDP10_ALL },
  { "jrstf",            A_OPCODE(025410),               PDP10_ALL },
  { "halt",             A_OPCODE(025420),               PDP10_ALL },
  { "xjrstf",           A_OPCODE(025424),               PDP10_KL10up },
  { "xjen",             A_OPCODE(025430),               PDP10_KL10up },
  { "xpcw",             A_OPCODE(025434),               PDP10_KL10up },
  { "jen",              A_OPCODE(025450),               PDP10_ALL },
  { "sfm",              A_OPCODE(025460),               PDP10_KL10up },
  { "jrst",             A_UNUSED(0254),                 PDP10_ALL },

  /* 255: JFCL instruction family.
     Special cases first, followed by the generic entry.
     25514, 25524, 25534, 25544, 25550, 25554, 25560, 25564, 25570, 25574: unassigned */
  { "nop",              A_OPCODE(025500) | E_UNUSED,    PDP10_ALL },
  { "jfov",             A_OPCODE(025504),               PDP10_KA10up },
  { "jpcch",            A_OPCODE(025504),               PDP6 },
  { "jcry1",            A_OPCODE(025510),               PDP10_ALL },
  { "jcry0",            A_OPCODE(025520),               PDP10_ALL },
  { "jcry",             A_OPCODE(025530),               PDP10_ALL },
  { "jov",              A_OPCODE(025540),               PDP10_ALL },
  { "jfcl",             BASIC(0255),                    PDP10_ALL },

  { "xct",              A_UNUSED(0256),                 PDP10_ALL }, /* A zero, or in user mode, or is a KA10 */
  { "pxct",             A_NONZERO(0256),                PDP10_ALL }, /* A non-zero and in executive mode */
  { "map",              BASIC(0257),                    PDP10_KA10_to_KI10 }, /* FIXME: and TOPS-10 KL10, nop on KA10 */
  { "pushj",            BASIC(0260),                    PDP10_ALL },
  { "push",             BASIC(0261),                    PDP10_ALL },
  { "pop",              BASIC(0262),                    PDP10_ALL },
  { "popj",             BASIC(0263) | E_UNUSED,         PDP10_ALL },
  { "jsr",              A_UNUSED(0264),                 PDP10_ALL },
  { "jsp",              BASIC(0265),                    PDP10_ALL },
  { "jsa",              BASIC(0266),                    PDP10_ALL },
  { "jra",              BASIC(0267),                    PDP10_ALL },
  { "add",              BASIC(0270),                    PDP10_ALL },
  { "addi",             BASIC(0271),                    PDP10_ALL },
  { "addm",             BASIC(0272),                    PDP10_ALL },
  { "addb",             BASIC(0273),                    PDP10_ALL },
  { "sub",              BASIC(0274),                    PDP10_ALL },
  { "subi",             BASIC(0275),                    PDP10_ALL },
  { "subm",             BASIC(0276),                    PDP10_ALL },
  { "subb",             BASIC(0277),                    PDP10_ALL },
  { "cai",              BASIC(0300),                    PDP10_ALL },
  { "cail",             BASIC(0301),                    PDP10_ALL },
  { "caie",             BASIC(0302),                    PDP10_ALL },
  { "caile",            BASIC(0303),                    PDP10_ALL },
  { "caia",             A_UNUSED(0304) | E_UNUSED,      PDP10_ALL },
  { "caige",            BASIC(0305),                    PDP10_ALL },
  { "cain",             BASIC(0306),                    PDP10_ALL },
  { "caig",             BASIC(0307),                    PDP10_ALL },
  { "cam",              BASIC(0310),                    PDP10_ALL },
  { "caml",             BASIC(0311),                    PDP10_ALL },
  { "came",             BASIC(0312),                    PDP10_ALL },
  { "camle",            BASIC(0313),                    PDP10_ALL },
  { "cama",             BASIC(0314),                    PDP10_ALL },
  { "camge",            BASIC(0315),                    PDP10_ALL },
  { "camn",             BASIC(0316),                    PDP10_ALL },
  { "camg",             BASIC(0317),                    PDP10_ALL },
  { "jump",             BASIC(0320),                    PDP10_ALL },
  { "jumpl",            BASIC(0321),                    PDP10_ALL },
  { "jumpe",            BASIC(0322),                    PDP10_ALL },
  { "jumple",           BASIC(0323),                    PDP10_ALL },
  { "jumpa",            A_UNUSED(0324),                 PDP10_ALL },
  { "jumpge",           BASIC(0325),                    PDP10_ALL },
  { "jumpn",            BASIC(0326),                    PDP10_ALL },
  { "jumpg",            BASIC(0327),                    PDP10_ALL },
  { "skip",             A_UNUSED(0330),                 PDP10_ALL },
  { "skipl",            A_UNUSED(0331),                 PDP10_ALL },
  { "skipe",            A_UNUSED(0332),                 PDP10_ALL },
  { "skiple",           A_UNUSED(0333),                 PDP10_ALL },
  { "skipa",            A_UNUSED(0334) | E_UNUSED,      PDP10_ALL },
  { "skipge",           A_UNUSED(0335),                 PDP10_ALL },
  { "skipn",            A_UNUSED(0336),                 PDP10_ALL },
  { "skipg",            A_UNUSED(0337),                 PDP10_ALL },
  { "aoj",              BASIC(0340),                    PDP10_ALL },
  { "aojl",             BASIC(0341),                    PDP10_ALL },
  { "aoje",             BASIC(0342),                    PDP10_ALL },
  { "aojle",            BASIC(0343),                    PDP10_ALL },
  { "aoja",             BASIC(0344),                    PDP10_ALL },
  { "aojge",            BASIC(0345),                    PDP10_ALL },
  { "aojn",             BASIC(0346),                    PDP10_ALL },
  { "aojg",             BASIC(0347),                    PDP10_ALL },
  { "aos",              A_UNUSED(0350),                 PDP10_ALL },
  { "aosl",             A_UNUSED(0351),                 PDP10_ALL },
  { "aose",             A_UNUSED(0352),                 PDP10_ALL },
  { "aosle",            A_UNUSED(0353),                 PDP10_ALL },
  { "aosa",             A_UNUSED(0354),                 PDP10_ALL },
  { "aosge",            A_UNUSED(0355),                 PDP10_ALL },
  { "aosn",             A_UNUSED(0356),                 PDP10_ALL },
  { "aosg",             A_UNUSED(0357),                 PDP10_ALL },
  { "soj",              BASIC(0360),                    PDP10_ALL },
  { "sojl",             BASIC(0361),                    PDP10_ALL },
  { "soje",             BASIC(0362),                    PDP10_ALL },
  { "sojle",            BASIC(0363),                    PDP10_ALL },
  { "soja",             BASIC(0364),                    PDP10_ALL },
  { "sojge",            BASIC(0365),                    PDP10_ALL },
  { "sojn",             BASIC(0366),                    PDP10_ALL },
  { "sojg",             BASIC(0367),                    PDP10_ALL },
  { "sos",              A_UNUSED(0370),                 PDP10_ALL },
  { "sosl",             A_UNUSED(0371),                 PDP10_ALL },
  { "sose",             A_UNUSED(0372),                 PDP10_ALL },
  { "sosle",            A_UNUSED(0373),                 PDP10_ALL },
  { "sosa",             A_UNUSED(0374),                 PDP10_ALL },
  { "sosge",            A_UNUSED(0375),                 PDP10_ALL },
  { "sosn",             A_UNUSED(0376),                 PDP10_ALL },
  { "sosg",             A_UNUSED(0377),                 PDP10_ALL },
  { "setz",             BASIC(0400) | E_UNUSED,         PDP10_ALL },
  { "setzi",            BASIC(0401) | E_UNUSED,         PDP10_ALL },
  { "setzm",            A_UNUSED(0402),                 PDP10_ALL },
  { "setzb",            BASIC(0403),                    PDP10_ALL },
  { "and",              BASIC(0404),                    PDP10_ALL },
  { "andi",             BASIC(0405),                    PDP10_ALL },
  { "andm",             BASIC(0406),                    PDP10_ALL },
  { "andb",             BASIC(0407),                    PDP10_ALL },
  { "andca",            BASIC(0410),                    PDP10_ALL },
  { "andcai",           BASIC(0411),                    PDP10_ALL },
  { "andcam",           BASIC(0412),                    PDP10_ALL },
  { "andcab",           BASIC(0413),                    PDP10_ALL },
  { "setm",             BASIC(0414),                    PDP10_ALL },
  { "xmovei",           BASIC(0415),                    PDP10_KL10up }, /* in non-zero section, setmi in zero section */
  { "setmi",            BASIC(0415),                    PDP10_ALL }, /* on KL10up depends on current section */
  { "setmm",            A_UNUSED(0416),                 PDP10_ALL },
  { "setmb",            BASIC(0417),                    PDP10_ALL },
  { "andcm",            BASIC(0420),                    PDP10_ALL },
  { "andcmi",           BASIC(0421),                    PDP10_ALL },
  { "andcmm",           BASIC(0422),                    PDP10_ALL },
  { "andcmb",           BASIC(0423),                    PDP10_ALL },
  { "seta",             BASIC(0424) | E_UNUSED,         PDP10_ALL },
  { "setai",            BASIC(0425) | E_UNUSED,         PDP10_ALL },
  { "setam",            BASIC(0426),                    PDP10_ALL },
  { "setab",            BASIC(0427),                    PDP10_ALL },
  { "xor",              BASIC(0430),                    PDP10_ALL },
  { "xori",             BASIC(0431),                    PDP10_ALL },
  { "xorm",             BASIC(0432),                    PDP10_ALL },
  { "xorb",             BASIC(0433),                    PDP10_ALL },
  { "ior",              BASIC(0434),                    PDP10_ALL },
  { "or",               BASIC(0434),                    PDP10_ALL }, /* alias for ior */
  { "iori",             BASIC(0435),                    PDP10_ALL },
  { "ori",              BASIC(0435),                    PDP10_ALL }, /* alias for iori */
  { "iorm",             BASIC(0436),                    PDP10_ALL },
  { "orm",              BASIC(0436),                    PDP10_ALL }, /* alias for iorm */
  { "iorb",             BASIC(0437),                    PDP10_ALL },
  { "orb",              BASIC(0437),                    PDP10_ALL }, /* alias for iorb */
  { "andcb",            BASIC(0440),                    PDP10_ALL },
  { "andcbi",           BASIC(0441),                    PDP10_ALL },
  { "andcbm",           BASIC(0442),                    PDP10_ALL },
  { "andcbb",           BASIC(0443),                    PDP10_ALL },
  { "eqv",              BASIC(0444),                    PDP10_ALL },
  { "eqvi",             BASIC(0445),                    PDP10_ALL },
  { "eqvm",             BASIC(0446),                    PDP10_ALL },
  { "eqvb",             BASIC(0447),                    PDP10_ALL },
  { "setca",            BASIC(0450) | E_UNUSED,         PDP10_ALL },
  { "setcai",           BASIC(0451) | E_UNUSED,         PDP10_ALL },
  { "setcam",           BASIC(0452),                    PDP10_ALL },
  { "setcab",           BASIC(0453),                    PDP10_ALL },
  { "orca",             BASIC(0454),                    PDP10_ALL },
  { "orcai",            BASIC(0455),                    PDP10_ALL },
  { "orcam",            BASIC(0456),                    PDP10_ALL },
  { "orcab",            BASIC(0457),                    PDP10_ALL },
  { "setcm",            BASIC(0460),                    PDP10_ALL },
  { "setcmi",           BASIC(0461),                    PDP10_ALL },
  { "setcmm",           A_UNUSED(0462),                 PDP10_ALL },
  { "setcmb",           BASIC(0463),                    PDP10_ALL },
  { "orcm",             BASIC(0464),                    PDP10_ALL },
  { "orcmi",            BASIC(0465),                    PDP10_ALL },
  { "orcmm",            BASIC(0466),                    PDP10_ALL },
  { "orcmb",            BASIC(0467),                    PDP10_ALL },
  { "orcb",             BASIC(0470),                    PDP10_ALL },
  { "orcbi",            BASIC(0471),                    PDP10_ALL },
  { "orcbm",            BASIC(0472),                    PDP10_ALL },
  { "orcbb",            BASIC(0473),                    PDP10_ALL },
  { "seto",             BASIC(0474) | E_UNUSED,         PDP10_ALL },
  { "setoi",            BASIC(0475) | E_UNUSED,         PDP10_ALL },
  { "setom",            A_UNUSED(0476),                 PDP10_ALL },
  { "setob",            BASIC(0477),                    PDP10_ALL },
  { "hll",              BASIC(0500),                    PDP10_ALL },
  { "xhlli",            BASIC(0501),                    PDP10_KL10up }, /* in non-zero section, hlli in zero section */
  { "hlli",             BASIC(0501),                    PDP10_ALL }, /* on KL10up depends on current section */
  { "hllm",             BASIC(0502),                    PDP10_ALL },
  { "hlls",             BASIC(0503),                    PDP10_ALL },
  { "hrl",              BASIC(0504),                    PDP10_ALL },
  { "hrli",             BASIC(0505),                    PDP10_ALL },
  { "hrlm",             BASIC(0506),                    PDP10_ALL },
  { "hrls",             BASIC(0507),                    PDP10_ALL },
  { "hllz",             BASIC(0510),                    PDP10_ALL },
  { "hllzi",            BASIC(0511),                    PDP10_ALL },
  { "hllzm",            BASIC(0512),                    PDP10_ALL },
  { "hllzs",            BASIC(0513),                    PDP10_ALL },
  { "hrlz",             BASIC(0514),                    PDP10_ALL },
  { "hrlzi",            BASIC(0515),                    PDP10_ALL },
  { "hrlzm",            BASIC(0516),                    PDP10_ALL },
  { "hrlzs",            BASIC(0517),                    PDP10_ALL },
  { "hllo",             BASIC(0520),                    PDP10_ALL },
  { "hlloi",            BASIC(0521),                    PDP10_ALL },
  { "hllom",            BASIC(0522),                    PDP10_ALL },
  { "hllos",            BASIC(0523),                    PDP10_ALL },
  { "hrlo",             BASIC(0524),                    PDP10_ALL },
  { "hrloi",            BASIC(0525),                    PDP10_ALL },
  { "hrlom",            BASIC(0526),                    PDP10_ALL },
  { "hrlos",            BASIC(0527),                    PDP10_ALL },
  { "hlle",             BASIC(0530),                    PDP10_ALL },
  { "hllei",            BASIC(0531),                    PDP10_ALL },
  { "hllem",            BASIC(0532),                    PDP10_ALL },
  { "hlles",            BASIC(0533),                    PDP10_ALL },
  { "hrle",             BASIC(0534),                    PDP10_ALL },
  { "hrlei",            BASIC(0535),                    PDP10_ALL },
  { "hrlem",            BASIC(0536),                    PDP10_ALL },
  { "hrles",            BASIC(0537),                    PDP10_ALL },
  { "hrr",              BASIC(0540),                    PDP10_ALL },
  { "hrri",             BASIC(0541),                    PDP10_ALL },
  { "hrrm",             BASIC(0542),                    PDP10_ALL },
  { "hrrs",             BASIC(0543),                    PDP10_ALL },
  { "hlr",              BASIC(0544),                    PDP10_ALL },
  { "hlri",             BASIC(0545),                    PDP10_ALL },
  { "hlrm",             BASIC(0546),                    PDP10_ALL },
  { "hlrs",             BASIC(0547),                    PDP10_ALL },
  { "hrrz",             BASIC(0550),                    PDP10_ALL },
  { "hrrzi",            BASIC(0551),                    PDP10_ALL },
  { "hrrzm",            BASIC(0552),                    PDP10_ALL },
  { "hrrzs",            BASIC(0553),                    PDP10_ALL },
  { "hlrz",             BASIC(0554),                    PDP10_ALL },
  { "hlrzi",            BASIC(0555),                    PDP10_ALL },
  { "hlrzm",            BASIC(0556),                    PDP10_ALL },
  { "hlrzs",            BASIC(0557),                    PDP10_ALL },
  { "hrro",             BASIC(0560),                    PDP10_ALL },
  { "hrroi",            BASIC(0561),                    PDP10_ALL },
  { "hrrom",            BASIC(0562),                    PDP10_ALL },
  { "hrros",            BASIC(0563),                    PDP10_ALL },
  { "hlro",             BASIC(0564),                    PDP10_ALL },
  { "hlroi",            BASIC(0565),                    PDP10_ALL },
  { "hlrom",            BASIC(0566),                    PDP10_ALL },
  { "hlros",            BASIC(0567),                    PDP10_ALL },
  { "hrre",             BASIC(0570),                    PDP10_ALL },
  { "hrrei",            BASIC(0571),                    PDP10_ALL },
  { "hrrem",            BASIC(0572),                    PDP10_ALL },
  { "hrres",            BASIC(0573),                    PDP10_ALL },
  { "hlre",             BASIC(0574),                    PDP10_ALL },
  { "hlrei",            BASIC(0575),                    PDP10_ALL },
  { "hlrem",            BASIC(0576),                    PDP10_ALL },
  { "hlres",            BASIC(0577),                    PDP10_ALL },
  { "trn",              BASIC(0600),                    PDP10_ALL },
  { "tln",              BASIC(0601),                    PDP10_ALL },
  { "trne",             BASIC(0602),                    PDP10_ALL },
  { "tlne",             BASIC(0603),                    PDP10_ALL },
  { "trna",             BASIC(0604),                    PDP10_ALL },
  { "tlna",             BASIC(0605),                    PDP10_ALL },
  { "trnn",             BASIC(0606),                    PDP10_ALL },
  { "tlnn",             BASIC(0607),                    PDP10_ALL },
  { "tdn",              BASIC(0610),                    PDP10_ALL },
  { "tsn",              BASIC(0611),                    PDP10_ALL },
  { "tdne",             BASIC(0612),                    PDP10_ALL },
  { "tsne",             BASIC(0613),                    PDP10_ALL },
  { "tdna",             BASIC(0614),                    PDP10_ALL },
  { "tsna",             BASIC(0615),                    PDP10_ALL },
  { "tdnn",             BASIC(0616),                    PDP10_ALL },
  { "tsnn",             BASIC(0617),                    PDP10_ALL },
  { "trz",              BASIC(0620),                    PDP10_ALL },
  { "tlz",              BASIC(0621),                    PDP10_ALL },
  { "trze",             BASIC(0622),                    PDP10_ALL },
  { "tlze",             BASIC(0623),                    PDP10_ALL },
  { "trza",             BASIC(0624),                    PDP10_ALL },
  { "tlza",             BASIC(0625),                    PDP10_ALL },
  { "trzn",             BASIC(0626),                    PDP10_ALL },
  { "tlzn",             BASIC(0627),                    PDP10_ALL },
  { "tdz",              BASIC(0630),                    PDP10_ALL },
  { "tsz",              BASIC(0631),                    PDP10_ALL },
  { "tdze",             BASIC(0632),                    PDP10_ALL },
  { "tsze",             BASIC(0633),                    PDP10_ALL },
  { "tdza",             BASIC(0634),                    PDP10_ALL },
  { "tsza",             BASIC(0635),                    PDP10_ALL },
  { "tdzn",             BASIC(0636),                    PDP10_ALL },
  { "tszn",             BASIC(0637),                    PDP10_ALL },
  { "trc",              BASIC(0640),                    PDP10_ALL },
  { "tlc",              BASIC(0641),                    PDP10_ALL },
  { "trce",             BASIC(0642),                    PDP10_ALL },
  { "tlce",             BASIC(0643),                    PDP10_ALL },
  { "trca",             BASIC(0644),                    PDP10_ALL },
  { "tlca",             BASIC(0645),                    PDP10_ALL },
  { "trcn",             BASIC(0646),                    PDP10_ALL },
  { "tlcn",             BASIC(0647),                    PDP10_ALL },
  { "tdc",              BASIC(0650),                    PDP10_ALL },
  { "tsc",              BASIC(0651),                    PDP10_ALL },
  { "tdce",             BASIC(0652),                    PDP10_ALL },
  { "tsce",             BASIC(0653),                    PDP10_ALL },
  { "tdca",             BASIC(0654),                    PDP10_ALL },
  { "tsca",             BASIC(0655),                    PDP10_ALL },
  { "tdcn",             BASIC(0656),                    PDP10_ALL },
  { "tscn",             BASIC(0657),                    PDP10_ALL },
  { "tro",              BASIC(0660),                    PDP10_ALL },
  { "tlo",              BASIC(0661),                    PDP10_ALL },
  { "troe",             BASIC(0662),                    PDP10_ALL },
  { "tloe",             BASIC(0663),                    PDP10_ALL },
  { "troa",             BASIC(0664),                    PDP10_ALL },
  { "tloa",             BASIC(0665),                    PDP10_ALL },
  { "tron",             BASIC(0666),                    PDP10_ALL },
  { "tlon",             BASIC(0667),                    PDP10_ALL },
  { "tdo",              BASIC(0670),                    PDP10_ALL },
  { "tso",              BASIC(0671),                    PDP10_ALL },
  { "tdoe",             BASIC(0672),                    PDP10_ALL },
  { "tsoe",             BASIC(0673),                    PDP10_ALL },
  { "tdoa",             BASIC(0674),                    PDP10_ALL },
  { "tsoa",             BASIC(0675),                    PDP10_ALL },
  { "tdon",             BASIC(0676),                    PDP10_ALL },
  { "tson",             BASIC(0677),                    PDP10_ALL },

  /* IO and system instructions.  Lots of model-specifics here.  */
  { "aprid",            A_OPCODE(070000),               PDP10_KL10any | PDP10_KS10 | PDP10_XKL1 }, /* aliases BLKI APR, and APR0 */
  { "rsw",              A_OPCODE(070004),               PDP10_KA10_to_KI10 }, /* aliases DATAI APR, */
  { "wrfil",            A_OPCODE(070010),               PDP10_KL10any }, /* aliases BLKO APR, and SYSID */
  { "wrapr",            A_OPCODE(070020),               PDP10_KS10 | PDP10_XKL1 }, /* aliases CONO APR, and APR0 */
  { "rdapr",            A_OPCODE(070024),               PDP10_KS10 | PDP10_XKL1 }, /* aliases CONI APR, and APR0 */
  { "rdera",            A_OPCODE(070040),               PDP10_KL10any }, /* aliases BLKI PI, and WCTRLF */
  { "sbdiag",           A_OPCODE(070050),               PDP10_KL10any }, /* aliases BLKO PI, and SIMIRD */
  { "wrpi",             A_OPCODE(070060),               PDP10_KS10 | PDP10_XKL1 }, /* aliases CONO PI, and APR0 */
  { "rdpi",             A_OPCODE(070064),               PDP10_KS10 | PDP10_XKL1 }, /* aliases CONI PI, and APR0 */
  { "rdubr",            A_OPCODE(070104),               PDP10_KS10 | PDP10_XKL1 }, /* aliases DATAI PAG, and APR1 */
  { "clrpt",            A_OPCODE(070110),               PDP10_KL10any | PDP10_KS10 | PDP10_XKL1 }, /* aliases BLKO PAG, and APR1 */
  { "wrubr",            A_OPCODE(070114),               PDP10_KS10 | PDP10_XKL1 }, /* aliases DATAO PAG, and APR1 */
  { "wrebr",            A_OPCODE(070120),               PDP10_KS10 }, /* aliases CONO PAG, */
  { "rdebr",            A_OPCODE(070124),               PDP10_KS10 }, /* aliases CONI PAG, */
  { "swpia",            A_OPCODE(070144) | E_UNUSED,    PDP10_KL10any | PDP10_XKL1 }, /* aliases DATAI CCA, and APR1 */
  { "swpva",            A_OPCODE(070150) | E_UNUSED,    PDP10_KL10any | PDP10_XKL1 }, /* aliases BLKO CCA, and APR1 */
  { "swpua",            A_OPCODE(070154) | E_UNUSED,    PDP10_KL10any | PDP10_XKL1 }, /* aliases DATAO CCA, and APR1 */
  { "swpio",            A_OPCODE(070164),               PDP10_KL10any | PDP10_XKL1 }, /* aliases CONI CCA, and APR1 */
  { "swpvo",            A_OPCODE(070170),               PDP10_KL10any | PDP10_XKL1 }, /* aliases CONSZ CCA, and APR1 */
  { "swpuo",            A_OPCODE(070174),               PDP10_KL10any | PDP10_XKL1 }, /* aliases CONSO CCA, and APR1 */
  { "rdperf",           A_OPCODE(070200),               PDP10_KL10any }, /* aliases BLKI TIM, APR2, and RDSPB */
  { "rdspb",            A_OPCODE(070200),               PDP10_KS10 | PDP10_XKL1 }, /* aliases BLKI TIM, APR2, and RDPERF */
  { "rdtime",           A_OPCODE(070204),               PDP10_KL10any & ~PDP10_XKL1 }, /* aliases DATAI TIM, (FIXME: was KL10any) */
  { "rdcsb",            A_OPCODE(070204),               PDP10_KS10 | PDP10_XKL1 }, /* aliases DATAI TIM, and APR2 */
  { "wrpae",            A_OPCODE(070210),               PDP10_KL10any }, /* aliases BLKO TIM, and APR2 */
  { "rdpur",            A_OPCODE(070210),               PDP10_KS10 | PDP10_XKL1 }, /* aliases BLKO TIM, APR2, and WRPAE */
  { "rdcstm",           A_OPCODE(070214),               PDP10_KS10 | PDP10_XKL1 }, /* aliases APR2 */
  { "rdtim",            A_OPCODE(070220),               PDP10_KS10 }, /* aliases CONO TIM, */
  { "rdint",            A_OPCODE(070224),               PDP10_KS10 }, /* aliases CONI TIM, */
  { "rdhsb",            A_OPCODE(070230),               PDP10_KS10 }, /* aliases CONSZ */
  { "rdmact",           A_OPCODE(070240),               PDP10_KL10any }, /* aliases BLKI MTR, APR2, and RDMACT */
  { "wrspb",            A_OPCODE(070240),               PDP10_KS10 | PDP10_XKL1 }, /* aliases BLKI MTR, APR2, and RDMACT */
  { "rdeact",           A_OPCODE(070244),               PDP10_KL10any }, /* aliases DATAI MTR, APR2, and WRCSB */
  { "wrcsb",            A_OPCODE(070244),               PDP10_KS10 | PDP10_XKL1 }, /* aliases DATAI MTR, APR2, and RDEACT */
  { "wrpur",            A_OPCODE(070250),               PDP10_KS10 | PDP10_XKL1 }, /* aliases APR2 */
  { "wrcstm",           A_OPCODE(070254),               PDP10_KS10 | PDP10_XKL1 }, /* aliases APR2 */
  { "wrtime",           A_OPCODE(070260),               PDP10_KL10any }, /* aliases CONO MTR (FIXME: ?) */
  { "wrtim",            A_OPCODE(070260),               PDP10_KS10 },
  { "wrint",            A_OPCODE(070264),               PDP10_KS10 },
  { "wrhsb",            A_OPCODE(070270),               PDP10_KS10 },
  { "umove",            BASIC(0704),                    PDP10_KS10 },
  { "umovem",           BASIC(0705),                    PDP10_KS10 },
  { "tioe",             BASIC(0710),                    PDP10_KS10 },
  { "tion",             BASIC(0711),                    PDP10_KS10 },
  { "rdio",             BASIC(0712),                    PDP10_KS10 },
  { "wrio",             BASIC(0713),                    PDP10_KS10 },
  { "bsio",             BASIC(0714),                    PDP10_KS10 },
  { "bcio",             BASIC(0715),                    PDP10_KS10 },
  { "tioeb",            BASIC(0720),                    PDP10_KS10 },
  { "tionb",            BASIC(0721),                    PDP10_KS10 },
  { "rdiob",            BASIC(0722),                    PDP10_KS10 },
  { "wriob",            BASIC(0723),                    PDP10_KS10 },
  { "bsiob",            BASIC(0724),                    PDP10_KS10 },
  { "bciob",            BASIC(0725),                    PDP10_KS10 },

  /* KA10/KL10 ITS system instructions.  */
  { "lpm",              A_OPCODE(010200),               PDP10_KA10_ITS | PDP10_KL10_ITS },
  { "spm",              A_OPCODE(010204),               PDP10_KA10_ITS | PDP10_KL10_ITS },
  { "lpmr",             A_OPCODE(010210),               PDP10_KA10_ITS | PDP10_KL10_ITS },
  { "lpmri",            A_OPCODE(010230),               PDP10_KA10_ITS | PDP10_KL10_ITS },

  /* KA10 ITS system instructions.  */
  { "xctr",             A_OPCODE(010300),               PDP10_KA10_ITS },
  { "xctri",            A_OPCODE(010320),               PDP10_KA10_ITS },

  /* KL10 ITS system instructions.  */
  { "xctr",             BASIC(074),                     PDP10_KL10_ITS },
  { "xctri",            BASIC(075),                     PDP10_KL10_ITS },
  { "lpmr",             BASIC(076),                     PDP10_KL10_ITS },
  { "spm",              BASIC(077),                     PDP10_KL10_ITS },

  /* KS10 ITS system instructions.  */
  { "xctr",             BASIC(0102),                    PDP10_KS10_ITS },
  { "xctri",            BASIC(0103),                    PDP10_KS10_ITS },
  { "aprid",            A_OPCODE(070000),               PDP10_KS10_ITS }, /* aliases blki */

  /* ITS appears to prefer CONO and CONI over these mnemonics.  */
#if 0
  { "wrapr",            A_OPCODE(070020),               PDP10_KS10_ITS },
  { "rdapr",            A_OPCODE(070024),               PDP10_KS10_ITS },
  { "wrpi",             A_OPCODE(070060),               PDP10_KS10_ITS },
  { "rdpi",             A_OPCODE(070064),               PDP10_KS10_ITS },
#endif

  { "clrcsh",           A_OPCODE(070100),               PDP10_KS10_ITS }, /* aliases blki */
  { "rdubr",            A_OPCODE(070104),               PDP10_KS10_ITS }, /* aliases datai */
  { "clrpt",            A_OPCODE(070110),               PDP10_KS10_ITS }, /* aliases blko */
  { "wrubr",            A_OPCODE(070114),               PDP10_KS10_ITS }, /* aliases datao */
  { "wrebr",            A_OPCODE(070120),               PDP10_KS10_ITS }, /* aliases cono */
  { "rdebr",            A_OPCODE(070124),               PDP10_KS10_ITS }, /* aliases coni */
  { "sdbr1",            A_OPCODE(070200),               PDP10_KS10_ITS }, /* aliases blki */
  { "sdbr2",            A_OPCODE(070204),               PDP10_KS10_ITS }, /* aliases datai */
  { "sdbr3",            A_OPCODE(070210),               PDP10_KS10_ITS }, /* aliases blko */
  { "sdbr4",            A_OPCODE(070214),               PDP10_KS10_ITS }, /* aliases datao */
  { "rdtim",            A_OPCODE(070220),               PDP10_KS10_ITS }, /* aliases cono */
  { "rdint",            A_OPCODE(070224),               PDP10_KS10_ITS }, /* aliases coni */
  { "rdhsb",            A_OPCODE(070230),               PDP10_KS10_ITS }, /* aliases consz */
  { "spm",              A_OPCODE(070234),               PDP10_KS10_ITS }, /* aliases conso */
  { "ldbr1",            A_OPCODE(070240),               PDP10_KS10_ITS }, /* aliases blki */
  { "ldbr2",            A_OPCODE(070244),               PDP10_KS10_ITS }, /* aliases datai */
  { "ldbr3",            A_OPCODE(070250),               PDP10_KS10_ITS }, /* aliases blko */
  { "ldbr4",            A_OPCODE(070254),               PDP10_KS10_ITS }, /* aliases datao */
  { "wrtim",            A_OPCODE(070260),               PDP10_KS10_ITS }, /* aliases cono */
  { "wrint",            A_OPCODE(070264),               PDP10_KS10_ITS }, /* aliases coni */
  { "wrhsb",            A_OPCODE(070270),               PDP10_KS10_ITS }, /* aliases consz */
  { "lpmr",             A_OPCODE(070274),               PDP10_KS10_ITS }, /* aliases conso */
  { "umove",            BASIC(0704),                    PDP10_KS10_ITS }, /* aliases blki */
  { "umovem",           BASIC(0705),                    PDP10_KS10_ITS }, /* aliases blki */
  { "iordi",            BASIC(0710),                    PDP10_KS10_ITS }, /* aliases blki */
  { "iordq",            BASIC(0711),                    PDP10_KS10_ITS }, /* aliases blki */
  { "iord",             BASIC(0712),                    PDP10_KS10_ITS }, /* aliases blki */
  { "iowr",             BASIC(0713),                    PDP10_KS10_ITS }, /* aliases blki */
  { "iowri",            BASIC(0714),                    PDP10_KS10_ITS }, /* aliases blki */
  { "iowrq",            BASIC(0715),                    PDP10_KS10_ITS }, /* aliases blki */
  { "bltbu",            BASIC(0716),                    PDP10_KS10_ITS }, /* aliases blki */
  { "bltub",            BASIC(0717),                    PDP10_KS10_ITS }, /* aliases blki */
  { "iordbi",           BASIC(0720),                    PDP10_KS10_ITS }, /* aliases blki */
  { "iordbq",           BASIC(0721),                    PDP10_KS10_ITS }, /* aliases blki */
  { "iordb",            BASIC(0722),                    PDP10_KS10_ITS }, /* aliases blki */
  { "iowrb",            BASIC(0723),                    PDP10_KS10_ITS }, /* aliases blki */
  { "iowrbi",           BASIC(0724),                    PDP10_KS10_ITS }, /* aliases blki */
  { "iowrbq",           BASIC(0725),                    PDP10_KS10_ITS }, /* aliases blki */

  /* XKL-1 system instructions.  */
  { "rdadb",            A_OPCODE(070004),               PDP10_XKL1 }, /* special cases of apr0 */
  { "sysid",            A_OPCODE(070010),               PDP10_XKL1 }, /* aliases WRFIL */
  { "wradb",            A_OPCODE(070014),               PDP10_XKL1 },
  { "szapr",            A_OPCODE(070030),               PDP10_XKL1 },
  { "snapr",            A_OPCODE(070034),               PDP10_XKL1 },
  { "wctrlf",           A_OPCODE(070040),               PDP10_XKL1 }, /* aliases RDERA */
  { "rctrlf",           A_OPCODE(070044),               PDP10_XKL1 },
  { "simird",           A_OPCODE(070050),               PDP10_XKL1 }, /* aliases SBDIAG */
  { "wrkpa",            A_OPCODE(070054),               PDP10_XKL1 },
  { "szpi",             A_OPCODE(070070),               PDP10_XKL1 },
  { "snpi",             A_OPCODE(070074),               PDP10_XKL1 },
  { "apr0",             BASIC(0700),                    PDP10_XKL1 },
  { "wrerr",            A_OPCODE(070120),               PDP10_XKL1 }, /* special cases of apr1 */
  { "rderr",            A_OPCODE(070124),               PDP10_XKL1 },
  { "wrctx",            A_OPCODE(070130),               PDP10_XKL1 },
  { "rdctx",            A_OPCODE(070134),               PDP10_XKL1 },
  { "rddcsh",           A_OPCODE(070140),               PDP10_XKL1 },
  { "dwrcsh",           A_OPCODE(070160),               PDP10_XKL1 },
  { "apr1",             BASIC(0701),                    PDP10_XKL1 },
  { "rditm",            A_OPCODE(070220),               PDP10_XKL1 }, /* special cases of apr2 */
  { "rdtime",           A_OPCODE(070224),               PDP10_XKL1 },
  { "drdptb",           A_OPCODE(070230),               PDP10_XKL1 },
  { "wrtime",           A_OPCODE(070234),               PDP10_XKL1 },
  { "writm",            A_OPCODE(070260),               PDP10_XKL1 },
  { "dwrptb",           A_OPCODE(070270),               PDP10_XKL1 },
  { "apr2",             BASIC(0702),                    PDP10_XKL1 },
  { "rdcty",            A_OPCODE(070304),               PDP10_XKL1 }, /* special cases of apr3 */
  { "wrcty",            A_OPCODE(070314),               PDP10_XKL1 },
  { "wrctys",           A_OPCODE(070320),               PDP10_XKL1 },
  { "rdctys",           A_OPCODE(070324),               PDP10_XKL1 },
  { "szcty",            A_OPCODE(070330),               PDP10_XKL1 },
  { "sncty",            A_OPCODE(070334),               PDP10_XKL1 },
  { "apr3",             BASIC(0703),                    PDP10_XKL1 },
  { "pmove",            BASIC(0704),                    PDP10_XKL1 },
  { "pmovem",           BASIC(0705),                    PDP10_XKL1 },
  { "nmove",            BASIC(0706),                    PDP10_XKL1 },
  { "nmovem",           BASIC(0707),                    PDP10_XKL1 },
  { "ldlpn",            BASIC(0710),                    PDP10_XKL1 },
  { "rdcfg",            BASIC(0711),                    PDP10_XKL1 },
  { "amove",            BASIC(0714),                    PDP10_XKL1 },
  { "amovem",           BASIC(0715),                    PDP10_XKL1 },
  { "umove",            BASIC(0716),                    PDP10_XKL1 },
  { "umovem",           BASIC(0717),                    PDP10_XKL1 },

  /* PDP6 / KA10 / KI10 / KL10 IO instructions.
     These take an operand which determines which device to access.
     Most of the system instructions above are instances of these with
     predetermined values for the device operands.  */
  { "blki",             IO(070000),                     PDP10_not_KS10_or_XKL1 },
  { "datai",            IO(070004),                     PDP10_not_KS10_or_XKL1 },
  { "blko",             IO(070010),                     PDP10_not_KS10_or_XKL1 },
  { "datao",            IO(070014),                     PDP10_not_KS10_or_XKL1 },
  { "cono",             IO(070020),                     PDP10_not_KS10_or_XKL1 },
  { "coni",             IO(070024),                     PDP10_not_KS10_or_XKL1 },
  { "consz",            IO(070030),                     PDP10_not_KS10_or_XKL1 },
  { "conso",            IO(070034),                     PDP10_not_KS10_or_XKL1 },

  /* Extended instructions, second word.  */

  { "cmpsl",            EXTENDED(001) | E_UNUSED,       PDP10_KL10up },
  { "cmpse",            EXTENDED(002) | E_UNUSED,       PDP10_KL10up },
  { "cmpsle",           EXTENDED(003) | E_UNUSED,       PDP10_KL10up },
  { "edit",             EXTENDED(004) | E_UNUSED,       PDP10_KL10up },
  { "cmpsge",           EXTENDED(005) | E_UNUSED,       PDP10_KL10up },
  { "cmpsn",            EXTENDED(006) | E_UNUSED,       PDP10_KL10up },
  { "cmpsg",            EXTENDED(007) | E_UNUSED,       PDP10_KL10up },
  { "cvtdbo",           EXTENDED(010),                  PDP10_KL10up },
  { "cvtdbt",           EXTENDED(011),                  PDP10_KL10up },
  { "cvtbdo",           EXTENDED(012),                  PDP10_KL10up },
  { "cvtbdt",           EXTENDED(013),                  PDP10_KL10up },
  { "movso",            EXTENDED(014),                  PDP10_KL10up },
  { "movst",            EXTENDED(015),                  PDP10_KL10up },
  { "movslj",           EXTENDED(016) | E_UNUSED,       PDP10_KL10up },
  { "movsrj",           EXTENDED(017) | E_UNUSED,       PDP10_KL10up },
  { "xblt",             EXTENDED(020) | E_UNUSED,       PDP10_KL10up },
  { "gsngl",            EXTENDED(021),                  PDP10_KL10_271 },
  { "gdble",            EXTENDED(022),                  PDP10_KL10_271 },
  { "gdfix",            EXTENDED(023),                  PDP10_KL10_271 },
  { "gdfixr",           EXTENDED(025),                  PDP10_KL10_271 },
  { "gfix",             EXTENDED(024),                  PDP10_KL10_271 },
  { "gfixr",            EXTENDED(026),                  PDP10_KL10_271 },
  { "dgfltr",           EXTENDED(027),                  PDP10_KL10_271 },
  { "gfltr",            EXTENDED(030),                  PDP10_KL10_271 },
  { "gfsc",             EXTENDED(031),                  PDP10_KL10_271 },
};

const int pdp10_num_insns = ARRAY_SIZE (pdp10_insns);
