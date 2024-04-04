#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/wait.h>

#define MAX 3
#define SHM_KEY 0x1234

struct shared_memory
{
    int VAR;
    int read_count;
    sem_t sem_read;
    sem_t sem_write;
};


void writerProcess(struct shared_memory *shm)
{
    while (shm->VAR < MAX)
    {
        sleep(1);
        sem_wait(&shm->sem_write);
        printf("The writer acquires the lock.\n");
        printf("The writer (%d) writes the value %d\n", getpid(), shm->VAR + 1);
        shm->VAR++;
        printf("The writer releases the lock.\n");
        sem_post(&shm->sem_write);
        sleep(1);
    }
}


void readerProcess(struct shared_memory *shm)
{

    while (shm->VAR < MAX)
    {

        sem_wait(&shm->sem_read);

        shm->read_count++;
        if (shm->read_count == 1)
        {
            sem_wait(&shm->sem_write);
            printf("The first reader acquires the lock.\n");
        }
        sem_post(&shm->sem_read);
        sleep(1);

        printf("The reader (%d) reads the value %d\n", getpid(), shm->VAR);

        sem_wait(&shm->sem_read);
        shm->read_count--;
        if (shm->read_count == 0)
        {
            sem_post(&shm->sem_write);
            printf("The last reader releases the lock.\n");
        }

        sem_post(&shm->sem_read);
        sleep(1);
    }
}

int main()
{
    int shm_id;
    struct shared_memory *shm;

    // Create shared memory
    shm_id = shmget(IPC_PRIVATE, sizeof(struct shared_memory), 0666 | IPC_CREAT);
    if (shm_id == -1)
    {
        perror("Error creating shared memory segment");
        return 1;
    }

    shm = shmat(shm_id, NULL, 0);
    if (shm == (void *)-1)
    {
        perror("shmat");
        return 1;
    }

    shm->VAR = 0;
    shm->read_count = 0;
    sem_init(&shm->sem_read, 1, 1);
    sem_init(&shm->sem_write, 1, 1);

    // Creating one writer and two readers
    for (int i = 0; i < 3; i++)
    {
        pid_t pid = fork();
        if (pid == -1)
        {
            perror("fork");
            return 1;
        }
        else if (pid == 0)
        {
            if (i == 0)
            {
                writerProcess(shm);
            }
            else
            {
                readerProcess(shm);
            }
            exit(0);
        }
    }

    // Wait for child processes to finish
    for (int i = 0; i < 3; i++)
    {
        wait(NULL);
    }

    // Cleanup
    sem_destroy(&shm->sem_read);
    sem_destroy(&shm->sem_write);
    shmdt(shm);
    shmctl(shm_id, IPC_RMID, 0);

    return 0;
}
