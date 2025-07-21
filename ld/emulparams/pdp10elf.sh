SCRIPT_NAME=elf
ELFSIZE=64
TEMPLATE_NAME=elf
OUTPUT_FORMAT="elf64-pdp10"
ARCH=pdp10
MACHINE=

# 0255000000000 = JFCL with zeros in A, I, X, and Y
NOP=0x568000000

# (00002001000 = section 2 page 1 offset 0) * 8
TEXT_START_ADDR=0x401000

# 512 words * 4 nonets per word * 2 octets per nonet
MAXPAGESIZE=4096
