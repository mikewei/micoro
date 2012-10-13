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
#ifndef __MT_UTILS_H__
#define __MT_UTILS_H__

#include <config.h>
#include <sched.h>
#include <stdint.h>

#define CASSERT(predicate) _impl_CASSERT_LINE(predicate,__LINE__)
#define _impl_PASTE(a,b) a##b
#define _impl_CASSERT_LINE(predicate, line) \
	typedef char _impl_PASTE(assertion_failed_line_,line)[2*!!(predicate)-1];

#ifndef MICORO_MT
#	define MICORO_MT 1
#endif

#if MICORO_MT
#	if MICORO_X86_OPTIMIZE
#		include "x86_mt_utils.h"
#		define ATOM_SWAP32 xchg_swap32
#		define MICORO_LOCK_T light_lock_t
#		define MICORO_LOCK_INITVAL LIGHT_LOCK_INIT
#		define MICORO_LOCK_INIT light_lock_init
#		define MICORO_LOCK light_lock
#		define MICORO_UNLOCK light_unlock
#	else
#		include <pthread.h>
#		define ATOM_SWAP32 lock_swap32
#		define MICORO_LOCK_T pthread_mutex_t
#		define MICORO_LOCK_INITVAL PTHREAD_MUTEX_INITIALIZER
#		define MICORO_LOCK_INIT(lock) pthread_mutex_init(lock, NULL)
#		define MICORO_LOCK pthread_mutex_lock
#		define MICORO_UNLOCK pthread_mutex_unlock
#	endif
#else
#   define ATOM_SWAP32 simple_swap32
#	define MICORO_LOCK_T int
#	define MICORO_LOCK_INITVAL {0}
#	define MICORO_LOCK_INIT(lock) do { (void)(lock); } while (0)
#	define MICORO_LOCK(lock) do { (void)(lock); } while (0)
#	define MICORO_UNLOCK(lock) do { (void)(lock); } while (0)
#endif

static inline void simple_swap32(uint32_t *target, uint32_t *value)
{
	uint32_t tmp = *target;
	*target = *value;
	*value = tmp;
}

static inline void lock_swap32(uint32_t *target, uint32_t *value)
{
	MICORO_LOCK_T lock = MICORO_LOCK_INITVAL;
	MICORO_LOCK(&lock);
	simple_swap32(target, value);
	MICORO_UNLOCK(&lock);
}

int init_once(int *init_flag);

#endif // __MT_UTILS_H__
