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
#include <stdio.h>
#include <string.h>
#include "coro_comm.h"
#include "coro_mm.h"

static int g_init_flag = 0;
static struct coro_mm_ops *g_mm_ops = NULL; 
static size_t g_ctx_size = 0;
static struct coro_stat g_stat;

int coro_init(size_t ctx_size, size_t pool_size)
{
	if (init_once(&g_init_flag) < 0) {
		fprintf(stderr, "coro init more than once!\n");
		return -1;
	}

	g_mm_ops = coro_mm_default_ops;
	g_ctx_size = ctx_size;

	if (g_mm_ops->init(&g_ctx_size, pool_size) < 0) {
		fprintf(stderr, "mm init failed!\n");
		return -2;
	}

	return 0;
}

static inline int check_ctx(struct coro_ctx *ctx, int prev, int next)
{
	if (!ctx) return 0;
	if (ctx->tag != CORO_CTX_TAG) return 0;
	if (prev >= 0 && !!ctx->prev != !!prev) return 0;
	if (next >= 0 && !!ctx->next != !!next) return 0;
	return 1;
}

static inline void check_ctx_or_die(struct coro_ctx *ctx, 
		int prev, int next, const char *str)
{
	if (!check_ctx(ctx, prev, next)) {
		// bug found
		fprintf(stderr, "%s\n", str);
		abort();
	}
}

void* coro_resume(coro_t *coro, void *arg)
{
	struct coro_ctx main_ctx;
	struct coro_ctx *cur, *to = coro->ctx;

	/* using a old/wild coro ? */
	check_ctx_or_die(to, -1, -1, "resuming bad coro (1)");

	if ((cur = g_mm_ops->locate(&arg))) {
		check_ctx_or_die(cur, 1, 0, "resuming from bad coro");
	}	
	else {
		memset(&main_ctx, 0, sizeof(struct coro_ctx));
		main_ctx.tag = CORO_CTX_TAG;
		cur = &main_ctx;
	}

	LIGHT_LOCK(&to->lock);

	/* using a dying coro ? */
	check_ctx_or_die(to, 0, 0, "resuming bad coro (2)");

	cur->next = to;
	to->prev = cur;
	to->ret = arg;

	g_stat.resume_count++;
	coro_switch(cur, to);

	if (to->flag & CORO_FLAG_END) {
		memset(to, 0, sizeof(struct coro_ctx));
		/* give a chance to avoid dead-locking */
		LIGHT_UNLOCK(&to->lock);
		/* must before release because coro maybe in the stack */
		coro->ctx = NULL;
		g_mm_ops->release(to);
	} else {
		LIGHT_UNLOCK(&to->lock);
	}

	return cur->ret;
}

static void* do_coro_yield(void *arg, unsigned int flag)
{
	struct coro_ctx *to, *cur = NULL;

	cur = g_mm_ops->locate(&arg);
	check_ctx_or_die(cur, 1, 0, "yielding from bad coro");
	check_ctx_or_die(cur->prev, -1, 1, "yielding to bad coro");
	to = cur->prev;
	cur->prev = NULL;
	to->next = NULL;
	to->ret = arg;
	cur->flag = flag;
	g_stat.yield_count++;
	coro_switch(cur, to);
	return cur->ret;
}

void* coro_yield(void *arg)
{
	return do_coro_yield(arg, 0);
}

__attribute__ ((noinline, regparm(0)))
static void coro_main(void* (*f)(void*))
{
	struct coro_ctx *cur;
	void *arg, *ret;

	cur = g_mm_ops->locate(&arg);
	check_ctx_or_die(cur, 1, 0, "coro_main found bad coro");
	arg = cur->ret;
	ret = f(arg);
	do_coro_yield(ret, CORO_FLAG_END);
	fprintf(stderr, "bug! resume dead coro_main\n");
	exit(1);
}

int coro_create(coro_t *coro, void* (*f)(void*))
{
	struct coro_ctx *ctx;
	unsigned long *stack;

	if (!g_init_flag) {
		fprintf(stderr, "bug! coro_create without coro_init! abort\n");
		abort();
	}
	coro->ctx = NULL;

	if (!(ctx = g_mm_ops->alloc()))
		return -1;

	stack = (void *)ctx + g_ctx_size - sizeof(unsigned long);
	*stack = 0UL;
	stack = (void *)((unsigned long)stack & ~(sizeof(unsigned long) - 1));

	stack[0] = (unsigned long)f;
	stack[-1] = 0UL; // IP
	stack[-2] = (unsigned long)coro_main;
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
	memset(ctx, 0, sizeof(struct coro_ctx));
	ctx->sp = &stack[_stack_top];
	ctx->tag = CORO_CTX_TAG;
	light_lock_init(&ctx->lock);

	coro->ctx = ctx;
	return 0;
}

int coro_self(coro_t *coro)
{
	struct coro_ctx *ctx;

	coro->ctx = NULL;
	if (!g_init_flag)
		return -1;
	ctx = g_mm_ops->locate(&coro);
	if (ctx == NULL)
		return -2;
	coro->ctx = ctx;
	return 0;
}

void coro_get_stat(struct coro_stat *stat)
{
	struct coro_mm_stat mm_stat;
	g_mm_ops->get_stat(&mm_stat);
	stat->create_count = mm_stat.alloc_count;
	stat->destroy_count = mm_stat.release_count;
	stat->alive_coro_num = mm_stat.use_block_num;
	stat->pool_left_num = mm_stat.free_block_num;
	stat->resume_count = g_stat.resume_count;
	stat->yield_count = g_stat.yield_count;
}

