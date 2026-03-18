#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>
#include <mqueue.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include "common.h"

#define QUEUE_NAME "/test_queue"
#define LOG_FILE "main_log.txt"

mqd_t mq;
int fd;

// SIGINT 
void handle_sigint(int sig) {
    printf("\nShutting down...\n");

    mq_close(mq);
    mq_unlink(QUEUE_NAME);
    waitpid(-1, NULL, 0); //wait for any child to terminate to avoid zombie process
    close(fd);

    exit(0);
}

// SIGUSR1 
void handle_sigusr1(int sig) {
    printf("\n[MAIN] Status requested\n");
}

// 🧵 Alert listener
void alert_listener() {
    char buffer[MAX_MSG_SIZE];

    while (1) {
        if (mq_receive(mq, buffer, sizeof(buffer), NULL) > 0) {

            printf("\n[ALERT]: %s\n", buffer);
            lseek(fd, 0, SEEK_END); 
            write(fd, buffer, strlen(buffer));
            write(fd, "\n", 1);
        }
    }
}

int main() {
   
    printf("  AUTOMOTIVE PARTS SUPPLY SYSTEM\n");
    printf("Initializing system...\n\n");

    sleep(1);

    printf("System Ready. Processing Orders...\n\n");

    struct mq_attr attr;

    // MQ setup
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = 0;

    mq = mq_open(QUEUE_NAME, O_CREAT | O_RDONLY, 0666, &attr);
    if (mq == -1) {
        perror("mq_open");
        exit(1);
    }

    // File open
    fd = open(LOG_FILE, O_CREAT | O_RDWR | O_TRUNC, 0666);
    if (fd < 0) {
        perror("file open");
        exit(1);
    }

    // CREATE PIPE
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(1);
    }

    // Signals
    signal(SIGINT, handle_sigint);
    signal(SIGUSR1, handle_sigusr1);

    // ORDER PROCESS
    pid_t p1 = fork();
    if (p1 == 0) {

        close(pipefd[0]); // close read end

        char fd_str[10];
        sprintf(fd_str, "%d", pipefd[1]);

        char *args[] = {"order", fd_str, NULL};
        execv("./myorder", args);

        perror("execv order failed");
        exit(1);
    }

    // MONITOR PROCESS
    pid_t p2 = fork();
    if (p2 == 0) {

        close(pipefd[1]); // close write end

        char fd_str[10];
        sprintf(fd_str, "%d", pipefd[0]);

        char *args[] = {"monitor", fd_str, NULL};
        execv("./mymonitor", args);

        perror("execv monitor failed");
        exit(1);
    }

    // close both ends 
    close(pipefd[0]);
    close(pipefd[1]);

    // Listen for alerts
    alert_listener();

    
    mq_close(mq);
    mq_unlink(QUEUE_NAME);
    close(fd);

    return 0;
}
