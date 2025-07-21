#objdump: --wide -d

.*\.o:     file format elf64-pdp10


Disassembly of section .text:

0000000000000000 <.text>:
   0:	00 80 01 17 00 53 01 2e 	move 10,@123456\(7\)
   8:	00 80 01 07 00 53 01 2e 	move 10,123456\(7\)
  10:	00 80 01 10 00 53 01 2e 	move 10,@123456
  18:	00 80 01 00 00 53 01 2e 	move 10,123456
