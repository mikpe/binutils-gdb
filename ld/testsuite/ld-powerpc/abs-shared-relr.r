#source: abs-reloc.s
#as: -a64
#ld: -melf64ppc -pie --hash-style=sysv -z pack-relative-relocs --defsym a=1 --defsym 'HIDDEN(b=2)' --defsym c=0x123456789abcdef0
#readelf: -rW

Relocation section '\.rela\.dyn' at offset .* contains 6 entries:
 +Offset +Info +Type +Symbol's Value +Symbol's Name \+ Addend
0+10408  0+500000026 R_PPC64_ADDR64         0+1 a \+ 0
0+10430  0+500000014 R_PPC64_GLOB_DAT       0+1 a \+ 0
0+10440  0+500000026 R_PPC64_ADDR64         0+1 a \+ 0
0+10418  0+400000026 R_PPC64_ADDR64         123456789abcdef0 c \+ 0
0+10428  0+400000014 R_PPC64_GLOB_DAT       123456789abcdef0 c \+ 0
0+10450  0+400000026 R_PPC64_ADDR64         123456789abcdef0 c \+ 0

Relocation section '\.relr\.dyn' at offset .* contains 1 entry which relocates 1 location:
Index: Entry            Address           Symbolic Address
0000: +[0-9a-f]+ [0-9a-f]+ +x

