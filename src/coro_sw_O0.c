/* Copyright (c) 2012, Bin Wei <bin@vip.qq.com>
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * The name of of its contributors may not be used to endorse or 
 * promote products derived from this software without specific prior 
 * written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
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

