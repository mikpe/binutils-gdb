	.text
	.p2align 3
foo:	move	8,fie
fie:	.pdp10_word fum
fum:	.pdp10_bptr foo+2
	.pdp10_hptr foo+4
fie2:	.pdp10_hword 0,fum
