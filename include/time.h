#ifndef _TIME_H
#define _TIME_H
#include <types.h>
struct tm {
	uint32_t tm_sec;
	uint32_t tm_min;
	uint32_t tm_hour;
	uint32_t tm_mday;
	uint32_t tm_mon;
	uint32_t tm_year;
	uint32_t tm_wday;
	uint32_t tm_yday;
	uint32_t tm_isdst;
};

extern uint32_t startup_time;

uint32_t kernel_mktime(struct tm * tm);
void time_init(void);
uint32_t get_sec_from_1970(void);
#endif/*!_TIME_H*/
