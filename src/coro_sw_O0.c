#include "coro_comm.h"

/*
 * stack layout should NOT be optimized (use -O0)
 */
void coro_switch(struct coro_ctx *from, struct coro_ctx *to)
{
	asm volatile (
#ifdef __x86_64__
			/*
			 * callee-saved register: RBX,RBP,R12~R15
			 * use RDI to pass param for coro_main
			 */
			"pushq %%rbx\n\t"
			"pushq %%rbp\n\t"
			"pushq %%rdi\n\t"
			"pushq %%r12\n\t"
			"pushq %%r13\n\t"
			"pushq %%r14\n\t"
			"pushq %%r15\n\t"
			"movq %%rsp, %0\n\t"
			"movq %1, %%rsp\n\t"
			"popq %%r15\n\t"
			"popq %%r14\n\t"
			"popq %%r13\n\t"
			"popq %%r12\n\t"
			"popq %%rdi\n\t"
			"popq %%rbp\n\t"
			"popq %%rbx\n\t"
#else
			/*
			 * callee-saved register: EBX,EBP,ESI,EDI
			 */
			"pushl %%ebx\n\t"
			"pushl %%ebp\n\t"
			"pushl %%edi\n\t"
			"pushl %%esi\n\t"
			"movl %%esp, %0\n\t"
			"movl %1, %%esp\n\t"
			"popl %%esi\n\t"
			"popl %%edi\n\t"
			"popl %%ebp\n\t"
			"popl %%ebx\n\t"
#endif
			:
			: "m"(from->sp), "r"(to->sp));
}

