#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
    int pipefd[2];
    pid_t pid;
    // Create a pipe
    pipe(pipefd);

    // Fork a child process
    switch (pid = fork())
    {
    case -1: // error
        perror("-1 fork()");
        break;
    case 0: // child
        printf("child\n");
        fflush(stdout);
        // Close the read end of the pipe
        close(pipefd[0]);

        // Redirect stdout to the write end of the pipe
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);

        // Execute the "ls /" command
        execlp("ls", "ls", "/", (char *)NULL);

    default: // parent
        wait(NULL);
        printf("parent\n");
        fflush(stdout);
        // Close the write end of the pipe
        close(pipefd[1]);

        // Redirect stdin from the read end of the pipe
        dup2(pipefd[0], STDIN_FILENO);
        close(pipefd[0]);

        // Execute the "wc -l" command
        execlp("wc", "wc", "-l", (char *)NULL);
        printf("parent\n");
        break;
    }
}
