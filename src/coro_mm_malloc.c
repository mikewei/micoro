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
