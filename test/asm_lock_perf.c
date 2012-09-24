#include <sched.h>
#include <pthread.h>
#include <stdio.h>
#include <assert.h>
#include "mt_utils.h"

#define N (50000)

struct block {
	char buf[4096];
};
static struct block array[N];
static volatile size_t cur;
static volatile size_t tmp;

void prepare()
{
}

void testcase()
{
	__asm__ __volatile__ (
			"lock; decl %0\n\t"
			:"=m"(tmp));

	array[cur].buf[0] = 1;
	cur = (cur + 1) % N;
}
