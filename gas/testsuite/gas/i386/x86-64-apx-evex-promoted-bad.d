#as: --divide
#objdump: -dw
#name: x86-64 EVEX-promoted bad

.*: +file format .*


Disassembly of section .text:

0+ <_start>:
[ 	]*[a-f0-9]+:[ 	]+62 fc 7e 08 60[ 	]+\(bad\)
[ 	]*[a-f0-9]+:[ 	]+c7[ 	]+\(bad\)
[ 	]*[a-f0-9]+:[ 	]+62 fc 7f 08 60[ 	]+\(bad\)
[ 	]*[a-f0-9]+:[ 	]+c7[ 	]+\(bad\)
[ 	]*[a-f0-9]+:[ 	]+62 e2 f9 41 91 84[ 	]+vpgatherqq \(bad\),%zmm16\{%k1\}
[ 	]*[a-f0-9]+:[ 	]+cd ff[ 	]+int    \$0xff
[ 	]*[a-f0-9]+:[ 	]+62 fd 7d 08 60[ 	]+\(bad\)
[ 	]*[a-f0-9]+:[ 	]+c7[ 	]+\(bad\)
[ 	]*[a-f0-9]+:[ 	]+62 fc 7d[ 	]+\(bad\).*
[ 	]*[a-f0-9]+:[ 	]+09 60 c7[ 	]+or     %esp,-0x39\(%rax\)
[ 	]*[a-f0-9]+:[ 	]+62 fc 7d[ 	]+\(bad\).*
[ 	]*[a-f0-9]+:[ 	]+28 60 c7[ 	]+.*
[ 	]*[a-f0-9]+:[ 	]+62 fc 7d[ 	]+\(bad\).*
[ 	]*[a-f0-9]+:[ 	]+8b 60 c7[ 	]+.*
[ 	]*[a-f0-9]+:[ 	]+62 f2 fc 09 f5[ 	]+\(bad\).*
[ 	]*[a-f0-9]+:[ 	]+0c 18[ 	]+or.*
[ 	]*[a-f0-9]+:[ 	]+62 f2 fc 28 f5[ 	]+\(bad\)
[ 	]*[a-f0-9]+:[ 	]+0c 18[ 	]+or.*
[ 	]*[a-f0-9]+:[ 	]+62 f2 fc 8b f5[ 	]+\(bad\).*
[ 	]*[a-f0-9]+:[ 	]+0c 18[ 	]+or.*
[ 	]*[a-f0-9]+:[ 	]+62 f2 fc 18 f5[ 	]+\(bad\)
[ 	]*[a-f0-9]+:[ 	]+0c 18[ 	]+or.*
[ 	]*[a-f0-9]+:[ 	]+62 f4 e4[ 	]+\(bad\)
[ 	]*[a-f0-9]+:[ 	]+08 ff[ 	]+.*
[ 	]*[a-f0-9]+:[ 	]+04 08[ 	]+.*
[ 	]*[a-f0-9]+:[ 	]+62 f4 3c[ 	]+\(bad\)
[ 	]*[a-f0-9]+:[ 	]+08 8f c0 ff ff ff[ 	]+or.*
[ 	]*[a-f0-9]+:[ 	]+62 74 7c 18 8f c0[ 	]+pop2   %rax,\(bad\)
[ 	]*[a-f0-9]+:[ 	]+62 d4 24 18 8f[ 	]+\(bad\)
[ 	]*[a-f0-9]+:[ 	]+c3[ 	]+.*
[ 	]*[a-f0-9]+:[ 	]+62 fc 7d 0c 60 c7[ 	]+movbe  \{bad-nf\},%r23w,%ax
[ 	]*[a-f0-9]+:[ 	]+62 fc 79 08 60[ 	]+\(bad\)
[ 	]*[a-f0-9]+:[ 	]+c2[ 	]+.*
#pass
