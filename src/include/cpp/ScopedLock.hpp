#ifndef SCOPEDLOCK_HPP_
#define SCOPEDLOCK_HPP_

/*
 * Author: jrahm
 * created: 2014/01/29
 * ScopedLock.hpp: <description>
 */

#include "cpp/Mutex.hpp"

class ScopedLock {
public:
    inline ScopedLock( Mutex& mutex ) :
        mutex( mutex ) {
            this->mutex.lock( ) ;
        }

    inline ~ScopedLock( ) {
        this->mutex.unlock() ;
    }
private:
    Mutex& mutex ;
};

#endif /* SCOPEDLOCK_HPP_ */
