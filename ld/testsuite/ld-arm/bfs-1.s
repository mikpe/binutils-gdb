.arch armv8.1-m.main
.text
.syntax unified
.thumb
.global _start
.global target
.type target, %function
_start:
target:
	add	r0, r0, r1
