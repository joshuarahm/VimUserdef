#include "blocking_queue.h"

#include <stdlib.h>
#include <stdio.h>

#include <sys/time.h>

#ifdef __MACH__
#include <mach/clock.h>
#include <mach/mach.h>
#endif

extern FILE* logfile ;
#ifdef DEBUG
#define lprintf( fmt, ... ) \
	if( logfile ) { fprintf( logfile, fmt, ##__VA_ARGS__ ) ; fflush( logfile ) ; }
#else
#define lprintf( fmt, ... )
#endif	

#define sassert( expr, message, ... ) \
	if( ! (expr) ) { \
		fprintf( stderr, message, ##__VA_ARGS__ ) ; \
		}

static void millis_in_future( struct timespec* ts, long millis ) {
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

/* creates a new list node that
 * has the initial data value set and
 * the next node set to null */
static struct list* new_list_node( void* data ) {
	struct list* ret = (struct list*)calloc( sizeof(struct list), 1 ) ;
	ret->data = data ;
	return ret ;
}

/*
 * construct and return a new blocking queue
 */
blocking_queue_t* new_blocking_queue( void ) {
	blocking_queue_t* ret = (blocking_queue_t*)calloc( sizeof(blocking_queue_t), 1 ) ;
	pthread_mutex_init( &ret->_m_mutex, NULL ) ;
    pthread_cond_init( &ret->_m_condition, NULL ) ;
    pthread_cond_init( &ret->_m_digest_condition, NULL ) ;
	return ret ;
}

/*
 * append to the blocking queue.
 */
void blocking_queue_add( blocking_queue_t* queue, void* data ) {
    lprintf( __FILE__ ":%d locking mutex\n", __LINE__ ) ;
	pthread_mutex_lock ( &queue->_m_mutex ) ;
	
	struct list* list_node = new_list_node( data ) ;

	if( queue->_m_tail == NULL ) {
		sassert( queue->_m_head == NULL, "Head of queue should be null as well!"
			" This is a memory leak! %s:%d\n", __FILE__, __LINE__ ) ;
		queue->_m_head = queue->_m_tail = list_node ;
		/* signal the waiting thread */
		pthread_cond_signal( &queue->_m_condition ) ;
	} else {
		queue->_m_tail->next = list_node ;
		queue->_m_tail = list_node ;
	}

    lprintf( __FILE__ ":%d unlocking mutex\n", __LINE__ ) ;
	pthread_mutex_unlock ( &queue->_m_mutex ) ;
}

/*
 * take the next item from the queue, or wait until
 * an item comes
 */
int blocking_queue_take( blocking_queue_t* queue, void** into, uint64_t timeout ) {
    lprintf( __FILE__ ":%d locking mutex\n", __LINE__ ) ;
	pthread_mutex_lock( &queue->_m_mutex ) ;
	struct timespec ts;

	if( queue->_m_head == NULL ) {
		sassert( queue->_m_tail == NULL, "Tail of queue should be null as well!"
			" This is a memory leak and will result in lost data! %s:%d\n", __FILE__, __LINE__) ;

		/* plz don't explode */
		queue->_m_tail = NULL ;
		millis_in_future( &ts, timeout );
		/* wait for the queue to be filled again */
		if( pthread_cond_timedwait( &queue->_m_condition, &queue->_m_mutex, &ts ) == ETIMEDOUT ) {
            pthread_mutex_unlock( &queue->_m_mutex ) ;
			return BQ_TIMEOUT ;
		}
	}

	*into = queue->_m_head->data ;
	void* garbage = queue->_m_head ;
	
	if( queue->_m_head == queue->_m_tail ) {
		/* there is only one item */
		sassert( queue->_m_head-> next == NULL,
			"Element past tail pointer. This will cause a memory leaka!\n" ) ;

		queue->_m_head = NULL ;
		queue->_m_tail = NULL ;
	} else {
		queue->_m_head = queue->_m_head->next ;
	}

    lprintf( __FILE__ ":%d unlocking mutex\n", __LINE__ ) ;
	pthread_mutex_unlock( &queue->_m_mutex ) ;

	free( garbage ) ;
	return BQ_OK ;
}

int blocking_queue_wait_digest( blocking_queue_t* queue, uint64_t timeout ) {
	struct timespec ts;
	millis_in_future( &ts, timeout );

    int ret = BQ_OK ;
    
    pthread_mutex_lock( &queue->_m_mutex ) ;
    lprintf( __FILE__ ":%d locking mutex\n", __LINE__ ) ;

    if( pthread_cond_timedwait( &queue->_m_digest_condition, &queue->_m_mutex, &ts ) == ETIMEDOUT ) {
        ret = BQ_TIMEOUT ;
    }

    lprintf( __FILE__ ":%d unlocking mutex\n", __LINE__ ) ;
    pthread_mutex_unlock( &queue->_m_mutex ) ;

    return ret ;
}
