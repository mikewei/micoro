#include "mt_utils.h"

int init_once(int *init_flag)
{
	int flag;

	static light_lock_t init_lock = LIGHT_LOCK_INIT;
	light_lock(&init_lock);
	flag = *init_flag;
	*init_flag = 1;
	light_unlock(&init_lock);

	return (flag ? -1 : 0);
}

