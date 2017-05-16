/* See COPYRIGHT for copyright information. */

#include <inc/mmu.h>
#include <inc/memlayout.h>
#include <inc/trap.h>
#include <kern/picirq.h>


###################################################################
# exceptions/interrupts
###################################################################

/* TRAPHANDLER defines a globally-visible function for handling a trap.
 * It pushes a trap number onto the stack, then jumps to _alltraps.
 * Use TRAPHANDLER for traps where the CPU automatically pushes an error code.
 *
 * You shouldn't call a TRAPHANDLER function from C, but you may
 * need to _declare_ one in C (for instance, to get a function pointer
 * during IDT setup).  You can declare the function with
 *   void NAME();
 * where NAME is the argument passed to TRAPHANDLER.
 */
#define TRAPHANDLER(name, num)						\
	.globl name;		/* define global symbol for 'name' */	\
	.type name, @function;	/* symbol type is function */		\
	.align 2;		/* align function definition */		\
	name:			/* function starts here */		\
	pushl $(num);							\
	jmp _alltraps

/* Use TRAPHANDLER_NOEC for traps where the CPU doesn't push an error code.
 * It pushes a 0 in place of the error code, so the trap frame has the same
 * format in either case.
 */
#define TRAPHANDLER_NOEC(name, num)					\
	.globl name;							\
	.type name, @function;						\
	.align 2;							\
	name:								\
	pushl $0;							\
	pushl $(num);							\
	jmp _alltraps

.text

/*
 * Lab 3: Your code here for generating entry points for the different traps.
 */
TRAPHANDLER_NOEC(handler_0, 0);
TRAPHANDLER_NOEC(handler_1, 1);
TRAPHANDLER_NOEC(handler_2, 2);
TRAPHANDLER_NOEC(handler_3, 3);
TRAPHANDLER_NOEC(handler_4, 4);
TRAPHANDLER_NOEC(handler_5, 5);
TRAPHANDLER_NOEC(handler_6, 6);
TRAPHANDLER_NOEC(handler_7, 7);
TRAPHANDLER(handler_8, 8);
TRAPHANDLER(handler_10, 10);
TRAPHANDLER(handler_11, 11);
TRAPHANDLER(handler_12, 12);
TRAPHANDLER(handler_13, 13);
TRAPHANDLER(handler_14, 14);
TRAPHANDLER_NOEC(handler_16, 16);
TRAPHANDLER(handler_17, 17);
TRAPHANDLER_NOEC(handler_18, 18);
TRAPHANDLER_NOEC(handler_19, 19);


TRAPHANDLER_NOEC(irq0, IRQ_OFFSET+0 );
TRAPHANDLER_NOEC(irq1, IRQ_OFFSET+1 );
TRAPHANDLER_NOEC(irq2, IRQ_OFFSET+2 );
TRAPHANDLER_NOEC(irq3, IRQ_OFFSET+3 );
TRAPHANDLER_NOEC(irq4, IRQ_OFFSET+4 );
TRAPHANDLER_NOEC(irq5, IRQ_OFFSET+5 );
TRAPHANDLER_NOEC(irq6, IRQ_OFFSET+6 );
TRAPHANDLER_NOEC(irq7, IRQ_OFFSET+7 );
TRAPHANDLER_NOEC(irq8, IRQ_OFFSET+8 );
TRAPHANDLER_NOEC(irq9, IRQ_OFFSET+9 );
TRAPHANDLER_NOEC(irq10, IRQ_OFFSET+10 );
TRAPHANDLER_NOEC(irq11, IRQ_OFFSET+11 );
TRAPHANDLER_NOEC(irq12, IRQ_OFFSET+12 );
TRAPHANDLER_NOEC(irq13, IRQ_OFFSET+13 );
TRAPHANDLER_NOEC(irq14, IRQ_OFFSET+14 );
TRAPHANDLER_NOEC(irq15, IRQ_OFFSET+15 );

.globl sysenter_handler;
.type sysenter_handler, @function;
.align 2;
sysenter_handler:
/*
 * Lab 3: Your code here for system call handling
 */
 
 pushl $GD_UD|3
 pushl %ebp
 pushfl
 pushl $GD_UT|3
 pushl %esi
 pushl $0
 pushl $0
 pushl %ds
 pushl %es
 pushal

 movw $GD_KD, %ax
 movw %ax, %ds
 movw %ax, %es
 
 pushl %esp

 call callsyscall
 
 popl %esp
 popal
 popl %es
 popl %ds
 movl %ebp, %ecx
 movl %esi, %edx
 
 sysexit 

/*
 * Lab 3: Your code here for _alltraps
 */
 _alltraps:
 	pushw $0x0
 	pushw %ds
 	pushw $0x0
 	pushw %es
 	pushal

 	movl $GD_KD, %eax
 	movw %ax, %ds
 	movw %ax, %es

 	pushl %esp
 	call trap