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
#ifndef __CORO_COMM_H__
#define __CORO_COMM_H__

#include <config.h>
#if !MICORO_X86_OPTIMIZE
#	if HAVE_UCONTEXT_H
#		include <ucontext.h>
#	elif HAVE_SYS_UCONTEXT_H
#		include <sys/ucontext.h>
#	endif
#endif
#include <stdlib.h>
#include "micoro.h"
#include "mt_utils.h"

#define CORO_FLAG_END 0x00000001

#define CORO_CTX_TAG 0x9988abcd

struct coro_ctx
{
#if MICORO_X86_OPTIMIZE
	void *sp;
#else
	ucontext_t uctx;
#endif
	struct coro_ctx *prev;
	struct coro_ctx *next;
	void *ret;
	MICORO_LOCK_T lock;
	unsigned int flag;
	unsigned int tag;
};

#if MICORO_X86_OPTIMIZE
#	define coro_switch x86_coro_switch
#	define coro_makectx x86_coro_makectx
#else
#	define coro_switch uctx_coro_switch
#	define coro_makectx uctx_coro_makectx
#endif

void coro_makectx(struct coro_ctx *ctx, size_t ctx_size, void* (*f)(void*));
void coro_switch(struct coro_ctx *from, struct coro_ctx *to)
	__attribute__ ((noinline, regparm(0)));

void __coro_main(void* (*f)(void*))
	__attribute__ ((noinline, regparm(0)));

#endif // __CORO_COMM_H__
