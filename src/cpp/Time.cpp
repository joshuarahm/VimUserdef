#include "cpp/Time.hpp"
#include <cstdlib>

void Time::millisInFuture( struct timespec* ts, timeout_t millis ) {
#ifdef __MACH__
	clock_serv_t cclock;
	mach_timespec_t mts;
	host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
	clock_get_time(cclock, &mts);
	mach_port_deallocate(mach_task_self(), cclock);
	ts->tv_sec = mts.tv_sec;
	ts->tv_nsec = mts.tv_nsec;
#else
	clock_gettime(CLOCK_REALTIME, ts);
#endif

    /* Add 50 ms */
    ts->tv_nsec += millis * 1000000;
    ts->tv_sec += ts->tv_nsec / 1000000000;
    ts->tv_nsec %= 1000000000;
}

void Time::millisInFuture( struct timespec& ts, timeout_t millis ) {
    Time::millisInFuture( &ts, millis ) ;
}

void Time::fromMillis( struct timespec& ts, timeout_t timeout ) {
    lldiv_t ret = lldiv( timeout * 1000000, 1000000000) ;
    ts.tv_sec = ret.quot ;
    ts.tv_nsec = ret.rem ;
}

timeout_t Time::toMillis( struct timespec& ts ) {
    return (ts.tv_sec * 1000000000 + ts.tv_nsec) / 1000000 ;
}

timeout_t Time::sleep( timeout_t timeout ) {
    struct timespec ts ;
    struct timespec ts_out ;
    Time::fromMillis( ts, timeout ) ;
    nanosleep( &ts, &ts_out ) ;
    return toMillis( ts_out ) ;
}
