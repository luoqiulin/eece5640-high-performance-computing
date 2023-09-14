/*
** trylock.c
**
** Demonstrate a simple use of pthread_mutex_trylock. The
** counter_thread updates a shared counter at intervals, and a
** monitor_thread occasionally reports the current value of the
** counter -- but only if the mutex is not already locked by
** counter_thread.
**/

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define err_abort(code,text) do { \
    fprintf (stderr, "%s at \"%s\":%d: %s\n", \
        text, __FILE__, __LINE__, strerror (code)); \
    abort (); \
    } while (0)
#define errno_abort(text) do { \
    fprintf (stderr, "%s at \"%s\":%d: %s\n", \
        text, __FILE__, __LINE__, strerror (errno)); \
    abort (); \
    } while (0)


#define SPIN 100000000

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
long counter;
time_t end_time;

/*
** Thread start routine that repeatedly locks a mutex and
** increments a counter.
**/
void *counter_thread (void *arg)
{
    int status;
    int spin;

/*
** Until end_time, increment the counter each
** second. Instead of just incrementing the counter, it
** sleeps for another second with the mutex locked, to give
** monitor_thread a reasonable chance of running.
**/
    while (time (NULL) < end_time)
    {
        status = pthread_mutex_lock (&mutex);
        if (status == 0)
             printf("Lock mutex\n");
        for (spin = 0; spin < SPIN; spin++)
            counter++;
        status = pthread_mutex_unlock (&mutex);
        if (status == 0)
            printf("Unlock mutex\n");
        sleep (1);
    }
    printf ("Counter is %ld\n", counter);
    return NULL;
}

/*
** Thread start routine to "monitor" the counter. Every 3
** seconds, try to lock the mutex and read the counter. If the
** trylock fails, skip this cycle.
**/
void *monitor_thread (void *arg)
{
    int status;
    int misses = 0;


/*
** Loop until end_time, checking the counter every 3
** seconds.
**/
    while (time (NULL) < end_time)
    {
        sleep (1);
        status = pthread_mutex_trylock (&mutex);
        if (status == 0)
        {
            printf("Trylock mutex was successful\n");
            printf ("Counter is %ld\n", counter);
            status = pthread_mutex_unlock (&mutex);
            if (status == 0)
                printf("Unlock mutex\n");
        } 
	else
        {
            printf("Trylock mutex was unsuccessful\n");
            misses++;    /* Count "misses" on the lock */
        }
    }
    printf ("Monitor thread missed update %d times.\n", misses);
    return NULL;
}

int main (int argc, char *argv[])
{
    int status;
    pthread_t counter_thread_id;
    pthread_t monitor_thread_id;

    end_time = time (NULL) + 60;        /* Run for 1 minute */
    status = pthread_create (
        &counter_thread_id, NULL, counter_thread, NULL);
    if (status == 0)
        printf("Created counter thread\n");
    status = pthread_create (
        &monitor_thread_id, NULL, monitor_thread, NULL);
    if (status == 0)
        printf("Created monitor thread\n");
    status = pthread_join (counter_thread_id, NULL);
    if (status == 0)
        printf("Joined counter thread\n");
    status = pthread_join (monitor_thread_id, NULL);
    if (status == 0)
        printf("Joined monitor thread\n");
    return 0;
}
