#ifndef THREAD_HPP_
#define THREAD_HPP_

#include <inttypes.h>
#include <pthread.h>

/*
 * Author: jrahm
 * created: 2014/01/29
 * Thread.hpp: <description>
 */

class Thread {
public:
    /*
     * The method which is called when the thread is started.
     */
    virtual void run() = 0;

    /*
     * Starts the thread
     */
    virtual int start( pthread_attr_t* attrs = NULL ) ;

    /*
     * Waits for the thread to exit
     */
    int join( ) ;

    virtual ~Thread() ;

protected:
    pthread_t m_thread ;
} ;

#endif /* THREAD_HPP_ */
