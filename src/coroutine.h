#ifndef __COROUTINE_H__
#define __COROUTINE_H__

#include <stdlib.h>
#include "coro_inc.h"

#ifdef __cplusplus
extern "C" {
#endif

int coro_init(size_t ctx_size, size_t pool_size);

int coro_create(coro_t *coro, void* (*f)(void*));

void* coro_resume(coro_t *coro, void *arg);

void* coro_yield(void *arg);

int coro_self(coro_t *coro);

int coro_err(const coro_t *coro);

#ifdef __cplusplus
}
#endif

#endif // __COROUTINE_H__
