#include "cpp/Condition.hpp"

Condition::Condition() {
    pthread_cond_init( & this->m_condition, NULL ) ;
}

void Condition::signal() {
   pthread_cond_signal( & this->m_condition ) ;
} ;

bool Condition::wait( Mutex& mutex, timeout_t timeout )  {
    if( timeout < 0 ) {
        return !pthread_cond_wait( & this->m_condition, mutex.raw() ) ;
    } else {
        timespec ts ;
        Time::millisInFuture( &ts, timeout ) ;
        return !pthread_cond_timedwait( & this->m_condition, mutex.raw(), &ts ) ;
    }
} ;

Condition::~Condition() {
    pthread_cond_destroy( & this->m_condition ) ;
}
