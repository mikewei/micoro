#include <time.h>
#include <stdlib.h>
#include <assert.h>
#include "micoro.h"

#define _assert

#define N (10000)
static coro_t tab[N];
static size_t cur = 0;

void* coro(void *arg)
{
	return NULL;
}

void prepare()
{
	size_t sz = 4096, i;
	assert(coro_init(sz, N) == 0);
	for (i=0; i<N; i++) {
		assert(coro_create(&tab[i], coro) == 0);
	}
	srand(time(NULL));
	cur = rand() % N;
}

void testcase()
{
	coro_resume(&tab[cur], NULL);
	assert(coro_create(&tab[cur], coro) == 0);
	cur = (cur + 1) % N;
}
