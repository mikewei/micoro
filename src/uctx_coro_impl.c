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
#include <stdlib.h>
#include <stdio.h>
#include "coro_comm.h"

static void uctx_coro_main(int arg1, int arg2)
{
	__coro_main((void*(*)(void*))(
				((uintptr_t)(unsigned int)arg1) + 
				((uintptr_t)(unsigned int)arg2 << (sizeof(int)*8))
				));
}

void uctx_coro_makectx(struct coro_ctx *ctx, size_t ctx_size, void* (*f)(void*))
{
	if (getcontext(&ctx->uctx) < 0) {
		perror("getcontext");
		abort();
	}
	ctx->uctx.uc_stack.ss_sp = (void *)ctx + sizeof(struct coro_ctx);
	ctx->uctx.uc_stack.ss_size = ctx_size - sizeof(struct coro_ctx);
	ctx->uctx.uc_link = NULL;
	/* according to makecontext(3), to be portable we must pass pointer 
	   by a pair of int args */
	makecontext(&ctx->uctx, (void(*)())uctx_coro_main, 2, 
			(int)f, (int)((uintptr_t)f >> (sizeof(int)*8)));
}

void uctx_coro_switch(struct coro_ctx *from, struct coro_ctx *to)
{
	if (swapcontext(&from->uctx, &to->uctx) < 0) {
		perror("swapcontext");
		abort();
	}
}
