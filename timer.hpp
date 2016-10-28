#ifndef __performance_timer__
#define __performance_timer__

//#include "sys.hpp"
#include <time.h>
#include <ctime>
#include <mach/clock.h>
#include <mach/mach.h>

typedef struct timespec TimePoint;

inline TimePoint currentTime() {
	struct timespec t;
	clock_serv_t cclock;
	mach_timespec_t mts;
	host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
	clock_get_time(cclock, &mts);
	mach_port_deallocate(mach_task_self(), cclock);
	t.tv_sec = mts.tv_sec;
	t.tv_nsec = mts.tv_nsec;
	return t;
}

// в секундах
inline double elapsedTime(const TimePoint& t1, const TimePoint& t2) {
	long double s = t2.tv_sec - t1.tv_sec;
	long double ns = t2.tv_nsec - t1.tv_nsec;
	s += ns * 1e-9;
	return double(s);
}

#endif


