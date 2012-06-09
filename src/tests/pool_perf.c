#include <time.h>
#include <stdlib.h>
#include <assert.h>
#include "coro_comm.h"
#include "coro_mm.h"

#define _assert

#define N (100000)
static void *tab[N];
static size_t cur = 0;

void prepare()
{
	size_t sz = 4096, i;
	assert(coro_mm_default_ops->init(&sz, N) == 0);
	assert(sz == 4096);
	for (i=0; i<N; i++) {
		assert(tab[i] = coro_mm_default_ops->alloc());
	}
	srand(time(NULL));
	cur = rand() % N;
}

void testcase()
{
	assert(coro_mm_default_ops->release(tab[cur]) == 0);
	assert(tab[cur] = coro_mm_default_ops->alloc());
	cur = (cur + 1) % N;
}
