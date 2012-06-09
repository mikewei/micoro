#include <sched.h>
#include <pthread.h>
#include <stdio.h>
#include <assert.h>
#include "mt_utils.h"

//#define MT
//#define YIELD

static light_lock_t lock = LIGHT_LOCK_INIT;
static size_t count;

void testcase2()
{
	light_lock(&lock);
#ifdef YIELD
	sched_yield();
#endif
	light_unlock(&lock);
	count++;
}

void* entry(void *arg)
{
	while (1) {
		testcase2();
	}
	return NULL;
}

void done()
{
	printf("thread loop %lu\n", count);
}

void prepare()
{
#ifdef MT
	pthread_t t;
	assert(pthread_create(&t, NULL, entry, NULL) == 0);
#endif
}

void testcase()
{
	light_lock(&lock);
	light_unlock(&lock);
}
