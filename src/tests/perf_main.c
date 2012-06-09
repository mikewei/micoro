#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <locale.h>
#include <stdio.h>

#ifndef TEST_TIME
#	define TEST_TIME 10
#endif

void prepare() __attribute__((weak));
void done() __attribute__((weak));
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

	if (done) done();

	printf("count: %'llu\n", count);
	printf(" time: %'d.%06d s\n", (int)tv_use.tv_sec, (int)tv_use.tv_usec);
	printf("   HZ: %'f\n", count/(tv_use.tv_sec+0.000001*tv_use.tv_usec));
	printf(" 1/HZ: %'.9f s\n", (tv_use.tv_sec+0.000001*tv_use.tv_usec)/count);

	return 0;
}
