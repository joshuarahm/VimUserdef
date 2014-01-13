#ifndef BLOCKING_QUEUE_H_
#define BLOCKING_QUEUE_H_

#include <pthread.h>
#include <inttypes.h>
#include <errno.h>

#define BQ_OK 0
#define BQ_TIMEOUT ETIMEDOUT

struct list {
	void* data ;
	struct list* next ;
} ;

typedef struct {
	pthread_mutex_t _m_mutex ;
	pthread_cond_t _m_condition ;
	struct list* _m_head ;
	struct list* _m_tail ;
} blocking_queue_t ;

/*
 * construct and return a new blocking queue
 */
blocking_queue_t* new_blocking_queue( void ) ;

/*
 * append to the blocking queue.
 */
void blocking_queue_add( blocking_queue_t* queue, void* data ) ;

/*
 * take the next item from the queue, or wait until
 * an item comes
 */
int blocking_queue_take( blocking_queue_t* queue, void** into, uint64_t timeout ) ;

#endif
