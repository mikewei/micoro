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
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <locale.h>
#include <stdio.h>

#ifndef TEST_TIME
#	define TEST_TIME 10
#endif

void prepare() __attribute__((weak));
void testcase();

static unsigned long long count;
static unsigned long timeout;

void on_timeout(int sig)
{
	timeout = 1;
}

int main(int argc, char *argv[])
{
	struct timeval tv_begin, tv_end, tv_use;

	setlocale(LC_ALL, "");
	signal(SIGALRM, on_timeout);

	if (prepare) prepare();

	alarm(TEST_TIME);
	gettimeofday(&tv_begin, NULL);

	for (count = 0; !timeout; count++)
	   	testcase();

	gettimeofday(&tv_end, NULL);

	timersub(&tv_end, &tv_begin, &tv_use);

	printf("count: %'llu\n", count);
	printf(" time: %'d.%06d s\n", (int)tv_use.tv_sec, (int)tv_use.tv_usec);
	printf("   HZ: %'f\n", count/(tv_use.tv_sec+0.000001*tv_use.tv_usec));
	printf(" 1/HZ: %'.9f s\n", (tv_use.tv_sec+0.000001*tv_use.tv_usec)/count);

	return 0;
}
