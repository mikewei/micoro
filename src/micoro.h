#ifndef __COROUTINE_H__
#define __COROUTINE_H__

#include <stdlib.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct coro_ctx;
typedef struct {
	struct coro_ctx *ctx;
} coro_t;

int coro_init(size_t ctx_size, size_t pool_size);

int coro_create(coro_t *coro, void* (*f)(void*));

void* coro_resume(coro_t *coro, void *arg);

void* coro_yield(void *arg);

int coro_self(coro_t *coro);

static inline int coro_err(const coro_t *coro)
{
	return (!coro || !coro->ctx);
}

struct coro_stat
{
	uint64_t create_count;
	uint64_t resume_count;
	uint64_t yield_count;
	uint64_t destroy_count;
	uint32_t alive_coro_num;
	uint32_t pool_left_num;
};
void coro_get_stat(struct coro_stat *stat);

#ifdef __cplusplus
}
#endif

#endif // __COROUTINE_H__
