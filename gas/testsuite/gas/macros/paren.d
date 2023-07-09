#as: -f
#objdump: -s -j .data
#name parenthesized macro arguments
# pdp10 allocates two octets per nonet so the offsets are off and the contents more spaced apart
#xfail: pdp10-*-*

.*: .*

Contents of section .data:
 0000 01000202 020402.. ........ ........  ................
#pass
