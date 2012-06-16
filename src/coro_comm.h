#ifndef __CORO_COMM_H__
#define __CORO_COMM_H__

#include <stdlib.h>
#include "micoro.h"
#include "mt_utils.h"

#define CORO_FLAG_END 0x00000001

#define CORO_CTX_TAG 0x9988abcd

struct coro_ctx
{
	void *sp;
	struct coro_ctx *prev;
	struct coro_ctx *next;
	void *ret;
	light_lock_t lock;
	unsigned int flag;
	unsigned int tag;
};

void coro_switch(struct coro_ctx *from, struct coro_ctx *to)
	__attribute__ ((noinline, regparm(0)));

#endif // __CORO_COMM_H__
