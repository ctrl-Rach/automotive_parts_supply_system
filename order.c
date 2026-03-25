// order.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "common.h"

int main(int argc, char *argv[]) {

    if (argc < 2) {
        printf("No pipe FD received\n");
        return 1;
    }

    int fd = atoi(argv[1]);  // write end of pipe

    struct order ord;
    int order_id = 1;

    srand(time(NULL));

    while (1) {

        ord.order_id = order_id++;
        ord.tyre = rand() % 10 + 1;
        ord.engine = rand() % 8 + 1;

        // send order to monitor
        if (write(fd, &ord, sizeof(ord)) < 0) 
        {
             perror("write failed");
        }
        sleep(3);
    }

    return 0;
}
