#include <sched.h>
#include <pthread.h>
#include <stdio.h>
#include <assert.h>
#include "mt_utils.h"

//#define MT
#define YIELD

static MICORO_LOCK_T lock = MICORO_LOCK_INITVAL;
static unsigned long count;

void testcase2()
{
	MICORO_LOCK(&lock);
#ifdef YIELD
	sched_yield();
#endif
	MICORO_UNLOCK(&lock);
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
	MICORO_LOCK(&lock);
	MICORO_UNLOCK(&lock);
}
