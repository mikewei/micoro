#include <stdio.h>
#include "micoro.h"

void* test1_coro(void *arg)
{
	int idx = 0; 
	char buf[1024];
	char *s;
	idx = snprintf(buf + idx, sizeof(buf) - idx, "%s ", (char *)arg);
	s = coro_yield("works!");
	idx = snprintf(buf + idx, sizeof(buf) - idx, "%s\n", (char *)s);
	printf("%s", buf);
	return "done";
}

int test1()
{
	void *s;
	coro_t coro;
	if (coro_init(4096, 10) < 0)
		return -1;
	if (coro_create(&coro, test1_coro) < 0)
		return -2;
	printf("new coro: %p\n", coro.ctx);
	s = coro_resume(&coro, "coro");
	s = coro_resume(&coro, s);
	printf("%s\n", (char *)s);
	return 0;
}

void* test2_producer(void *arg)
{
	int i;
	for (i = 0; i < 10; i++)
		coro_yield((void *)&i);
	return NULL;
}

void* test2_consumer(void *arg)
{
	coro_t *prod = (coro_t *)arg;
	while (1) {
		int *pi = coro_resume(prod, NULL);
		if (coro_err(prod)) break;
		printf("%d ", *pi);
	}
	printf("\n");
	return NULL;
}

int test2()
{
	coro_t prod, cons;
	if (coro_create(&prod, test2_producer) < 0
			|| coro_create(&cons, test2_consumer) < 0)
		return -1;
	coro_resume(&cons, &prod);
	return 0;
}

int main()
{
	if (test1() < 0) return 1;
	if (test2() < 0) return 2;
	return 0;
}

