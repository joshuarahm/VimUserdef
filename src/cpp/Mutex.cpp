#include "cpp/Mutex.hpp"
#include "cpp/Time.hpp"

Mutex::Mutex() {
    pthread_mutex_init( & this->mutex, NULL ) ;
}

int Mutex::lock( timeout_t timeout ) {
    if( timeout < 0 ) {
        return pthread_mutex_lock( &this->mutex ) ;
    } else {
        timespec ts ;
        Time::millisInFuture( &ts, timeout ) ;
        return pthread_mutex_timedlock(&this->mutex,&ts) ;
    }
}

int Mutex::unlock() {
    return pthread_mutex_unlock( &this->mutex ) ;
}

bool Mutex::tryLock() {
    return !pthread_mutex_trylock( &this->mutex ) ;
}

Mutex::~Mutex() {
    pthread_mutex_destroy( &this->mutex ) ;
}
