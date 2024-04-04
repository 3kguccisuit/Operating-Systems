#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <unistd.h>
#include <ctype.h>

#define MAX_TEXT 2048
#define OUT    0
#define IN    1

struct message
{
    long msg_type;
    char text[MAX_TEXT];
};


// https://www.geeksforgeeks.org/count-words-in-a-given-string/
// returns number of words in str 
unsigned countWords(char *str)
{
    int state = OUT;
    unsigned wc = 0;  // word count
 
    // Scan all characters one by one
    while (*str)
    {
        // If next character is a separator, set the 
        // state as OUT
        if (*str == ' ' || *str == '\n' || *str == '\t')
            state = OUT;
 
        // If next character is not a word separator and 
        // state is OUT, then set the state as IN and 
        // increment word count
        else if (state == OUT)
        {
            state = IN;
            ++wc;
        }
 
        // Move to next character
        ++str;
    }
 
    return wc;
}


int main()
{
    int msgid;
    struct message msg;
    pid_t pid;

    // Create a message queue
    msgid = msgget(IPC_PRIVATE, 0666 | IPC_CREAT);

    switch (pid = fork())
    {
    case -1: // error
        break;
    case 0: // child (sender)
        printf("Child!!\n");
        FILE *fp;
        char content[MAX_TEXT];

        // Open the file for reading
        fp = fopen("text.txt", "r");
        if (fp == NULL)
        {
            perror("Error opening file");
            exit(1);
        }

        // Read the content of the file
        fread(content, sizeof(char), MAX_TEXT, fp);
        fclose(fp);

        // Prepare the message
        msg.msg_type = 1;
        strncpy(msg.text, content, MAX_TEXT);

        // Send the message
        if (msgsnd(msgid, &msg, sizeof(msg.text), 0) == -1)
        {
            perror("msgsnd");
            exit(1);
        }
        // terminate the child process
        exit(0);

    default: // parent (reciever)
        wait(NULL);
        printf("parent!\n");
        // Receive the message
        if (msgrcv(msgid, &msg, sizeof(msg.text), 1, 0) == -1)
        {
            perror("msgrcv");
            exit(1);
        }

         int numOfWords = countWords(msg.text);
         printf("Number of words: %d\n", numOfWords);
        break;
    }

    return 0;
}
