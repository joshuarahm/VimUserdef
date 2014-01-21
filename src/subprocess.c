#include "util/subprocess.h"
#include "radiation.h"
#include "errno.h"
#include <string.h>
#include <wait.h>
#include <fcntl.h>

struct thread_args {
    error_callback_t callback ;
    void* arg ;
    FILE* err ;
} ;

static void* error_thread_run( void* args__ ) {
    struct thread_args* args = ( struct thread_args* ) args__ ;
    char buffer[1024] ;
    size_t bytes_read ;

    while ( (bytes_read = fread(buffer, 1, sizeof(buffer) - 1, args->err )) > 0 ) {
        buffer[bytes_read] = 0 ;
        args->callback( args->arg, buffer ) ;
    }
    
    fclose( args->err ) ; 
    free( args ) ;
    pthread_exit( NULL ) ;
}

/* 
 * This acts almost idetically to popen except that it allows the
 * user to register a callback for when there is action on stderr
 */
FILE* run_process( const char* command, error_callback_t callback, void* arg ) {;
    FILE* ret ;
    FILE* err ;

    pthread_t thread ;
    struct thread_args* args ; 

    int stdout_pipe[2] ;
    int stderr_pipe[2] ;

    if( pipe( stdout_pipe ) || pipe( stderr_pipe ) ) {
        return NULL ;
    }

    pid_t child ;
    if( (child = fork()) == 0 ) {
        dup2(stdout_pipe[1], 1) ;
        dup2(stderr_pipe[1], 2) ;
        
        close( stdout_pipe[0] ) ;
        close( stderr_pipe[0] ) ;
        
        execl("/bin/sh", "/bin/sh", "-c", command, NULL) ;
        fprintf(stderr, "Failed to execute command `%s`:\n%s\n", command, strerror(errno)) ;
        exit( 1 ) ; 
    } else {
        close( stdout_pipe[1] ) ;
        close( stderr_pipe[1] ) ;
    
        ret = fdopen( stdout_pipe[0], "r" ) ;
        err = fdopen( stderr_pipe[0], "r" ) ;

        if( callback != NULL && err ) {
            /* setup to kick off threadding */
            args = malloc( sizeof(struct thread_args) ) ;
            args->callback = callback ;
            args->arg = arg ;
            args->err = err ;
    
            /* kick off the thread that reads from the childs
             * stderr and notifies the callback */
            pthread_create( &thread, NULL, error_thread_run, args ) ;
        } else { 
            /* If the callback is NULL, then do nothing */
            if( err ) {
                fclose( err ) ;
            }
        }
    }

    return ret ;
}

FILE* spawn_oevp( const char* file, char *const argv[] ) {
    int out_pipe[2] ;
    if( pipe( out_pipe ) ) {
        return NULL ;
    }

    pid_t child ;
    if( (child = fork()) == 0 ) {
        dup2(out_pipe[1], STDOUT_FILENO) ;
        dup2(out_pipe[1], STDERR_FILENO) ;
        close( out_pipe[0] ) ;
        execvp( file, argv ) ;
        perror("Execvp failed") ;
        exit(1) ;
    }

    close( out_pipe[1] ) ;
    return fdopen(out_pipe[0], "r") ;
}

int spawn_waitvp( const char* file, char *const argv[], int options ) {
    pid_t child ;
    int output ;

    if( (child = fork()) == 0 ) {

        if( options & SPAWN_QUIET ) {
            /* we need to quiet down the process */
            output = open("/dev/null",O_RDONLY) ;
            dup2( output, STDOUT_FILENO ) ;
            dup2( output, STDERR_FILENO ) ;
        }

        execvp( file, argv ) ;
        perror("Execvp failed") ;
        exit(32) ;
    }
    
    int ret ;
    waitpid( child, &ret, 0 ) ;

    return ret ;
}
