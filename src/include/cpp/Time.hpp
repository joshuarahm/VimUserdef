#ifndef TIME_HPP_
#define TIME_HPP_

/*
 * Author: jrahm
 * created: 2014/01/29
 * Time.hpp: <description>
 */

#include <time.h>
#include <inttypes.h>

typedef int64_t timeout_t ;

/*
 * The class that finally adds an intuitive inteface
 * to get time values in terms of milliseconds
 */
class Time {
public:
    /*
     * Populate a timespec struct with the value equal to now +
     * millis milliseconds.
     */
    static void millisInFuture( struct timespec* ts, timeout_t millis ) ;
    static void millisInFuture( struct timespec& ts, timeout_t millis ) ;

    /*
     * populate a timespec with the value representative of
     * millis milliseconds.
     */
    static void fromMillis( struct timespec& ts, timeout_t millis ) ;

    /*
     * Converts a timespec to milliseconds
     */
    static timeout_t toMillis( struct timespec& ts ) ;

    /*
     * Sleep for `millis` milliseconds
     * returns the remaining milliseconds
     */
    static timeout_t sleep( timeout_t millis ) ;
} ;

#endif /* TIME_HPP_ */
