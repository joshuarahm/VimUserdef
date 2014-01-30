#ifndef MUTEX_HPP_
#define MUTEX_HPP_

/*
 * Author: jrahm
 * created: 2014/01/29
 * Mutex.hpp: <description>
 */

#include <stdint.h>
#include <pthread.h>

class Mutex {
public:
    Mutex() ;
    virtual ~Mutex() ;

    /* Lock the mutex */
    virtual int lock( int64_t timeout=-1 ) ;

    /* Unlock the mutex */
    virtual int unlock() ;

    /* try to lock the mutex */
    virtual bool tryLock() ;

    inline pthread_mutex_t* raw() {
        return &mutex ;
    } ;
private:
    pthread_mutex_t mutex ;
} ;

#endif /* MUTEX_HPP_ */
