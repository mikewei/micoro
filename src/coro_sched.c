#include <stdio.h>
#include <string.h>
#include "coro_comm.h"
#include "coro_mm.h"

static struct coro_mm_ops *g_mm_ops;
static size_t g_ctx_size;


int coro_init(size_t ctx_size, size_t pool_size)
{

	g_mm_ops = coro_mm_default_ops;
	g_ctx_size = ctx_size;

	if (g_mm_ops->init(&g_ctx_size, pool_size) < 0) {

		fprintf(stderr, "mm init failed!\n");
		return -1;
	}

	return 0;
}

static inline void check_ctx(struct coro_ctx *ctx, int prev, int next)
{
	do {
		if (!ctx) break;
		if (ctx->tag != CORO_CTX_TAG) break;
		if (prev >= 0 && !!ctx->prev != !!prev) break;
		if (next >= 0 && !!ctx->next != !!next) break;
		return;
	} while(0);
	// bug found
	fprintf(stderr, "bad coro ctx!\n");
	exit(1);
}

void* coro_resume(coro_t *coro, void *arg)
{
	struct coro_ctx main_ctx;
	struct coro_ctx *cur, *to = coro->ctx;

	check_ctx(to, 0, 0);
	if ((cur = g_mm_ops->locate(&arg))) {
		check_ctx(cur, 1, 0);
	}	
	else {
		memset(&main_ctx, 0, sizeof(struct coro_ctx));
		main_ctx.tag = CORO_CTX_TAG;
		cur = &main_ctx;
	}
	cur->next = to;
	to->prev = cur;
	to->ret = arg;

	coro_switch(cur, to);

	if (to->flag & CORO_FLAG_END) {
		memset(to, 0, sizeof(struct coro_ctx));
		g_mm_ops->release(to);
		coro->ctx = NULL;
	}
	return cur->ret;
}

static void* do_coro_yield(void *arg, unsigned int flag)
{
	struct coro_ctx *to, *cur = NULL;

	cur = g_mm_ops->locate(&arg);
	check_ctx(cur, 1, 0);
	check_ctx(cur->prev, -1, 1);
	to = cur->prev;
	cur->prev = NULL;
	to->next = NULL;
	to->ret = arg;
	cur->flag = flag;
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
	check_ctx(cur, 1, 0);
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

	if (!coro || !f)
		return -1;
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

	coro->ctx = ctx;
	return 0;
}

