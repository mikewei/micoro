#ifndef __MT_UTILS_H__
#define __MT_UTILS_H__

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
	volatile uint32_t word;
} light_lock_t;

static inline void light_lock(light_lock_t *lock)
{
	uint32_t count = 0;

	/* fast spin */
	__asm__ __volatile__ (
			"\n1:\t"
			"lock ; "
			"decb %0\n\t"
			"jns 3f\n"
			"2:\t"
			"rep;nop\n\t"
			"inc %1\n\t"
			"cmp $1000000,%1\n\t"
			"jae 3f\n\t"
			"cmpb $0,%0\n\t"
			"jle 2b\n\t"
			"jmp 1b\n"
			"3:\n\t"
			:"=m"(lock->word), "=r"(count)
			:"1"(count)
			:"memory" );
	if (count < 1000000)
		return;

	/* slow spin */
	for (;;) {
		if (lock->word > 0) {
			register int done = 0;
			__asm__ __volatile__ (
					"lock ; "
					"decb %0\n\t"
					"js 2f\n\t"
					"mov $1,%1\n"
					"2:\n\t"
					:"=m"(lock->word), "=r"(done)
					:"1"(done)
					:"memory" );
			if (done) return;
		}
		sched_yield();
	}
}

static inline void light_unlock(light_lock_t *lock)
{
	__asm__ __volatile__ (
			"movb $1,%0"
			:"=m"(lock->word)
			: :"memory" );
}

#endif // __MT_UTILS_H__
