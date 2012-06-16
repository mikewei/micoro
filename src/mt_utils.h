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

#include <sched.h>
#include <stdint.h>

#define CASSERT(predicate) _impl_CASSERT_LINE(predicate,__LINE__)
#define _impl_PASTE(a,b) a##b
#define _impl_CASSERT_LINE(predicate, line) \
	typedef char _impl_PASTE(assertion_failed_line_,line)[2*!!(predicate)-1];


static inline void atom_swap32(uint32_t *target, uint32_t *value)
{
	__asm__ __volatile__ (
			"xchgl %0, %1"
			:"=r"(*value)
			:"m"(*target), "0"(*value)
			:"memory" );
}

typedef struct {
	volatile int val;
} light_lock_t;

#define LIGHT_LOCK_INIT {1}
#define light_lock_init light_unlock

static inline void light_lock(light_lock_t *lock)
{
	/* fast spin
	 * borrowed from kernel-2.6.16
	 * ~16ns if no spin
	 */
	__asm__ __volatile__ (
			"\n1:\t" );

	__asm__ __volatile__ (
			"\n2:\t"
			"lock ; "
			"decl %0\n\t"
			"jns 5f\n"
			"3:\t"
			"rep;nop\n\t"
			"incl %1\n\t"
			"cmpl $5000,%1\n\t" /* ~ 3us */
			"jae 4f\n\t"
			"cmpl $0,%0\n\t"
			"jle 3b\n\t"
			"jmp 2b\n"
			"4:\n\t"
			:"=m"(lock->val)
			:"r"(0)
			:"memory" );
	/* slow spin
	 * if the lock-owner-thread is SUSPENDED busywait is meaningless
	 * sched_yield may help or less harmful
	 */
	while (lock->val <= 0) {
		sched_yield();
	}
	__asm__ __volatile__ (
			"jmp 1b\n"
			"5:\n\t");
}

static inline void light_unlock(light_lock_t *lock)
{
	__asm__ __volatile__ (
			"movl $1,%0"
			:"=m"(lock->val)
			: :"memory" );
}

int init_once(int *init_flag);

#endif // __MT_UTILS_H__
