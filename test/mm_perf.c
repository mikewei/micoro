#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include "coro_mm.h"

//#define USE_MALLOC

#define N (100000)
static void *tab[N];
static size_t cur = 0;

void prepare()
{
	size_t sz = 4096, i;
#ifndef USE_MALLOC
	assert(coro_mm_default_ops->init(&sz, N) == 0);
#endif
	assert(sz == 4096);
	for (i=0; i<N; i++) {
#ifdef USE_MALLOC
		assert(tab[cur] = malloc(4096));	
#else
		assert(tab[i] = coro_mm_default_ops->alloc());
#endif
	}
	srand(time(NULL));
	cur = rand() % N;
}

void testcase()
{
#ifdef USE_MALLOC
	free(tab[cur]);
	assert(tab[cur] = malloc(4096));	
#else
	assert(coro_mm_default_ops->release(tab[cur]) == 0);
	assert(tab[cur] = coro_mm_default_ops->alloc());
#endif
	cur = (cur + 1) % N;
}
