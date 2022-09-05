! pj relocation and relax test.
! test stretching of conditionals

	ipush near
	ipush undef
	ipush far
	ipush far2
	ipush farweak
	ifeq far
	ifne far
	iflt far
	ifge far
	ifgt far
	ifle far
	if_icmpeq far
	if_icmpne far
	if_icmplt far
	if_icmpge far
	if_icmpgt far
	if_icmple far
	if_acmpeq far
	if_acmpne far
	if_icmpeq far2
	if_icmpne far2
	if_icmplt far2
	if_icmpge far2
	if_icmpgt far2
	if_icmple far2
	if_acmpeq far2
	if_acmpne far2
	if_icmpeq undef2
	if_icmpne undef2
	if_icmplt undef2
	if_icmpge undef2
	if_icmpgt undef2
	if_icmple undef2
	if_acmpeq undef2
	if_acmpne undef2
near:
	goto undef
	goto undef	
	goto_w undef
	goto_w undef	
	jsr undef
	jsr undef	
	jsr_w undef
	jsr_w undef	

	goto undef2
	goto undef2	
	goto_w undef2
	goto_w undef2	
	jsr undef2
	jsr undef2	
	jsr_w undef2
	jsr_w undef2	
	
	goto near
	goto near	
	goto_w near
	goto_w near	
	jsr near
	jsr near	
	jsr_w near
	jsr_w near	

	goto far
	goto far	
	goto_w far
	goto_w far	
	jsr far
	jsr far	
	jsr_w far
	jsr_w far	
	
	ifeq undef
	ifne undef
	iflt undef
	ifge undef
	ifgt undef
	ifle undef
	if_icmpeq undef
	if_icmpne undef
	if_icmplt undef
	if_icmpge undef
	if_icmpgt undef
	if_icmple undef
	if_acmpeq undef
	if_acmpne undef

	ifeq near
	ifne near
	iflt near
	ifge near
	ifgt near
	ifle near
	if_icmpeq near
	if_icmpne near
	if_icmplt near
	if_icmpge near
	if_icmpgt near
	if_icmple near
	if_acmpeq near
	if_acmpne near

	.space 1000
far:
	nop
	nop
	.data
	.space 13
far2:	
	nop

	.weak farweak
farweak:
	nop
	

	
	.text
! make sure that the change from short to long pc rel branching
! is done at just the right moment.
	ifeq self
	ifeq self
	ifeq self
	ifeq self
	ifeq self
	ifeq self
	ifeq self
	ifeq self
	ifeq self
	ifeq self							
	ifeq self
	ifeq self
	ifeq self
	ifeq self
	ifeq self
	ifeq self
	ifeq self
	ifeq self							
	ifeq self
	ifeq self
	ifeq self
	ifeq self
	ifeq self
	ifeq self							
	ifeq self
	ifeq self
	ifeq self
	ifeq self
	ifeq self
	ifeq self
	ifeq self
	ifeq self							
	ifeq self
	ifeq self
	ifeq self
	ifeq self
	ifeq self
	ifeq self
	ifeq self
	ifeq self
	ifeq self
	ifeq self
	ifeq self							
	ifeq self
	ifeq self
	iflt self
	nop
self:	ifne self
	ifeq self
	nop
	nop
	ifeq self
	ifeq self
	ifeq self							
	ifeq self
	ifeq self
	ifeq self
	ifeq self
	ifeq self
	ifeq self
	ifeq self
	ifeq self							
	ifeq self
	ifeq self
	ifeq self
	ifeq self
	ifeq self
	ifeq self
	ifeq self
	ifeq self							
	ifeq self
	ifeq self
	ifeq self
	ifeq self
	ifeq self
	ifeq self							
	ifeq self
	ifeq self
	ifeq self
	ifeq self
	ifeq self
	ifeq self
	ifeq self
	ifeq self							
	ifeq self
	ifeq self
	ifeq self
	ifeq self
	ifeq self
	ifeq self
	ifeq self
	ifeq self							
	ifeq self
