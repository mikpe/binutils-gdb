/* Opcodes support for the RCA CDP1802 processor.
   Copyright (C) 2024 Free Software Foundation, Inc.
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
#include "opcode/cdp1802.h"

const struct cdp1802_opcode cdp1802_opcodes[] = {
  /* This array must be sorted alphabetically by NAME.  */

  {"adc",  0x74, ISA_1802, OPND_NONE}, /* ADD WITH CARRY  */
  {"adci", 0x7C, ISA_1802, OPND_IMM1}, /* ADD WITH CARRY IMMEDIATE  */
  {"add",  0xF4, ISA_1802, OPND_NONE}, /* ADD  */
  {"adi",  0xFC, ISA_1802, OPND_IMM1}, /* ADD IMMEDIATE  */
  {"and",  0xF2, ISA_1802, OPND_NONE}, /* AND  */
  {"ani",  0xFA, ISA_1802, OPND_IMM1}, /* AND IMMEDIATE  */

  {"b1",   0x34, ISA_1802, OPND_IMM1}, /* SHORT BRANCH IF EF1=1  */
  {"b2",   0x35, ISA_1802, OPND_IMM1}, /* SHORT BRANCH IF EF2=1  */
  {"b3",   0x36, ISA_1802, OPND_IMM1}, /* SHORT BRANCH IF EF3=1  */
  {"b4",   0x37, ISA_1802, OPND_IMM1}, /* SHORT BRANCH IF EF4=1  */
  {"bci",  0x3E, ISA_1804, OPND_IMM1}, /* 683E: SHORT BRANCH ON COUNTER INTERRUPT  */
  {"bdf",  0x33, ISA_1802, OPND_IMM1}, /* SHORT BRANCH IF DF=1  */
  {"bge",  0x33, ISA_1802, OPND_IMM1}, /* SHORT BRANCH EQUAL OR GREATER (alias for BDF)  */
  {"bl",   0x3B, ISA_1802, OPND_IMM1}, /* SHORT BRANCH IF LESS (alias for BNF)  */
  {"bm",   0x3B, ISA_1802, OPND_IMM1}, /* SHORT BRANCH IF MINUS (alias for BNF)  */
  {"bn1",  0x3C, ISA_1802, OPND_IMM1}, /* SHORT BRANCH IF EF1=0  */
  {"bn2",  0x3D, ISA_1802, OPND_IMM1}, /* SHORT BRANCH IF EF2=0  */
  {"bn3",  0x3E, ISA_1802, OPND_IMM1}, /* SHORT BRANCH IF EF3=0  */
  {"bn4",  0x3F, ISA_1802, OPND_IMM1}, /* SHORT BRANCH IF EF4=0  */
  {"bnf",  0x3B, ISA_1802, OPND_IMM1}, /* SHORT BRANCH IF DF=0  */
  {"bnq",  0x39, ISA_1802, OPND_IMM1}, /* SHORT BRANCH IF Q=0  */
  {"bnz",  0x3A, ISA_1802, OPND_IMM1}, /* SHORT BRANCH IF D NOT 0  */
  {"bpz",  0x33, ISA_1802, OPND_IMM1}, /* SHORT BRANCH POS OR ZERO (alias for BDF)  */
  {"bq",   0x31, ISA_1802, OPND_IMM1}, /* SHORT BRANCH IF Q=1  */
  {"br",   0x30, ISA_1802, OPND_IMM1}, /* UNCONDITIONAL SHORT BRANCH  */
  {"bxi",  0x3F, ISA_1804, OPND_IMM1}, /* 683F: SHORT BRANCH ON EXTERNAL INTERRUPT  */
  {"bz",   0x32, ISA_1802, OPND_IMM1}, /* SHORT BRANCH IF D=0  */

  {"cid",  0x0D, ISA_1804, OPND_NONE}, /* 680D: COUNTER INTERRUPT DISABLE  */
  {"cie",  0x0C, ISA_1804, OPND_NONE}, /* 680C: COUNTER INTERRUPT ENABLE  */

  {"daci", 0x7C, ISA_1804AC, OPND_IMM1}, /* 687C: DECIMAL ADD WITH CARRY, IMMEDIATE  */
  {"dadc", 0x74, ISA_1804AC, OPND_NONE}, /* 6874: DECIMAL ADD WITH CARRY  */
  {"dadd", 0xF4, ISA_1804AC, OPND_NONE}, /* 68F4: DECIMAL ADD  */
  {"dadi", 0xFC, ISA_1804AC, OPND_IMM1}, /* 68FC: DECIMAL ADD IMMEDIATE  */
  {"dbnz", 0x20, ISA_1804AC, OPND_N|OPND_IMM2|OPND_LAB}, /* 682N: DECREMENT REG N AND LONG BRANCH IF NOT EQUAL 0  */
  {"dec",  0x20, ISA_1802, OPND_N}, /* 2N: DECREMENT REG N  */
  {"dis",  0x71, ISA_1802, OPND_NONE}, /* DISABLE  */
  {"dsav", 0x76, ISA_1804AC, OPND_NONE}, /* 6876: SAVE T, D, DF  */
  {"dsbi", 0x7F, ISA_1804AC, OPND_IMM1}, /* 687F: DECIMAL SUBTRACT MEMORY WITH BORROW, IMMEDIATE  */
  {"dsm",  0xF7, ISA_1804AC, OPND_NONE}, /* 68F7: DECIMAL SUBTRACT MEMORY  */
  {"dsmb", 0x77, ISA_1804AC, OPND_NONE}, /* 6877: DECIMAL SUBTRACT MEMORY WITH BORROW  */
  {"dsmi", 0xFF, ISA_1804AC, OPND_IMM1}, /* 68FF: DECIMAL SUBTRACT MEMORY, IMMEDIATE  */
  {"dtc",  0x01, ISA_1804, OPND_NONE}, /* 6801: DECREMENT COUNTER  */

  {"etq",  0x09, ISA_1804, OPND_NONE}, /* 6809: ENABLE TOGGLE Q  */

  {"gec",  0x08, ISA_1804, OPND_NONE}, /* 6808: GET COUNTER  */
  {"ghi",  0x90, ISA_1802, OPND_N}, /* 9N: GET HIGH REG N  */
  {"glo",  0x80, ISA_1802, OPND_N}, /* 8N: GET LOW REG N  */

  {"idl",  0x00, ISA_1802, OPND_NONE}, /* IDLE  */
  {"inc",  0x10, ISA_1802, OPND_N}, /* 1N: INCREMENT REG N  */
  {"inp",  0x68, ISA_1802, OPND_N|OPND_PORT}, /* 6N: INPUT, N=9-F  */
  {"irx",  0x60, ISA_1802, OPND_NONE}, /* INCREMENT REG X  */

  {"lbdf", 0xC3, ISA_1802, OPND_IMM2}, /* LONG BRANCH IF DF=1  */
  {"lbnf", 0xCB, ISA_1802, OPND_IMM2}, /* LONG BRANCH IF DF=0  */
  {"lbnq", 0xC9, ISA_1802, OPND_IMM2}, /* LONG BRANCH IF Q=0  */
  {"lbnz", 0xCA, ISA_1802, OPND_IMM2}, /* LONG BRANCH IF D NOT 0  */
  {"lbq",  0xC1, ISA_1802, OPND_IMM2}, /* LONG BRANCH IF Q=1  */
  {"lbr",  0xC0, ISA_1802, OPND_IMM2}, /* LONG BRANCH  */
  {"lbz",  0xC2, ISA_1802, OPND_IMM2}, /* LONG BRANCH IF D=0  */
  {"lda",  0x40, ISA_1802, OPND_N}, /* 4N: LOAD ADVANCE  */
  {"ldc",  0x06, ISA_1804, OPND_NONE}, /* 6808: LOAD COUNTER  */
  {"ldi",  0xF8, ISA_1802, OPND_IMM1}, /* LOAD IMMEDIATE  */
  {"ldn",  0x00, ISA_1802, OPND_N}, /* 0N: LOAD VIA N, N NOT 0  */
  {"ldx",  0xF0, ISA_1802, OPND_NONE}, /* LOAD VIA X  */
  {"ldxa", 0x72, ISA_1802, OPND_NONE}, /* LOAD VIA X AND ADVANCE  */
  {"lsdf", 0xCF, ISA_1802, OPND_NONE}, /* LONG SKIP IF DF=1  */
  {"lsie", 0xCC, ISA_1802, OPND_NONE}, /* LONG SKIP IF IE=1  */
  {"lskp", 0xC8, ISA_1802, OPND_NONE}, /* LONG SKIP (alias for NLBR)  */
  {"lsnf", 0xC7, ISA_1802, OPND_NONE}, /* LONG SKIP IF DF=0  */
  {"lsnq", 0xC5, ISA_1802, OPND_NONE}, /* LONG SKIP IF Q=0  */
  {"lsnz", 0xC6, ISA_1802, OPND_NONE}, /* LONG SKIP IF D NOT 0  */
  {"lsq",  0xCD, ISA_1802, OPND_NONE}, /* LONG SKIP IF Q=1  */
  {"lsz",  0xCE, ISA_1802, OPND_NONE}, /* LONG SKIP IF D=0  */

  {"mark", 0x79, ISA_1802, OPND_NONE}, /* PUSH X, P TO STACK  */

  {"nbr",  0x38, ISA_1802, OPND_NONE}, /* NO SHORT BRANCH  */
  {"nlbr", 0xC8, ISA_1802, OPND_NONE}, /* NO LONG BRANCH  */
  {"nop",  0xC4, ISA_1802, OPND_NONE}, /* NO OPERATION  */

  {"or",   0xF1, ISA_1802, OPND_NONE}, /* OR  */
  {"ori",  0xF9, ISA_1802, OPND_IMM1}, /* OR IMMEDIATE  */
  {"out",  0x60, ISA_1802, OPND_N|OPND_PORT}, /* OUTPUT, 6N, N=1-7  */

  {"phi",  0xB0, ISA_1802, OPND_N}, /* BN: PUT HIGH REG N  */
  {"plo",  0xA0, ISA_1802, OPND_N}, /* AN: PUT LOW REG N  */

  {"req",  0x7A, ISA_1802, OPND_NONE}, /* RESET Q  */
  {"ret",  0x70, ISA_1802, OPND_NONE}, /* RETURN  */
  {"rldi", 0xC0, ISA_1804, OPND_N|OPND_IMM2}, /* 68CN: REGISTER LOAD IMMEDIATE  */
  {"rlxa", 0x60, ISA_1804, OPND_N}, /* 686N: REGISTER LOAD VIA X AND ADVANCE  */
  {"rnx",  0xB0, ISA_1804, OPND_N}, /* 68BN: REGISTER N TO REGISTER X COPY  */
  {"rshl", 0x7E, ISA_1802, OPND_NONE}, /* RING_SHIFT LEFT (alias for SHLC)  */
  {"rshr", 0x76, ISA_1802, OPND_NONE}, /* RING SHIFT RIGHT (alias for SHRC)  */
  {"rsxd", 0xA0, ISA_1804, OPND_N}, /* 68AN: REGISTER STORE VIA X AND DECREMENT  */

  {"sav",  0x78, ISA_1802, OPND_NONE}, /* SAVE  */
  {"scal", 0x80, ISA_1804, OPND_N}, /* 688N: STANDARD CALL  */
  {"scm1", 0x05, ISA_1804, OPND_NONE}, /* 6805: SET COUNTER MODE 1 AND START  */
  {"scm2", 0x03, ISA_1804, OPND_NONE}, /* 6803: SET COUNTER MODE 2 AND START  */
  {"sd",   0xF5, ISA_1802, OPND_NONE}, /* SUBTRACT D  */
  {"sdb",  0x75, ISA_1802, OPND_NONE}, /* SUBTRACT D WITH BORROW  */
  {"sdbi", 0x7D, ISA_1802, OPND_IMM1}, /* SUBTRACT D WITH BORROW IMMEDIATE  */
  {"sdi",  0xFD, ISA_1802, OPND_IMM1}, /* SUBTRACT D IMMEDIATE  */
  {"sep",  0xD0, ISA_1802, OPND_N}, /* DN: SET P  */
  {"seq",  0x7B, ISA_1802, OPND_NONE}, /* SET Q  */
  {"sex",  0xE0, ISA_1802, OPND_N}, /* EN: SET X  */
  {"shl",  0xFE, ISA_1802, OPND_NONE}, /* SHIFT LEFT  */
  {"shlc", 0x7E, ISA_1802, OPND_NONE}, /* SHIFT LEFT WITH CARRY  */
  {"shr",  0xF6, ISA_1802, OPND_NONE}, /* SHIFT RIGHT  */
  {"shrc", 0x76, ISA_1802, OPND_NONE}, /* SHIFT RIGHT WITH CARRY  */
  {"skp",  0x38, ISA_1802, OPND_NONE}, /* SHORT SKIP (alias for NBR)  */
  {"sm",   0xF7, ISA_1802, OPND_NONE}, /* SUBTRACT MEMORY  */
  {"smb",  0x77, ISA_1802, OPND_NONE}, /* SUBTRACT MEMORY WITH BORROW  */
  {"smbi", 0x7F, ISA_1802, OPND_IMM1}, /* SUBTRACT MEMORY WITH BORROW IMMEDIATE  */
  {"smi",  0xFF, ISA_1802, OPND_IMM1}, /* SUBTRACT MEMORY IMMEDIATE  */
  {"spm1", 0x04, ISA_1804, OPND_NONE}, /* 6804: SET PULSE WIDTH MODE 1, START  */
  {"spm2", 0x02, ISA_1804, OPND_NONE}, /* 6802: SET PULSE WIDTH MODE 2, START  */
  {"sret", 0x90, ISA_1804, OPND_N}, /* 689N: STANDARD RETURN  */
  {"stm",  0x07, ISA_1804, OPND_NONE}, /* 6807: SET TIMER MODE AND START  */
  {"stpc", 0x00, ISA_1804, OPND_NONE}, /* 6800: STOP COUNTER  */
  {"str",  0x50, ISA_1802, OPND_N}, /* 5N: STORE VIA N  */
  {"stxd", 0x73, ISA_1802, OPND_NONE}, /* STORE VIA X AND DECREMENT  */

  {"xid",  0x0B, ISA_1804, OPND_NONE}, /* 680B: EXTERNAL INTERRUPT DISABLE  */
  {"xie",  0x0A, ISA_1804, OPND_NONE}, /* 680A: EXTERNAL INTERRUPT ENABLE  */
  {"xor",  0xF3, ISA_1802, OPND_NONE}, /* EXCLUSIVE-OR  */
  {"xri",  0xFB, ISA_1802, OPND_IMM1}, /* EXCLUSIVE-OR IMMEDIATE  */
};

const unsigned int cdp1802_nr_opcodes = sizeof (cdp1802_opcodes) / sizeof (cdp1802_opcodes[0]);

const struct cdp1802_opcode *
cdp1802_lookup_opcode (const char *name, size_t len)
{
  unsigned int low, high, mid;
  int cmp;

  if (len > 4)
    return NULL;

  low = 0;
  high = sizeof (cdp1802_opcodes) / sizeof (cdp1802_opcodes[0]);

  while (low < high)
    {
      mid = (low + high) / 2;
      cmp = strncasecmp (name, cdp1802_opcodes[mid].name, len);
      if (cmp == 0)
	{
	  if (strnlen (cdp1802_opcodes[mid].name, 4) == len)
	    return &cdp1802_opcodes[mid];
	  else
	    high = mid;
	}
      else if (cmp < 0)
	high = mid;
      else
	low = mid + 1;
    }

  return NULL;
}
