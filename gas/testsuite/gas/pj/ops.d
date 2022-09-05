#objdump: -dr
#name: pj
#as

# test all the instructions

.*: +file format elf32-pj.*

Disassembly of section .text:

00000000 <foo0-0x1>:
	...

00000001 <foo0>:
	...

00000002 <foo1>:
	...

00000003 <foo2>:
	...

00000004 <foo3>:
	...

00000005 <foo4>:
	...

00000006 <foo5>:
	...

00000007 <foo6>:
	...

00000008 <foo7>:
	...

00000009 <foo8>:
	...

0000000a <foo9>:
	...

0000000b <foo10>:
	...

0000000c <foo11>:
	...

0000000d <foo12>:
	...

0000000e <foo13>:
	...

0000000f <foo14>:
	...

00000010 <foo15>:
	...

00000011 <foo16>:
	...

00000012 <foo17>:
	...

00000013 <foo18>:
	...

00000014 <foo19>:
  14:	00                      	nop
  15:	01                      	aconst_null
  16:	02                      	iconst_m1
  17:	03                      	iconst_0
  18:	04                      	iconst_1
  19:	05                      	iconst_2
  1a:	06                      	iconst_3
  1b:	07                      	iconst_4
  1c:	08                      	iconst_5
  1d:	09                      	lconst_0
  1e:	0a                      	lconst_1
  1f:	0b                      	fconst_0
  20:	0c                      	fconst_1
  21:	0d                      	fconst_2
  22:	0e                      	dconst_0
  23:	0f                      	dconst_1
  24:	10 e7                   	bipush	-25
  26:	11 a3 c6                	sipush	-23610
  29:	12 00                   	ldc
  2b:	13 00 00                	ldc_w
  2e:	14 00 00                	ldc2_w
  31:	15 69                   	iload	105
  33:	16 73                   	lload	115
  35:	17 51                   	fload	81
  37:	18 ff                   	dload	255
  39:	19 4a                   	aload	74
  3b:	1a                      	iload_0
  3c:	1b                      	iload_1
  3d:	1c                      	iload_2
  3e:	1d                      	iload_3
  3f:	1e                      	lload_0
  40:	1f                      	lload_1
  41:	20                      	lload_2
  42:	21                      	lload_3
  43:	22                      	fload_0
  44:	23                      	fload_1
  45:	24                      	fload_2
  46:	25                      	fload_3
  47:	26                      	dload_0
  48:	27                      	dload_1
  49:	28                      	dload_2
  4a:	29                      	dload_3
  4b:	2a                      	aload_0
  4c:	2b                      	aload_1
  4d:	2c                      	aload_2
  4e:	2d                      	aload_3
  4f:	2e 00 00                	iaload
  52:	2f 00 00                	laload
  55:	30 00 00                	faload
  58:	31 00 00                	daload
  5b:	32 00 00                	aaload
  5e:	33                      	baload
  5f:	34                      	caload
  60:	35                      	saload
  61:	36 ec                   	istore	236
  63:	37 29                   	lstore	41
  65:	38 cd                   	fstore	205
  67:	39 ba                   	dstore	186
  69:	3a ab                   	astore	171
  6b:	3b                      	istore_0
  6c:	3c                      	istore_1
  6d:	3d                      	istore_2
  6e:	3e                      	istore_3
  6f:	3f                      	lstore_0
  70:	40                      	lstore_1
  71:	41                      	lstore_2
  72:	42                      	lstore_3
  73:	43                      	fstore_0
  74:	44                      	fstore_1
  75:	45                      	fstore_2
  76:	46                      	fstore_3
  77:	47                      	dstore_0
  78:	48                      	dstore_1
  79:	49                      	dstore_2
  7a:	4a                      	dstore_3
  7b:	4b                      	astore_0
  7c:	4c                      	astore_1
  7d:	4d                      	astore_2
  7e:	4e                      	astore_3
  7f:	4f                      	iastore
  80:	50                      	lastore
  81:	51                      	fastore
  82:	52                      	dastore
  83:	53                      	aastore
  84:	54                      	bastore
  85:	55                      	castore
  86:	56                      	sastore
  87:	57                      	pop
  88:	58                      	pop2
  89:	59                      	dup
  8a:	5a                      	dup_x1
  8b:	5b                      	dup_x2
  8c:	5c                      	dup2
  8d:	5d                      	dup2_x1
  8e:	5e                      	dup2_x2
  8f:	5f                      	swap
  90:	60                      	iadd
  91:	61                      	ladd
  92:	62                      	fadd
  93:	63                      	dadd
  94:	64                      	isub
  95:	65                      	lsub
  96:	66                      	fsub
  97:	67                      	dsub
  98:	68                      	imul
  99:	69                      	lmul
  9a:	6a                      	fmul
  9b:	6b                      	dmul
  9c:	6c                      	idiv
  9d:	6d                      	ldiv
  9e:	6e                      	fdiv
  9f:	6f                      	ddiv
  a0:	70                      	irem
  a1:	71                      	lrem
  a2:	72                      	frem
  a3:	73                      	drem
  a4:	74                      	ineg
  a5:	75                      	lneg
  a6:	76                      	fneg
  a7:	77                      	dneg
  a8:	78                      	ishl
  a9:	79                      	lshl
  aa:	7a                      	ishr
  ab:	7b                      	lshr
  ac:	7c                      	iushr
  ad:	7d                      	lushr
  ae:	7e                      	iand
  af:	7f                      	land
  b0:	80                      	ior
  b1:	81                      	lor
  b2:	82                      	ixor
  b3:	83                      	lxor
  b4:	84 f2 7b                	iinc	242,123
  b7:	85                      	i2l
  b8:	86                      	i2f
  b9:	87                      	i2d
  ba:	88                      	l2i
  bb:	89                      	l2f
  bc:	8a                      	l2d
  bd:	8b                      	f2i
  be:	8c                      	f2l
  bf:	8d                      	f2d
  c0:	8e                      	d2i
  c1:	8f                      	d2l
  c2:	90                      	d2f
  c3:	91                      	i2b
  c4:	92                      	i2c
  c5:	93                      	i2s
  c6:	94                      	lcmp
  c7:	95                      	fcmpl
  c8:	96                      	fcmpg
  c9:	97                      	dcmpl
  ca:	98                      	dcmpg
  cb:	9a 00 08 c8 ff ff ff 34 	ifeq	2 <foo1>
  d3:	99 00 08 c8 ff ff ff 35 	ifne	b <foo10>
  db:	9c 00 08 c8 ff ff ff 2a 	iflt	8 <foo7>
  e3:	9b 00 08 c8 ff ff ff 2c 	ifge	12 <foo17>
  eb:	9e 00 08 c8 ff ff ff 18 	ifgt	6 <foo5>
  f3:	9d 00 08 c8 ff ff ff 14 	ifle	a <foo9>
  fb:	a0 00 08 c8 ff ff ff 0d 	if_icmpeq	b <foo10>
 103:	9f 00 08 c8 ff ff ff 0a 	if_icmpne	10 <foo15>
 10b:	a2 00 08 c8 ff ff fe fb 	if_icmplt	9 <foo8>
 113:	a1 00 08 c8 ff ff fe f7 	if_icmpge	d <foo12>
 11b:	a4 00 08 c8 ff ff fe ea 	if_icmpgt	8 <foo7>
 123:	a3 00 08 c8 ff ff fe e7 	if_icmple	d <foo12>
 12b:	a6 00 08 c8 ff ff fe d9 	if_acmpeq	7 <foo6>
 133:	a5 00 08 c8 ff ff fe d2 	if_acmpne	8 <foo7>
 13b:	c8 ff ff fe cb          	goto_w	6 <foo5>
 140:	c9 ff ff fe c2          	jsr_w	2 <foo1>
 145:	a9 c9                   	ret	201
 147:	aa ff ff fe be 00 00 00 	tableswitch 1..5 default: 5 <foo4>
	case     1:	\[pc\+fffffecb = 12 <foo17>\]
	case     2:	\[pc\+fffffec0 = 7 <foo6>\]
	case     3:	\[pc\+fffffebe = 5 <foo4>\]
	case     4:	\[pc\+fffffebf = 6 <foo5>\]
	case     5:	\[pc\+fffffec8 = f <foo14>\]
 14f:	01 00 00 00 05 ff ff fe 
 157:	cb ff ff fe c0 ff ff fe 
 15f:	be ff ff fe bf ff ff fe 
 167:	c8 
 168:	ab 00 00 00 ff ff fe a8 	lookupswitch default: 10 <foo15> 7:\[e <foo13> \] 37:\[11 <foo16> \]
 170:	00 00 00 02 00 00 00 07 
 178:	ff ff fe a6 00 00 00 25 
 180:	ff ff fe a9 
 184:	ac                      	ireturn
 185:	ad                      	lreturn
 186:	ae                      	freturn
 187:	af                      	dreturn
 188:	b0                      	areturn
 189:	b1                      	return
 18a:	b2 00 00                	getstatic
 18d:	b3 00 00                	putstatic
 190:	b4 00 00                	getfield
 193:	b5 00 00                	putfield
 196:	b6 00 00                	invokevirtual
 199:	b7 00 00                	invokespecial
 19c:	b8 00 00                	invokestatic
 19f:	b9 00 00 00 00          	invokeinterface
 1a4:	bb 00 00                	new
 1a7:	bc 00                   	newarray
 1a9:	bd 00 00                	anewarray
 1ac:	be                      	arraylength
 1ad:	bf                      	athrow
 1ae:	c0 00 00                	checkcast
 1b1:	c1 00 00                	instanceof
 1b4:	c2                      	monitorenter
 1b5:	c3                      	monitorexit
 1b6:	c4                      	wide
 1b7:	c5 00 00 00             	multianewarray
 1bb:	c6 00 00                	ifnull
 1be:	c7 00 00                	ifnonnull
 1c1:	c8 ff ff fe 40          	goto_w	1 <foo0>
 1c6:	c9 ff ff fe 49          	jsr_w	f <foo14>
 1cb:	ca                      	breakpoint
 1cc:	cb                      	bytecode
 1cd:	cc                      	try
 1ce:	cd                      	endtry
 1cf:	ce                      	catch
 1d0:	cf                      	var
 1d1:	d0                      	endvar
 1d2:	dc                      	aastore_quick
 1d3:	e8                      	agetstatic_quick	45347
 1d4:	b1                      	return
 1d5:	23                      	fload_1
 1d6:	17 ee                   	fload	238
 1d8:	58                      	pop2
 1d9:	69                      	lmul
 1da:	ef 5e 54                	load_short_index	94,84
 1dd:	f0 ab 32                	load_char_index	171,50
 1e0:	f1 cd 46                	load_byte_index	205,70
 1e3:	f2 9b 34                	load_ubyte_index	155,52
 1e6:	f3 54 91                	store_word_index	84,-111
 1e9:	f4 0e 02                	na_store_word_index	14,2
 1ec:	f5 74 c1                	store_short_index	116,-63
 1ef:	f6 21 bd                	store_byte_index	33,-67
 1f2:	ff 00                   	load_ubyte	
 1f4:	ff 01                   	load_byte	
 1f6:	ff 02                   	load_char	
 1f8:	ff 03                   	load_short	
 1fa:	ff 04                   	load_word	
 1fc:	ff 05                   	priv_ret_from_trap	
 1fe:	ff 06                   	priv_read_dcache_tag	
 200:	ff 07                   	priv_read_dcache_data	
 202:	ff 0a                   	load_char_oe	
 204:	ff 0b                   	load_short_oe	
 206:	ff 0c                   	load_word_oe	
 208:	ff 0d                   	return0	
 20a:	ff 0e                   	priv_read_icache_tag	
 20c:	ff 0f                   	priv_read_icache_data	
 20e:	ff 10                   	ncload_ubyte	
 210:	ff 11                   	ncload_byte	
 212:	ff 12                   	ncload_char	
 214:	ff 13                   	ncload_short	
 216:	ff 14                   	ncload_word	
 218:	ff 15                   	iucmp	
 21a:	ff 16                   	priv_powerdown	
 21c:	ff 17                   	cache_invalidate	
 21e:	ff 1a                   	ncload_char_oe	
 220:	ff 1b                   	ncload_short_oe	
 222:	ff 1c                   	ncload_word_oe	
 224:	ff 1d                   	return1	
 226:	ff 1e                   	cache_flush	
 228:	ff 1f                   	cache_index_flush	
 22a:	ff 20                   	store_byte	
 22c:	ff 22                   	store_short	
 22e:	ff 24                   	store_word	
 230:	ff 25                   	soft_trap	
 232:	ff 26                   	priv_write_dcache_tag	
 234:	ff 27                   	priv_write_dcache_data	
 236:	ff 2a                   	store_short_oe	
 238:	ff 2c                   	store_word_oe	
 23a:	ff 2d                   	return2	
 23c:	ff 2e                   	priv_write_icache_tag	
 23e:	ff 2f                   	priv_write_icache_data	
 240:	ff 30                   	ncstore_byte	
 242:	ff 32                   	ncstore_short	
 244:	ff 34                   	ncstore_word	
 246:	ff 36                   	priv_reset	
 248:	ff 37                   	get_current_class	
 24a:	ff 3a                   	ncstore_short_oe	
 24c:	ff 3c                   	ncstore_word_oe	
 24e:	ff 3d                   	call	
 250:	ff 3e                   	zero_line	
 252:	ff 3f                   	priv_update_optop	
 254:	ff 40                   	read_pc	
 256:	ff 41                   	read_vars	
 258:	ff 42                   	read_frame	
 25a:	ff 43                   	read_optop	
 25c:	ff 44                   	priv_read_oplim	
 25e:	ff 45                   	read_const_pool	
 260:	ff 46                   	priv_read_psr	
 262:	ff 47                   	priv_read_trapbase	
 264:	ff 48                   	priv_read_lockcount0	
 266:	ff 49                   	priv_read_lockcount1	
 268:	ff 4c                   	priv_read_lockaddr0	
 26a:	ff 4d                   	priv_read_lockaddr1	
 26c:	ff 50                   	priv_read_userrange1	
 26e:	ff 51                   	priv_read_gc_config	
 270:	ff 52                   	priv_read_brk1a	
 272:	ff 53                   	priv_read_brk2a	
 274:	ff 54                   	priv_read_brk12c	
 276:	ff 55                   	priv_read_userrange2	
 278:	ff 57                   	priv_read_versionid	
 27a:	ff 58                   	priv_read_hcr	
 27c:	ff 59                   	priv_read_sc_bottom	
 27e:	ff 5a                   	read_global0	
 280:	ff 5b                   	read_global1	
 282:	ff 5c                   	read_global2	
 284:	ff 5d                   	read_global3	
 286:	ff 60                   	write_pc	
 288:	ff 61                   	write_vars	
 28a:	ff 62                   	write_frame	
 28c:	ff 63                   	write_optop	
 28e:	ff 64                   	priv_write_oplim	
 290:	ff 65                   	write_const_pool	
 292:	ff 66                   	priv_write_psr	
 294:	ff 67                   	priv_write_trapbase	
 296:	ff 68                   	priv_write_lockcount0	
 298:	ff 69                   	priv_write_lockcount1	
 29a:	ff 6c                   	priv_write_lockaddr0	
 29c:	ff 6d                   	priv_write_lockaddr1	
 29e:	ff 70                   	priv_write_userrange1	
 2a0:	ff 71                   	priv_write_gc_config	
 2a2:	ff 72                   	priv_write_brk1a	
 2a4:	ff 73                   	priv_write_brk2a	
 2a6:	ff 74                   	priv_write_brk12c	
 2a8:	ff 75                   	priv_write_userrange2	
 2aa:	ff 79                   	priv_write_sc_bottom	
 2ac:	ff 7a                   	write_global0	
 2ae:	ff 7b                   	write_global1	
 2b0:	ff 7c                   	write_global2	
 2b2:	ff 7d                   	write_global3	
 2b4:	ff ae                   	tm_check_args	
 2b6:	ff af                   	tm_exit	
 2b8:	ff b0                   	tm_trap	
 2ba:	ff b1                   	tm_minfo	
 2bc:	ff b2                   	tm_not_implemented	
 2be:	ff b3                   	read_global4	
 2c0:	ff b4                   	read_global5	
 2c2:	ff b5                   	read_global6	
 2c4:	ff b6                   	write_global4	
 2c6:	ff b7                   	write_global5	
 2c8:	ff b8                   	write_global6	
 2ca:	ff b9                   	tm_movstrsi	
 2cc:	ff ba                   	tm_memsetsi	
 2ce:	ff bb                   	tm_cmpstrsi	
 2d0:	ff bc                   	tm_strcpy	
 2d2:	ff bd                   	tm_strcmp	
 2d4:	ff be                   	tm_strlensi	
 2d6:	ff bf                   	tm_frame	
 2d8:	ff c0                   	tm_load_long	
 2da:	ff c1                   	tm_store_long	
