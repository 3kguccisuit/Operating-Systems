#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdbool.h>

#define NUM_THREADS 3

int buffer = 0;
pthread_mutex_t mutex;

void *threadFunction(void *arg)
{

    int *counter = malloc(sizeof(int));

    while (true)
    {
        // Sleep to allow CPU time for other threads
        sleep(1);
        // Lock the mutex before accessing shared resource
        pthread_mutex_lock(&mutex);
        if (buffer == 15)
        {
            pthread_mutex_unlock(&mutex);
            break;
        }
        buffer++;

        printf("TID: %lu,  PID: %d, Buffer value: %d\n",
               (unsigned long)pthread_self(), getpid(), buffer);

        (*counter)++;
        // Unlock the mutex when done with shared resource
        pthread_mutex_unlock(&mutex);
    }

    return (void *)counter;
}

int main()
{

    // Initialize the mutex
    pthread_mutex_init(&mutex, NULL);

    void *returnValue;
    // Arrays to store thread IDs and arguments
    pthread_t threads[NUM_THREADS];

    // Create threads
    for (int i = 0; i < NUM_THREADS; i++)
    {
        int rc = pthread_create(&threads[i], NULL, threadFunction, NULL);
        printf("Thread %d created\n", i);
    }

    // Wait for threads to finish
    for (int i = 0; i < NUM_THREADS; i++)
    {
        pthread_join(threads[i], (void **)&returnValue);
        printf("\nTID: %lu, worked on buffer, %d times", threads[i], *(int *)returnValue);
    }

    // Clean up and exit
    free(returnValue);
    pthread_mutex_destroy(&mutex);
    pthread_exit(NULL);
}
