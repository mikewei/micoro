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
#ifndef __COROUTINE_H__
#define __COROUTINE_H__

#include <stdlib.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct coro_ctx;

typedef struct {
	struct coro_ctx *ctx;
} coro_t;

/* @coro_init
 * initialize coroutine library (e.g. alloc context pool)
 * must do this before calling any other functions
 *
 * @ctx_size : fixed context (including the stack) size for each coroutine,
 *             to avoid stack overflow >=8K is suggested
 * @pool_size: size of pre-allocated context pool, also means max 
 *             concurrent coroutines
 * @return   : 0 for success, <0 for error
 */
int coro_init(size_t ctx_size, size_t pool_size);

/* @coro_create
 * create a new coroutine
 * this function create context for the new coroutine without lauching it,
 * after this call done use coro_resume to launch the coroutine
 *
 * @coro  : output param, return handler of the new coroutine if no error
 * @f     : entry function of the new coroutine
 * @return: 0 for success, <0 for error
 */
int coro_create(coro_t *coro, void* (*f)(void*));

/* @coro_resume
 * resume execution of the suspending coroutine (as sub-coro of the caller  
 * which is either a coroutine or non-coroutine code)
 * a suspending coroutine may come from either case:
 *   1) a new coroutine created by coro_create
 *   2) a running coroutine had yield cpu by coro_yield
 * in case 1) the new coroutine is resumed to run from the beginning of the
 *            entry function
 * in case 2) the coroutine is resumed as if the coro_yield() returns
 * the caller is suspended as parent-coro waiting for the sub-coro to 
 * yield or exit
 *
 * @coro  : the coroutine to resume
 * @arg   : param passed to the coroutine (sub-coro)
 *          in case 1) @arg is assigned to the argument of entry function
 *          in case 2) @arg is returned from coro_yield()
 * @return: value of argument @arg in coro_yiled() from sub-coro
 */
void* coro_resume(coro_t *coro, void *arg);

/* @coro_yield
 * yield execution of current coroutine and resume execution of the 
 * parent-coro which is actually either a coroutine or non-coroutine code
 * current coroutine is suspended in this call and will be resumed by 
 * later coro_resume()
 *
 * @arg   : param passed to parent-coro
 *          @arg is returned from coro_resume() when parent-coro is resumed
 * @return: value of argument @arg in coro_resume() that resume the coro
 */
void* coro_yield(void *arg);

/* @coro_self
 * get current coroutine in which this function is called
 * note that all coroutines do come from coro_create, so calling coro_self 
 * from non-coroutine code (e.g. from main() ) will fail
 *
 * @coro  : output param, return handler of current coroutine if no error
 * @return: 0 for success, <0 for error
 */
int coro_self(coro_t *coro);

/* @coro_err
 * get error status of the coroutine
 *
 * @coro  : the coroutine to check
 * @return: 0 for no error, >0 indicate error number
 */
static inline int coro_err(const coro_t *coro)
{
	if (!coro || !coro->ctx)
		return 1;
	return 0;
}

/* coroutine library running status */
struct coro_stat
{
	/* total coro_create count */
	uint64_t create_count;
	/* total coro_resume count */
	uint64_t resume_count;
	/* total coro_yield count */
	uint64_t yield_count;
	/* total coroutine exit count */
	uint64_t destroy_count;
	/* concurrent running coroutine number now */
	uint32_t alive_coro_num;
	/* free slots number of context-pool now */
	uint32_t pool_left_num;
};

/* @coro_get_stat
 * get coroutine library running status
 *
 * @stat: output param, return coro_stat structure
 */
void coro_get_stat(struct coro_stat *stat);

#ifdef __cplusplus
}
#endif

#endif // __COROUTINE_H__
