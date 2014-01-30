#ifndef BLOCKINGQUEUE_HPP_
#define BLOCKINGQUEUE_HPP_

/*
 * Author: jrahm
 * created: 2014/01/29
 * BlockingQueue.hpp: <description>
 */

#include <queue>

#include "cpp/Mutex.hpp"
#include "cpp/Condition.hpp"
#include "cpp/ScopedLock.hpp"

template <class T, class delegate_T=std::queue<T> >
class BlockingQueue {
public:
    /* Construct and initialize a
     * blocking queue */
    BlockingQueue() :
        m_condition(),
        m_mutex(),
        m_queue() {}

    /* add a new value to the blocking queue */
    void push( T& val ) {
        ScopedLock _sl_( this->m_mutex ) ;
        m_queue.push( val ) ;

        if( m_queue.size() == 1 ) {
            this->m_condition.signal() ;
        }
    }

    /* return whatever is on the front of the blocking queue
     * if there is nothing on the head of the queue, then block
     * until there is something on the queue */
    T& front( ) {
        ScopedLock _sl_( this->m_mutex ) ;

        if( m_queue.empty() ) {
            m_condition.wait( this->m_mutex ) ;
        }

        return m_queue.front() ;
    }

    /* 
     * return what is on the front of the blocking queue.
     * If nothing is on the head of the blocking queue, then
     * waid for the specified timeout. Returns NULL on timeout
     */
    T* front_timed( timeout_t timeout ) {
        ScopedLock _sl_( this->m_mutex ) ;

        if( m_queue.empty() ) {
            if( ! m_condition.wait( this->m_mutex, timeout ) ) {
                return NULL ;
            }
        }

        return & m_queue.front() ;
    }

    /* remove the head element of the queue, if the queue is empty, then
     * this function will just return */
    void pop() {
        ScopedLock( this->m_mutex ) ;
        m_queue.pop() ;
    }

    virtual ~BlockingQueue() {}
private:
    delegate_T m_queue ;
    Mutex m_mutex ;
    Condition m_condition ;
} ;

#endif /* BLOCKINGQUEUE_HPP_ */
