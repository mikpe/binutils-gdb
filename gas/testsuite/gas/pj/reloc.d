#objdump: -dr
#name: pj relocation test
#as:

dump.o: +file format elf32-pjl

Disassembly of section .text:

00000000 <near-0x10e>:
   0:	11 01 0e ed 00 00       	ipush	10e <near>
			1: R_PJ_CODE_HILO	.text
   6:	11 00 00 ed 00 00       	ipush	0 <near-0x10e>
			7: R_PJ_CODE_HILO	undef
   c:	11 06 6e ed 00 00       	ipush	66e <far>
			d: R_PJ_CODE_HILO	.text
  12:	11 00 0d ed 00 00       	ipush	d <near-0x101>
			13: R_PJ_CODE_HILO	.data
  18:	11 00 00 ed 00 00       	ipush	0 <near-0x10e>
			19: R_PJ_CODE_HILO	farweak
  1e:	9a 00 08 c8 00 00 06 4d 	ifeq	66e <far>
  26:	99 00 08 c8 00 00 06 45 	ifne	66e <far>
  2e:	9c 00 08 c8 00 00 06 3d 	iflt	66e <far>
  36:	9b 00 08 c8 00 00 06 35 	ifge	66e <far>
  3e:	9e 00 08 c8 00 00 06 2d 	ifgt	66e <far>
  46:	9d 00 08 c8 00 00 06 25 	ifle	66e <far>
  4e:	a0 00 08 c8 00 00 06 1d 	if_icmpeq	66e <far>
  56:	9f 00 08 c8 00 00 06 15 	if_icmpne	66e <far>
  5e:	a2 00 08 c8 00 00 06 0d 	if_icmplt	66e <far>
  66:	a1 00 08 c8 00 00 06 05 	if_icmpge	66e <far>
  6e:	a4 00 08 c8 00 00 05 fd 	if_icmpgt	66e <far>
  76:	a3 00 08 c8 00 00 05 f5 	if_icmple	66e <far>
  7e:	a6 00 08 c8 00 00 05 ed 	if_acmpeq	66e <far>
  86:	a5 00 08 c8 00 00 05 e5 	if_acmpne	66e <far>
  8e:	a0 00 08 c8 00 00 00 0d 	if_icmpeq	9e <near-0x70>
			92: R_PJ_CODE_REL32	.data
  96:	9f 00 08 c8 00 00 00 0d 	if_icmpne	a6 <near-0x68>
			9a: R_PJ_CODE_REL32	.data
  9e:	a2 00 08 c8 00 00 00 0d 	if_icmplt	ae <near-0x60>
			a2: R_PJ_CODE_REL32	.data
  a6:	a1 00 08 c8 00 00 00 0d 	if_icmpge	b6 <near-0x58>
			aa: R_PJ_CODE_REL32	.data
  ae:	a4 00 08 c8 00 00 00 0d 	if_icmpgt	be <near-0x50>
			b2: R_PJ_CODE_REL32	.data
  b6:	a3 00 08 c8 00 00 00 0d 	if_icmple	c6 <near-0x48>
			ba: R_PJ_CODE_REL32	.data
  be:	a6 00 08 c8 00 00 00 0d 	if_acmpeq	ce <near-0x40>
			c2: R_PJ_CODE_REL32	.data
  c6:	a5 00 08 c8 00 00 00 0d 	if_acmpne	d6 <near-0x38>
			ca: R_PJ_CODE_REL32	.data
  ce:	a0 00 08 c8 00 00 00 00 	if_icmpeq	d1 <near-0x3d>
			d2: R_PJ_CODE_REL32	undef2
  d6:	9f 00 08 c8 00 00 00 00 	if_icmpne	d9 <near-0x35>
			da: R_PJ_CODE_REL32	undef2
  de:	a2 00 08 c8 00 00 00 00 	if_icmplt	e1 <near-0x2d>
			e2: R_PJ_CODE_REL32	undef2
  e6:	a1 00 08 c8 00 00 00 00 	if_icmpge	e9 <near-0x25>
			ea: R_PJ_CODE_REL32	undef2
  ee:	a4 00 08 c8 00 00 00 00 	if_icmpgt	f1 <near-0x1d>
			f2: R_PJ_CODE_REL32	undef2
  f6:	a3 00 08 c8 00 00 00 00 	if_icmple	f9 <near-0x15>
			fa: R_PJ_CODE_REL32	undef2
  fe:	a6 00 08 c8 00 00 00 00 	if_acmpeq	101 <near-0xd>
			102: R_PJ_CODE_REL32	undef2
 106:	a5 00 08 c8 00 00 00 00 	if_acmpne	109 <near-0x5>
			10a: R_PJ_CODE_REL32	undef2

0000010e <near>:
 10e:	c8 00 00 00 00          	goto_w	10e <near>
			10f: R_PJ_CODE_REL32	undef
 113:	c8 00 00 00 00          	goto_w	113 <near\+0x5>
			114: R_PJ_CODE_REL32	undef
 118:	c8 00 00 00 00          	goto_w	118 <near\+0xa>
			119: R_PJ_CODE_REL32	undef
 11d:	c8 00 00 00 00          	goto_w	11d <near\+0xf>
			11e: R_PJ_CODE_REL32	undef
 122:	c9 00 00 00 00          	jsr_w	122 <near\+0x14>
			123: R_PJ_CODE_REL32	undef
 127:	c9 00 00 00 00          	jsr_w	127 <near\+0x19>
			128: R_PJ_CODE_REL32	undef
 12c:	c9 00 00 00 00          	jsr_w	12c <near\+0x1e>
			12d: R_PJ_CODE_REL32	undef
 131:	c9 00 00 00 00          	jsr_w	131 <near\+0x23>
			132: R_PJ_CODE_REL32	undef
 136:	c8 00 00 00 00          	goto_w	136 <near\+0x28>
			137: R_PJ_CODE_REL32	undef2
 13b:	c8 00 00 00 00          	goto_w	13b <near\+0x2d>
			13c: R_PJ_CODE_REL32	undef2
 140:	c8 00 00 00 00          	goto_w	140 <near\+0x32>
			141: R_PJ_CODE_REL32	undef2
 145:	c8 00 00 00 00          	goto_w	145 <near\+0x37>
			146: R_PJ_CODE_REL32	undef2
 14a:	c9 00 00 00 00          	jsr_w	14a <near\+0x3c>
			14b: R_PJ_CODE_REL32	undef2
 14f:	c9 00 00 00 00          	jsr_w	14f <near\+0x41>
			150: R_PJ_CODE_REL32	undef2
 154:	c9 00 00 00 00          	jsr_w	154 <near\+0x46>
			155: R_PJ_CODE_REL32	undef2
 159:	c9 00 00 00 00          	jsr_w	159 <near\+0x4b>
			15a: R_PJ_CODE_REL32	undef2
 15e:	a7 ff b0                	goto	10e <near>
 161:	a7 ff ad                	goto	10e <near>
 164:	c8 ff ff ff aa          	goto_w	10e <near>
 169:	c8 ff ff ff a5          	goto_w	10e <near>
 16e:	a8 ff a0                	jsr	10e <near>
 171:	a8 ff 9d                	jsr	10e <near>
 174:	c9 ff ff ff 9a          	jsr_w	10e <near>
 179:	c9 ff ff ff 95          	jsr_w	10e <near>
 17e:	c8 00 00 04 f0          	goto_w	66e <far>
 183:	c8 00 00 04 eb          	goto_w	66e <far>
 188:	c8 00 00 04 e6          	goto_w	66e <far>
 18d:	c8 00 00 04 e1          	goto_w	66e <far>
 192:	c9 00 00 04 dc          	jsr_w	66e <far>
 197:	c9 00 00 04 d7          	jsr_w	66e <far>
 19c:	c9 00 00 04 d2          	jsr_w	66e <far>
 1a1:	c9 00 00 04 cd          	jsr_w	66e <far>
 1a6:	9a 00 08 c8 00 00 00 00 	ifeq	1a9 <near\+0x9b>
			1aa: R_PJ_CODE_REL32	undef
 1ae:	99 00 08 c8 00 00 00 00 	ifne	1b1 <near\+0xa3>
			1b2: R_PJ_CODE_REL32	undef
 1b6:	9c 00 08 c8 00 00 00 00 	iflt	1b9 <near\+0xab>
			1ba: R_PJ_CODE_REL32	undef
 1be:	9b 00 08 c8 00 00 00 00 	ifge	1c1 <near\+0xb3>
			1c2: R_PJ_CODE_REL32	undef
 1c6:	9e 00 08 c8 00 00 00 00 	ifgt	1c9 <near\+0xbb>
			1ca: R_PJ_CODE_REL32	undef
 1ce:	9d 00 08 c8 00 00 00 00 	ifle	1d1 <near\+0xc3>
			1d2: R_PJ_CODE_REL32	undef
 1d6:	a0 00 08 c8 00 00 00 00 	if_icmpeq	1d9 <near\+0xcb>
			1da: R_PJ_CODE_REL32	undef
 1de:	9f 00 08 c8 00 00 00 00 	if_icmpne	1e1 <near\+0xd3>
			1e2: R_PJ_CODE_REL32	undef
 1e6:	a2 00 08 c8 00 00 00 00 	if_icmplt	1e9 <near\+0xdb>
			1ea: R_PJ_CODE_REL32	undef
 1ee:	a1 00 08 c8 00 00 00 00 	if_icmpge	1f1 <near\+0xe3>
			1f2: R_PJ_CODE_REL32	undef
 1f6:	a4 00 08 c8 00 00 00 00 	if_icmpgt	1f9 <near\+0xeb>
			1fa: R_PJ_CODE_REL32	undef
 1fe:	a3 00 08 c8 00 00 00 00 	if_icmple	201 <near\+0xf3>
			202: R_PJ_CODE_REL32	undef
 206:	a6 00 08 c8 00 00 00 00 	if_acmpeq	209 <near\+0xfb>
			20a: R_PJ_CODE_REL32	undef
 20e:	a5 00 08 c8 00 00 00 00 	if_acmpne	211 <near\+0x103>
			212: R_PJ_CODE_REL32	undef
 216:	9a 00 08 c8 ff ff fe f5 	ifeq	10e <near>
 21e:	99 00 08 c8 ff ff fe ed 	ifne	10e <near>
 226:	9c 00 08 c8 ff ff fe e5 	iflt	10e <near>
 22e:	9b 00 08 c8 ff ff fe dd 	ifge	10e <near>
 236:	9e 00 08 c8 ff ff fe d5 	ifgt	10e <near>
 23e:	9d 00 08 c8 ff ff fe cd 	ifle	10e <near>
 246:	a0 00 08 c8 ff ff fe c5 	if_icmpeq	10e <near>
 24e:	9f 00 08 c8 ff ff fe bd 	if_icmpne	10e <near>
 256:	a2 00 08 c8 ff ff fe b5 	if_icmplt	10e <near>
 25e:	a1 00 08 c8 ff ff fe ad 	if_icmpge	10e <near>
 266:	a4 00 08 c8 ff ff fe a5 	if_icmpgt	10e <near>
 26e:	a3 00 08 c8 ff ff fe 9d 	if_icmple	10e <near>
 276:	a6 00 08 c8 ff ff fe 95 	if_acmpeq	10e <near>
 27e:	a5 00 08 c8 ff ff fe 8d 	if_acmpne	10e <near>
	...

0000066e <far>:
 66e:	00                      	nop
 66f:	00                      	nop
 670:	9a 00 08 c8 00 00 00 9c 	ifeq	70f <self>
 678:	9a 00 08 c8 00 00 00 94 	ifeq	70f <self>
 680:	9a 00 08 c8 00 00 00 8c 	ifeq	70f <self>
 688:	9a 00 08 c8 00 00 00 84 	ifeq	70f <self>
 690:	99 00 7f                	ifeq	70f <self>
 693:	99 00 7c                	ifeq	70f <self>
 696:	99 00 79                	ifeq	70f <self>
 699:	99 00 76                	ifeq	70f <self>
 69c:	99 00 73                	ifeq	70f <self>
 69f:	99 00 70                	ifeq	70f <self>
 6a2:	99 00 6d                	ifeq	70f <self>
 6a5:	99 00 6a                	ifeq	70f <self>
 6a8:	99 00 67                	ifeq	70f <self>
 6ab:	99 00 64                	ifeq	70f <self>
 6ae:	99 00 61                	ifeq	70f <self>
 6b1:	99 00 5e                	ifeq	70f <self>
 6b4:	99 00 5b                	ifeq	70f <self>
 6b7:	99 00 58                	ifeq	70f <self>
 6ba:	99 00 55                	ifeq	70f <self>
 6bd:	99 00 52                	ifeq	70f <self>
 6c0:	99 00 4f                	ifeq	70f <self>
 6c3:	99 00 4c                	ifeq	70f <self>
 6c6:	99 00 49                	ifeq	70f <self>
 6c9:	99 00 46                	ifeq	70f <self>
 6cc:	99 00 43                	ifeq	70f <self>
 6cf:	99 00 40                	ifeq	70f <self>
 6d2:	99 00 3d                	ifeq	70f <self>
 6d5:	99 00 3a                	ifeq	70f <self>
 6d8:	99 00 37                	ifeq	70f <self>
 6db:	99 00 34                	ifeq	70f <self>
 6de:	99 00 31                	ifeq	70f <self>
 6e1:	99 00 2e                	ifeq	70f <self>
 6e4:	99 00 2b                	ifeq	70f <self>
 6e7:	99 00 28                	ifeq	70f <self>
 6ea:	99 00 25                	ifeq	70f <self>
 6ed:	99 00 22                	ifeq	70f <self>
 6f0:	99 00 1f                	ifeq	70f <self>
 6f3:	99 00 1c                	ifeq	70f <self>
 6f6:	99 00 19                	ifeq	70f <self>
 6f9:	99 00 16                	ifeq	70f <self>
 6fc:	99 00 13                	ifeq	70f <self>
 6ff:	99 00 10                	ifeq	70f <self>
 702:	99 00 0d                	ifeq	70f <self>
 705:	99 00 0a                	ifeq	70f <self>
 708:	99 00 07                	ifeq	70f <self>
 70b:	9b 00 04                	iflt	70f <self>
	...

0000070f <self>:
 70f:	9a 00 00                	ifne	70f <self>
 712:	99 ff fd                	ifeq	70f <self>
 715:	00                      	nop
 716:	00                      	nop
 717:	99 ff f8                	ifeq	70f <self>
 71a:	99 ff f5                	ifeq	70f <self>
 71d:	99 ff f2                	ifeq	70f <self>
 720:	99 ff ef                	ifeq	70f <self>
 723:	99 ff ec                	ifeq	70f <self>
 726:	99 ff e9                	ifeq	70f <self>
 729:	99 ff e6                	ifeq	70f <self>
 72c:	99 ff e3                	ifeq	70f <self>
 72f:	99 ff e0                	ifeq	70f <self>
 732:	99 ff dd                	ifeq	70f <self>
 735:	99 ff da                	ifeq	70f <self>
 738:	99 ff d7                	ifeq	70f <self>
 73b:	99 ff d4                	ifeq	70f <self>
 73e:	99 ff d1                	ifeq	70f <self>
 741:	99 ff ce                	ifeq	70f <self>
 744:	99 ff cb                	ifeq	70f <self>
 747:	99 ff c8                	ifeq	70f <self>
 74a:	99 ff c5                	ifeq	70f <self>
 74d:	99 ff c2                	ifeq	70f <self>
 750:	99 ff bf                	ifeq	70f <self>
 753:	99 ff bc                	ifeq	70f <self>
 756:	99 ff b9                	ifeq	70f <self>
 759:	99 ff b6                	ifeq	70f <self>
 75c:	99 ff b3                	ifeq	70f <self>
 75f:	99 ff b0                	ifeq	70f <self>
 762:	99 ff ad                	ifeq	70f <self>
 765:	99 ff aa                	ifeq	70f <self>
 768:	99 ff a7                	ifeq	70f <self>
 76b:	99 ff a4                	ifeq	70f <self>
 76e:	99 ff a1                	ifeq	70f <self>
 771:	99 ff 9e                	ifeq	70f <self>
 774:	99 ff 9b                	ifeq	70f <self>
 777:	99 ff 98                	ifeq	70f <self>
 77a:	99 ff 95                	ifeq	70f <self>
 77d:	99 ff 92                	ifeq	70f <self>
 780:	99 ff 8f                	ifeq	70f <self>
 783:	99 ff 8c                	ifeq	70f <self>
 786:	99 ff 89                	ifeq	70f <self>
 789:	99 ff 86                	ifeq	70f <self>
 78c:	99 ff 83                	ifeq	70f <self>
 78f:	99 ff 80                	ifeq	70f <self>
 792:	9a 00 08 c8 ff ff ff 7a 	ifeq	70f <self>
