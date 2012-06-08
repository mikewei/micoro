#ifndef __CORO_COMM_H__
#define __CORO_COMM_H__

#include <stdlib.h>
#include "coroutine.h"

struct coro_mm_ops
{
	int (*init)(size_t *alloc_size, size_t pool_size);
	void* (*alloc)();
	int (*release)(void *ptr);
	void* (*locate)(void *ptr);
};

#define CORO_FLAG_END 0x00000001

#define CORO_CTX_TAG 0x9988abcd

struct coro_ctx
{
	void *sp;
	struct coro_ctx *prev;
	struct coro_ctx *next;
	void *ret;
	unsigned int flag;
	unsigned int tag;
};

void coro_switch(struct coro_ctx *from, struct coro_ctx *to)
	__attribute__ ((noinline, regparm(0)));

#endif // __CORO_COMM_H__
