#include "cpp/Thread.hpp"

#include <pthread.h>

static void* pthread_Thread_run( void* _thread ) {
    Thread* thread = (Thread*) _thread ;
    thread->run() ;
    pthread_exit( NULL ) ;
}

int Thread::start( pthread_attr_t* attrs ) {
    return pthread_create( & this->m_thread, attrs, pthread_Thread_run, this ) ;
}

int Thread::join( ) {
    return pthread_join( m_thread, NULL ) ;
}

Thread::~Thread() {}
