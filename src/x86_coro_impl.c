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

void x86_coro_makectx(struct coro_ctx *ctx, size_t ctx_size, void* (*f)(void*))
{
	unsigned long *stack;

	stack = (void *)ctx + ctx_size - sizeof(unsigned long);
	/* now stack point to the last word of the context space */
	*stack = 0UL;

	/* make stack-frame of coro_main 16-byte aligned
	 * for amd64-ABI and some systems (such as Mac OS) require this
	 */
	stack = (void *)((unsigned long)stack & ~0x0fUL);

	stack[0] = (unsigned long)f;
	/* here is 16-byte alignment boundary */
	stack[-1] = 0UL; // IP
	stack[-2] = (unsigned long)__coro_main;
	stack[-3] = 0UL; // BP
	stack[-4] = 0UL; // BX
	stack[-5] = (unsigned long)&stack[-3]; // BP
	stack[-6] = stack[0]; // DI
#ifdef __x86_64__
	stack[-7] = 0UL; // R12
	stack[-8] = 0UL; // R13
	stack[-9] = 0UL; // R14
	stack[-10] = 0UL; // R15
#	define _stack_top (-10)
#else
	stack[-7] = 0UL; // SI
#	define _stack_top (-7)
#endif
	ctx->sp = &stack[_stack_top];
}
