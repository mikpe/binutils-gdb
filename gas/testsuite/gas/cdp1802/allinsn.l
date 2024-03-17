   1              		.text
   2 0000 74       		adc
   3 0001 7C42     		adci	0x42
   4 0003 F4       		add
   5 0004 FC42     		adi	0x42
   6 0006 F2       		and
   7 0007 FA42     		ani	0x42
   8 0009 3442     		b1	0x42
   9 000b 3542     		b2	0x42
  10 000d 3642     		b3	0x42
  11 000f 3742     		b4	0x42
  12 0011 683E42   		bci	0x42
  13 0014 3342     		bdf	0x42
  14 0016 3342     		bge	0x42
  15 0018 3B42     		bl	0x42
  16 001a 3B42     		bm	0x42
  17 001c 3C42     		bn1	0x42
  18 001e 3D42     		bn2	0x42
  19 0020 3E42     		bn3	0x42
  20 0022 3F42     		bn4	0x42
  21 0024 3B42     		bnf	0x42
  22 0026 3942     		bnq	0x42
  23 0028 3A42     		bnz	0x42
  24 002a 3342     		bpz	0x42
  25 002c 3142     		bq	0x42
  26 002e 3042     		br	0x42
  27 0030 683F42   		bxi	0x42
  28 0033 3242     		bz	0x42
  29 0035 680D     		cid
  30 0037 680C     		cie
  31 0039 687C42   		daci	0x42
  32 003c 6874     		dadc
  33 003e 68F4     		dadd
  34 0040 68FC42   		dadi	0x42
  35 0043 68274711 		dbnz	7, 0x4711
  36 0047 27       		dec	7
  37 0048 71       		dis
  38 0049 6876     		dsav
  39 004b 687F42   		dsbi	0x42
  40 004e 68F7     		dsm
  41 0050 6877     		dsmb
  42 0052 68FF42   		dsmi	0x42
  43 0055 6801     		dtc
  44 0057 6809     		etq
  45 0059 6808     		gec
  46 005b 97       		ghi	7
  47 005c 87       		glo	7
  48 005d 00       		idl
  49 005e 17       		inc	7
  50 005f 6F       		inp	7
  51 0060 60       		irx
  52 0061 C34711   		lbdf	0x4711
  53 0064 CB4711   		lbnf	0x4711
  54 0067 C94711   		lbnq	0x4711
  55 006a CA4711   		lbnz	0x4711
  56 006d C14711   		lbq	0x4711
  57 0070 C04711   		lbr	0x4711
  58 0073 C24711   		lbz	0x4711
  59 0076 47       		lda	7
  60 0077 6806     		ldc
  61 0079 F842     		ldi	0x42
  62 007b 07       		ldn	7
  63 007c F0       		ldx
  64 007d 72       		ldxa
  65 007e CF       		lsdf
  66 007f CC       		lsie
  67 0080 C8       		lskp
  68 0081 C7       		lsnf
  69 0082 C5       		lsnq
  70 0083 C6       		lsnz
  71 0084 CD       		lsq
  72 0085 CE       		lsz
  73 0086 79       		mark
  74 0087 38       		nbr
  75 0088 C8       		nlbr
  76 0089 C4       		nop
  77 008a F1       		or
  78 008b F942     		ori	0x42
  79 008d 67       		out	7
  80 008e B7       		phi	7
  81 008f A7       		plo	7
  82 0090 7A       		req
  83 0091 70       		ret
  84 0092 68C74711 		rldi	7, 0x4711
  85 0096 6867     		rlxa	7
  86 0098 68B7     		rnx	7
  87 009a 7E       		rshl
  88 009b 76       		rshr
  89 009c 68A7     		rsxd	7
  90 009e 78       		sav
  91 009f 6887     		scal	7
  92 00a1 6805     		scm1
  93 00a3 6803     		scm2
  94 00a5 F5       		sd
  95 00a6 75       		sdb
  96 00a7 7D42     		sdbi	0x42
  97 00a9 FD42     		sdi	0x42
  98 00ab D7       		sep	7
  99 00ac 7B       		seq
 100 00ad E7       		sex	7
 101 00ae FE       		shl
 102 00af 7E       		shlc
 103 00b0 F6       		shr
 104 00b1 76       		shrc
 105 00b2 38       		skp
 106 00b3 F7       		sm
 107 00b4 77       		smb
 108 00b5 7F42     		smbi	0x42
 109 00b7 FF42     		smi	0x42
 110 00b9 6804     		spm1
 111 00bb 6802     		spm2
 112 00bd 6897     		sret	7
 113 00bf 6807     		stm
 114 00c1 6800     		stpc
 115 00c3 57       		str	7
 116 00c4 73       		stxd
 117 00c5 680B     		xid
 118 00c7 680A     		xie
 119 00c9 F3       		xor
 120 00ca FB42     		xri	0x42
