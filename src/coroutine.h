#ifndef __COROUTINE_H__
#define __COROUTINE_H__

#include <stdlib.h>

struct coro_ctx;

typedef struct {
	struct coro_ctx *ctx;
} coro_t;

int coro_init(size_t ctx_size_order, size_t pool_size);

int coro_create(coro_t *coro, void* (*f)(void*));
void* coro_resume(coro_t *coro, void *arg);
void* coro_yield(void *arg);

static inline int coro_err(const coro_t *coro)
{
	return (!coro || !coro->ctx);
}

#endif // __COROUTINE_H__
