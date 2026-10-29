#include "mlibc-asm/helpers.h"

    .abicalls
    .set noreorder

PROC_START(__start)
    bal 1f
    nop
1:
    # Load gp.
    .cpsetup $ra, $zero, 1b

    move $ra, $zero
    move $fp, $zero

    lui    $t9, %hi(%gp_rel(relocateSelf))
    daddiu $t9, $t9, %lo(%gp_rel(relocateSelf))
    daddu  $t9, $t9, $gp

    jalr $t9
    nop
	

	move $a0, $sp

    dla $t9, interpreterMain
    jalr $t9
    nop

	jr $v0
    nop
PROC_END(__start)

GNU_STACK_NOTE()
