#include <stdlib.h>
#include <assert.h>
#include "coro_mm.h"

static size_t g_alloc_size;
static size_t g_pool_size;

static void **g_alloc_table;
static void *g_max_addr = NULL;

static struct coro_mm_stat g_stat;

static int init(size_t *alloc_size, size_t pool_size)
{
	g_alloc_size = *alloc_size;
	g_pool_size = pool_size;
	if (!(g_alloc_table = calloc(pool_size, sizeof(void*)))) {
		return -1;
	}
	return 0;
}

static void* alloc()
{
	static size_t index = 0;
	void *ptr = NULL;
	size_t i;
	for (i = 0; i < g_pool_size && !ptr; i++) {
		if (g_alloc_table[index] == NULL) {
			ptr = g_alloc_table[index] = malloc(g_alloc_size);
			g_stat.alloc_count++;
			g_stat.use_block_num++;
		}
		index = (index + 1) % g_pool_size;
	}
	if (ptr + g_alloc_size - 1 > g_max_addr)
		g_max_addr = ptr + g_alloc_size - 1;
	return ptr;
}

static int release(void *ptr)
{
	size_t i;
	for (i = 0; i < g_pool_size; i++) {
		if (g_alloc_table[i] == ptr) {
			g_alloc_table[i] = NULL;
			free(ptr);
			g_stat.release_count++;
			g_stat.use_block_num--;
			return 0;
		}
	}
	return -1;
}

static void* locate(void *ptr)
{
	size_t i;
	if (ptr > g_max_addr)
		return NULL;
	for (i = 0; i < g_pool_size; i++) {
		if (ptr >= g_alloc_table[i] && ptr < g_alloc_table[i] + g_alloc_size) {
			return g_alloc_table[i];
		}
	}
	return NULL;
}

static void get_stat(struct coro_mm_stat *stat)
{
	stat->alloc_count = g_stat.alloc_count;
	stat->release_count = g_stat.release_count;
	stat->use_block_num = g_stat.use_block_num;
	assert(g_pool_size >= g_stat.use_block_num);
	stat->free_block_num = g_pool_size - g_stat.use_block_num;
}

static struct coro_mm_ops g_ops = {
	.init = init,
	.alloc = alloc,
	.release = release,
	.locate = locate,
	.get_stat = get_stat
};

struct coro_mm_ops *coro_mm_malloc_ops = &g_ops;
