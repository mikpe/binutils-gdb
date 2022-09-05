/* Definitions for decoding the picoJava opcode table.
   Copyright (C) 1999 Free Software Foundation, Inc.
   Contributed by Steve Chamberlain of Transmeta (sac@pobox.com).

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


/* Names used to describe the type of instruction arguments, used by
   the assembler and disassembler.  Attributes are encoded in various fields. */

/*            reloc  size pcrel    uns */
#define O_N    0
#define O_16  (1<<4 | 2 | (0<<6) | (0<<3))
#define O_U16 (1<<4 | 2 | (0<<6) | (1<<3))
#define O_R16 (2<<4 | 2 | (1<<6) | (0<<3))
#define O_8   (3<<4 | 1 | (0<<6) | (0<<3))
#define O_U8  (3<<4 | 1 | (0<<6) | (1<<3))
#define O_R8  (4<<4 | 1 | (0<<6) | (0<<3))
#define O_R32 (5<<4 | 4 | (1<<6) | (0<<3))
#define O_32  (6<<4 | 4 | (0<<6) | (0<<3))

#define ASIZE(x)  ((x) & 0x7)
#define PCREL(x)  (!!((x) & (1<<6)))
#define UNS(x)    (!!((x) & (1<<3)))


void pj_opc_rev_init (unsigned char *revop);
                  
typedef struct 
{
  short opcode;
  short opcode_next;
  char len;
  unsigned char arg[2];
  const char *name;
} pj_opc_info_t;

#define NOP        0x00
#define ACONST_NULL 0x01
#define ICONST_M1  0x02
#define ICONST_0   0x03
#define ICONST_1   0x04
#define ICONST_2   0x05
#define ICONST_3   0x06
#define ICONST_4   0x07
#define ICONST_5   0x08
#define LCONST_0   0x09
#define LCONST_1   0x0a
#define FCONST_0   0x0b
#define FCONST_1   0x0c
#define FCONST_2   0x0d
#define DCONST_0   0x0e
#define DCONST_1   0x0f
#define BIPUSH     0x10
#define SIPUSH     0x11
#define LDC        0x12
#define LDC_W      0x13
#define LDC2_W     0x14
#define ILOAD      0x15
#define LLOAD      0x16
#define FLOAD      0x17
#define DLOAD      0x18
#define ALOAD      0x19
#define ILOAD_0    0x1a
#define ILOAD_1    0x1b
#define ILOAD_2    0x1c
#define ILOAD_3    0x1d
#define LLOAD_0    0x1e
#define LLOAD_1    0x1f
#define LLOAD_2    0x20
#define LLOAD_3    0x21
#define FLOAD_0    0x22
#define FLOAD_1    0x23
#define FLOAD_2    0x24
#define FLOAD_3    0x25
#define DLOAD_0    0x26
#define DLOAD_1    0x27
#define DLOAD_2    0x28
#define DLOAD_3    0x29
#define ALOAD_0    0x2a
#define ALOAD_1    0x2b
#define ALOAD_2    0x2c
#define ALOAD_3    0x2d
#define IALOAD     0x2e
#define LALOAD     0x2f
#define FALOAD     0x30
#define DALOAD     0x31
#define AALOAD     0x32
#define BALOAD     0x33
#define CALOAD     0x34
#define SALOAD     0x35
#define ISTORE     0x36
#define LSTORE     0x37
#define FSTORE     0x38
#define DSTORE     0x39
#define ASTORE     0x3a
#define ISTORE_0   0x3b
#define ISTORE_1   0x3c
#define ISTORE_2   0x3d
#define ISTORE_3   0x3e
#define LSTORE_0   0x3f
#define LSTORE_1   0x40
#define LSTORE_2   0x41
#define LSTORE_3   0x42
#define FSTORE_0   0x43
#define FSTORE_1   0x44
#define FSTORE_2   0x45
#define FSTORE_3   0x46
#define DSTORE_0   0x47
#define DSTORE_1   0x48
#define DSTORE_2   0x49
#define DSTORE_3   0x4a
#define ASTORE_0   0x4b
#define ASTORE_1   0x4c
#define ASTORE_2   0x4d
#define ASTORE_3   0x4e
#define IASTORE    0x4f
#define LASTORE    0x50
#define FASTORE    0x51
#define DASTORE    0x52
#define AASTORE    0x53
#define BASTORE    0x54
#define CASTORE    0x55
#define SASTORE    0x56
#define POP        0x57
#define POP2       0x58
#define DUP        0x59
#define DUP_X1     0x5a
#define DUP_X2     0x5b
#define DUP2       0x5c
#define DUP2_X1    0x5d
#define DUP2_X2    0x5e
#define SWAP       0x5f
#define IADD       0x60
#define LADD       0x61
#define FADD       0x62
#define DADD       0x63
#define ISUB       0x64
#define LSUB       0x65
#define FSUB       0x66
#define DSUB       0x67
#define IMUL       0x68
#define LMUL       0x69
#define FMUL       0x6a
#define DMUL       0x6b
#define IDIV       0x6c
#define LDIV       0x6d
#define FDIV       0x6e
#define DDIV       0x6f
#define IREM       0x70
#define LREM       0x71
#define FREM       0x72
#define DREM       0x73
#define INEG       0x74
#define LNEG       0x75
#define FNEG       0x76
#define DNEG       0x77
#define ISHL       0x78
#define LSHL       0x79
#define ISHR       0x7a
#define LSHR       0x7b
#define IUSHR      0x7c
#define LUSHR      0x7d
#define IAND       0x7e
#define LAND       0x7f
#define IOR        0x80
#define LOR        0x81
#define IXOR       0x82
#define LXOR       0x83
#define IINC       0x84
#define I2L        0x85
#define I2F        0x86
#define I2D        0x87
#define L2I        0x88
#define L2F        0x89
#define L2D        0x8a
#define F2I        0x8b
#define F2L        0x8c
#define F2D        0x8d
#define D2I        0x8e
#define D2L        0x8f
#define D2F        0x90
#define I2B        0x91
#define I2C        0x92
#define I2S        0x93
#define LCMP       0x94
#define FCMPL      0x95
#define FCMPG      0x96
#define DCMPL      0x97
#define DCMPG      0x98
#define IFEQ       0x99
#define IFNE       0x9a
#define IFLT       0x9b
#define IFGE       0x9c
#define IFGT       0x9d
#define IFLE       0x9e
#define IF_ICMPEQ  0x9f
#define IF_ICMPNE  0xa0
#define IF_ICMPLT  0xa1
#define IF_ICMPGE  0xa2
#define IF_ICMPGT  0xa3
#define IF_ICMPLE  0xa4
#define IF_ACMPEQ  0xa5
#define IF_ACMPNE  0xa6
#define GOTO       0xa7
#define JSR        0xa8
#define RET        0xa9
#define TABLESWITCH 0xaa
#define LOOKUPSWITCH 0xab
#define IRETURN    0xac
#define LRETURN    0xad
#define FRETURN    0xae
#define DRETURN    0xaf
#define ARETURN    0xb0
#define RETURN     0xb1
#define GETSTATIC  0xb2
#define PUTSTATIC  0xb3
#define GETFIELD   0xb4
#define PUTFIELD   0xb5
#define INVOKEVIRTUAL 0xb6
#define INVOKESPECIAL 0xb7
#define INVOKESTATIC 0xb8
#define INVOKEINTERFACE 0xb9
#define BAD_BA     0xba
#define NEW        0xbb
#define NEWARRAY   0xbc
#define ANEWARRAY  0xbd
#define ARRAYLENGTH 0xbe
#define ATHROW     0xbf
#define CHECKCAST  0xc0
#define INSTANCEOF 0xc1
#define MONITORENTER 0xc2
#define MONITOREXIT 0xc3
#define WIDE       0xc4
#define MULTIANEWARRAY 0xc5
#define IFNULL     0xc6
#define IFNONNULL  0xc7
#define GOTO_W     0xc8
#define JSR_W      0xc9
#define BREAKPOINT 0xca
#define BYTECODE   0xcb
#define TRY        0xcc
#define ENDTRY     0xcd
#define CATCH      0xce
#define VAR        0xcf
#define ENDVAR     0xd0
#define BAD_D1     0xd1
#define BAD_D2     0xd2
#define BAD_D3     0xd3
#define BAD_D4     0xd4
#define BAD_D5     0xd5
#define BAD_D6     0xd6
#define BAD_D7     0xd7
#define BAD_D8     0xd8
#define BAD_D9     0xd9
#define BAD_DA     0xda
#define BAD_DB     0xdb
#define AASTORE_QUICK 0xdc
#define BAD_DD     0xdd
#define BAD_DE     0xde
#define BAD_DF     0xdf
#define BAD_E0     0xe0
#define BAD_E1     0xe1
#define BAD_E2     0xe2
#define BAD_E3     0xe3
#define BAD_E4     0xe4
#define BAD_E5     0xe5
#define BAD_E6     0xe6
#define BAD_E7     0xe7
#define AGETSTATIC_QUICK 0xe8
#define BAD_E9     0xe9
#define BAD_EA     0xea
#define BAD_EB     0xeb
#define BAD_EC     0xec
#define SETHI      0xed
#define LOAD_WORD_JREFB 0xee
#define LOAD_SHORT_JREFB 0xef
#define LOAD_CHAR_JREFB 0xf0
#define LOAD_BYTE_JREFB 0xf1
#define LOAD_UBYTE_JREFB 0xf2
#define STORE_WORD_JREFB 0xf3
#define NA_STORE_WORD_JREFB 0xf4
#define STORE_SHORT_JREFB 0xf5
#define STORE_BYTE_JREFB 0xf6
#define BAD_F7     0xf7
#define BAD_F8     0xf8
#define BAD_F9     0xf9
#define BAD_FA     0xfa
#define BAD_FB     0xfb
#define BAD_FC     0xfc
#define BAD_FD     0xfd
#define BAD_FE     0xfe
#define LOAD_UBYTE 0xff
#define LOAD_BYTE  0xff
#define FF_LOAD_CHAR  0x02
#define FF_LOAD_SHORT 0x03
#define FF_LOAD_WORD  0x04
#define FF_PRIV_RET_FROM_TRAP 0x05
#define FF_PRIV_READ_DCACHE_TAG 0x06
#define FF_PRIV_READ_DCACHE_DATA 0x07
#define FF_LOAD_CHAR_OE 0x0a
#define FF_LOAD_SHORT_OE 0x0b
#define FF_LOAD_WORD_OE 0x0c
#define FF_RETURN0    0x0d
#define FF_PRIV_READ_ICACHE_TAG 0x0e
#define FF_PRIV_READ_ICACHE_DATA 0x0f
#define FF_NCLOAD_UBYTE 0x10
#define FF_NCLOAD_BYTE 0x11
#define FF_NCLOAD_CHAR 0x12
#define FF_NCLOAD_SHORT 0x13
#define FF_NCLOAD_WORD 0x14
#define FF_IUCMP      0x15
#define FF_PRIV_POWERDOWN 0x16
#define FF_CACHE_INVALIDATE 0x17
#define FF_NCLOAD_CHAR_OE 0x1a
#define FF_NCLOAD_SHORT_OE 0x1b
#define FF_NCLOAD_WORD_OE 0x1c
#define FF_RETURN1    0x1d
#define FF_CACHE_FLUSH 0x1e
#define FF_CACHE_INDEX_FLUSH 0x1f
#define FF_STORE_BYTE 0x20
#define FF_STORE_SHORT 0x22
#define FF_STORE_WORD 0x24
#define FF_SOFT_TRAP  0x25
#define FF_PRIV_WRITE_DCACHE_TAG 0x26
#define FF_PRIV_WRITE_DCACHE_DATA 0x27
#define FF_STORE_SHORT_OE 0x2a
#define FF_STORE_WORD_OE 0x2c
#define FF_RETURN2    0x2d
#define FF_PRIV_WRITE_ICACHE_TAG 0x2e
#define FF_PRIV_WRITE_ICACHE_DATA 0x2f
#define FF_NCSTORE_BYTE 0x30
#define FF_NCSTORE_SHORT 0x32
#define FF_NCSTORE_WORD 0x34
#define FF_PRIV_RESET 0x36
#define FF_GET_CURRENT_CLASS 0x37
#define FF_NCSTORE_SHORT_OE 0x3a
#define FF_NCSTORE_WORD_OE 0x3c
#define FF_CALL       0x3d
#define FF_ZERO_LINE  0x3e
#define FF_PRIV_UPDATE_OPTOP 0x3f
#define FF_READ_PC    0x40
#define FF_READ_VARS  0x41
#define FF_READ_FRAME 0x42
#define FF_READ_OPTOP 0x43
#define FF_PRIV_READ_OPLIM 0x44
#define FF_READ_CONST_POOL 0x45
#define FF_PRIV_READ_PSR 0x46
#define FF_PRIV_READ_TRAPBASE 0x47
#define FF_PRIV_READ_LOCKCOUNT0 0x48
#define FF_PRIV_READ_LOCKCOUNT1 0x49
#define FF_PRIV_READ_LOCKADDR0 0x4c
#define FF_PRIV_READ_LOCKADDR1 0x4d
#define FF_PRIV_READ_USERRANGE1 0x50
#define FF_PRIV_READ_GC_CONFIG 0x51
#define FF_PRIV_READ_BRK1A 0x52
#define FF_PRIV_READ_BRK2A 0x53
#define FF_PRIV_READ_BRK12C 0x54
#define FF_PRIV_READ_USERRANGE2 0x55
#define FF_PRIV_READ_VERSIONID 0x57
#define FF_PRIV_READ_HCR 0x58
#define FF_PRIV_READ_SC_BOTTOM 0x59
#define FF_READ_GLOBAL0 0x5a
#define FF_READ_GLOBAL1 0x5b
#define FF_READ_GLOBAL2 0x5c
#define FF_READ_GLOBAL3 0x5d
#define FF_WRITE_PC   0x60
#define FF_WRITE_VARS 0x61
#define FF_WRITE_FRAME 0x62
#define FF_WRITE_OPTOP 0x63
#define FF_PRIV_WRITE_OPLIM 0x64
#define FF_WRITE_CONST_POOL 0x65
#define FF_PRIV_WRITE_PSR 0x66
#define FF_PRIV_WRITE_TRAPBASE 0x67
#define FF_PRIV_WRITE_LOCKCOUNT0 0x68
#define FF_PRIV_WRITE_LOCKCOUNT1 0x69
#define FF_PRIV_WRITE_LOCKADDR0 0x6c
#define FF_PRIV_WRITE_LOCKADDR1 0x6d
#define FF_PRIV_WRITE_USERRANGE1 0x70
#define FF_PRIV_WRITE_GC_CONFIG 0x71
#define FF_PRIV_WRITE_BRK1A 0x72
#define FF_PRIV_WRITE_BRK2A 0x73
#define FF_PRIV_WRITE_BRK12C 0x74
#define FF_PRIV_WRITE_USERRANGE2 0x75
#define FF_PRIV_WRITE_SC_BOTTOM 0x79
#define FF_WRITE_GLOBAL0 0x7a
#define FF_WRITE_GLOBAL1 0x7b
#define FF_WRITE_GLOBAL2 0x7c
#define FF_WRITE_GLOBAL3 0x7d
#define FF_TM_CHECK_ARGS 0xae
#define FF_TM_EXIT    0xaf
#define FF_TM_TRAP    0xb0
#define FF_TM_MINFO   0xb1
#define FF_TM_NOT_IMPLEMENTED 0xb2
#define FF_READ_GLOBAL4 0xb3
#define FF_READ_GLOBAL5 0xb4
#define FF_READ_GLOBAL6 0xb5
#define FF_WRITE_GLOBAL4 0xb6
#define FF_WRITE_GLOBAL5 0xb7
#define FF_WRITE_GLOBAL6 0xb8
#define FF_TM_MOVSTRSI 0xb9
#define FF_TM_MEMSETSI 0xba
#define FF_TM_CMPSTRSI 0xbb
#define FF_TM_STRCPY  0xbc
#define FF_TM_STRCMP  0xbd
#define FF_TM_STRLENSI 0xbe
