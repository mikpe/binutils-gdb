#objdump: -d

.*\.o:     file format elf32-cdp1802


Disassembly of section .text:

00000000 <.text>:
   0:	74          	adc
   1:	7c 42       	adci	0x42
   3:	f4          	add
   4:	fc 42       	adi	0x42
   6:	f2          	and
   7:	fa 42       	ani	0x42
   9:	34 42       	b1	0x42
   b:	35 42       	b2	0x42
   d:	36 42       	b3	0x42
   f:	37 42       	b4	0x42
  11:	68 3e 42    	bci	0x42
  14:	33 42       	bpz	0x42
  16:	33 42       	bpz	0x42
  18:	3b 42       	bnf	0x42
  1a:	3b 42       	bnf	0x42
  1c:	3c 42       	bn1	0x42
  1e:	3d 42       	bn2	0x42
  20:	3e 42       	bn3	0x42
  22:	3f 42       	bn4	0x42
  24:	3b 42       	bnf	0x42
  26:	39 42       	bnq	0x42
  28:	3a 42       	bnz	0x42
  2a:	33 42       	bpz	0x42
  2c:	31 42       	bq	0x42
  2e:	30 42       	br	0x42
  30:	68 3f 42    	bxi	0x42
  33:	32 42       	bz	0x42
  35:	68 0d       	cid
  37:	68 0c       	cie
  39:	68 7c 42    	daci	0x42
  3c:	68 74       	dadc
  3e:	68 f4       	dadd
  40:	68 fc 42    	dadi	0x42
  43:	68 27 47 11 	dbnz	7,0x4711
  47:	27          	dec	7
  48:	71          	dis
  49:	68 76       	dsav
  4b:	68 7f 42    	dsbi	0x42
  4e:	68 f7       	dsm
  50:	68 77       	dsmb
  52:	68 ff 42    	dsmi	0x42
  55:	68 01       	dtc
  57:	68 09       	etq
  59:	68 08       	gec
  5b:	97          	ghi	7
  5c:	87          	glo	7
  5d:	00          	ldn	0
  5e:	17          	inc	7
  5f:	6f          	inp	7
  60:	60          	irx
  61:	c3 47 11    	lbdf	0x4711
  64:	cb 47 11    	lbnf	0x4711
  67:	c9 47 11    	lbnq	0x4711
  6a:	ca 47 11    	lbnz	0x4711
  6d:	c1 47 11    	lbq	0x4711
  70:	c0 47 11    	lbr	0x4711
  73:	c2 47 11    	lbz	0x4711
  76:	47          	lda	7
  77:	68 06       	ldc
  79:	f8 42       	ldi	0x42
  7b:	07          	ldn	7
  7c:	f0          	ldx
  7d:	72          	ldxa
  7e:	cf          	lsdf
  7f:	cc          	lsie
  80:	c8          	nlbr
  81:	c7          	lsnf
  82:	c5          	lsnq
  83:	c6          	lsnz
  84:	cd          	lsq
  85:	ce          	lsz
  86:	79          	mark
  87:	38          	skp
  88:	c8          	nlbr
  89:	c4          	nop
  8a:	f1          	or
  8b:	f9 42       	ori	0x42
  8d:	67          	out	7
  8e:	b7          	phi	7
  8f:	a7          	plo	7
  90:	7a          	req
  91:	70          	ret
  92:	68 c7 47 11 	rldi	7,0x4711
  96:	68 67       	rlxa	7
  98:	68 b7       	rnx	7
  9a:	7e          	shlc
  9b:	76          	shrc
  9c:	68 a7       	rsxd	7
  9e:	78          	sav
  9f:	68 87       	scal	7
  a1:	68 05       	scm1
  a3:	68 03       	scm2
  a5:	f5          	sd
  a6:	75          	sdb
  a7:	7d 42       	sdbi	0x42
  a9:	fd 42       	sdi	0x42
  ab:	d7          	sep	7
  ac:	7b          	seq
  ad:	e7          	sex	7
  ae:	fe          	shl
  af:	7e          	shlc
  b0:	f6          	shr
  b1:	76          	shrc
  b2:	38          	skp
  b3:	f7          	sm
  b4:	77          	smb
  b5:	7f 42       	smbi	0x42
  b7:	ff 42       	smi	0x42
  b9:	68 04       	spm1
  bb:	68 02       	spm2
  bd:	68 97       	sret	7
  bf:	68 07       	stm
  c1:	68 00       	stpc
  c3:	57          	str	7
  c4:	73          	stxd
  c5:	68 0b       	xid
  c7:	68 0a       	xie
  c9:	f3          	xor
  ca:	fb 42       	xri	0x42
