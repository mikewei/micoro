#ifndef __CORO_MM_H__
#define __CORO_MM_H__

struct coro_mm_ops;

extern struct coro_mm_ops *coro_mm_malloc_ops;
extern struct coro_mm_ops *coro_mm_pool_ops;

#define coro_mm_default_ops coro_mm_pool_ops

#endif // __CORO_MM_H__
