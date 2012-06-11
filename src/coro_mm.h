#ifndef __CORO_MM_H__
#define __CORO_MM_H__

#include <stdint.h>

struct coro_mm_stat
{
	uint64_t alloc_count;
	uint64_t release_count;
	uint32_t use_block_num;
	uint32_t free_block_num;
};

struct coro_mm_ops
{
	int (*init)(size_t *alloc_size, size_t pool_size);
	void* (*alloc)();
	int (*release)(void *ptr);
	void* (*locate)(void *ptr);
	void (*get_stat)(struct coro_mm_stat *stat);
};

extern struct coro_mm_ops *coro_mm_malloc_ops;
extern struct coro_mm_ops *coro_mm_pool_ops;

#define coro_mm_default_ops coro_mm_pool_ops

#endif // __CORO_MM_H__
