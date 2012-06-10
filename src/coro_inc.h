#ifndef __CORO_INC_H__
#define __CORO_INC_H__

struct coro_ctx;

typedef struct {
	struct coro_ctx *ctx;
} coro_t;

static inline int coro_err(const coro_t *coro)
{
	return (!coro || !coro->ctx);
}

#endif // __CORO_INC_H__
