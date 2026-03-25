// monitor.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <mqueue.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include "common.h"

// initial stock
int tyre_stock = INITIAL_TYRE_STOCK;
int engine_stock = INITIAL_ENGINE_STOCK;

mqd_t mq;
int low_stock_flag = 0;

pthread_mutex_t lock;

//  Logger Thread
void* logger_thread(void* arg) {

    int fd = *(int*)arg;
    struct order ord;

    while (1) {
            
             ssize_t bytes = read(fd, &ord, sizeof(ord));

              if (bytes <= 0) {
                  perror("read failed");
                  continue;
              }
              
            pthread_mutex_lock(&lock);
            int supplied_tyre, supplied_engine;

            // tyre logic
            if (tyre_stock >= ord.tyre) {
                supplied_tyre = ord.tyre;
                tyre_stock -= ord.tyre;
            } else {
                supplied_tyre = tyre_stock;
                tyre_stock = 0;
                low_stock_flag = 1;
            }

            // engine logic
            if (engine_stock >= ord.engine) {
                supplied_engine = ord.engine;
                engine_stock -= ord.engine;
            } else {
                supplied_engine = engine_stock;
                engine_stock = 0;
                low_stock_flag = 1;
            }
            
           
            // output
            printf("\n----------------------------------------\n");
            printf("\nOrder %d :\n", ord.order_id);
            printf("Tyre = %d\n", ord.tyre);
            printf("Engine = %d\n", ord.engine);

            printf("\nSupplied :\n");
            printf("Tyre = %d\n", supplied_tyre);
            printf("Engine = %d\n", supplied_engine);

            printf("\nTotal Stock Remaining :\n");
            printf("Tyre = %d\n", tyre_stock);
            printf("Engine = %d\n", engine_stock);
            
            // CASE 1: Only tyres finished
            if (tyre_stock == 0 && engine_stock > 0) {
                printf("\nNOTICE: Tyre stock unavailable. Processing only engine orders.\n");
                ord.tyre = 0;
            }

            // CASE 2: Only engines finished
            if (engine_stock == 0 && tyre_stock > 0) {
                printf("\nNOTICE: Engine stock unavailable. Processing only tyre orders.\n");
                ord.engine = 0;
            }
                        
             if (tyre_stock == 0 && engine_stock == 0)
              {
                  printf("\nSYSTEM NOTICE: Inventory depleted. Order processing stopped.\n");
                  
                  pthread_mutex_unlock(&lock);
                  // send signal to main process
                  kill(getppid(), SIGINT);

                  exit(0);
              }
              pthread_mutex_unlock(&lock);

        }
    }


// 🧵 Alert Thread
void* alert_thread(void* arg) {

    char msg[MAX_MSG_SIZE];

    while (1) {
    
        pthread_mutex_lock(&lock);
        if (low_stock_flag) {

            strcpy(msg, "LOW STOCK! Reorder required");

            if (mq_send(mq, msg, MAX_MSG_SIZE, 0) == -1) {
                perror("mq_send");
            }

            low_stock_flag = 0;
        }
        pthread_mutex_unlock(&lock);

        sleep(1);
    }
}

int main(int argc, char *argv[]) {

    if (argc < 2) {
        printf("Pipe FD not received\n");
        return 1;
    }

    int fd = atoi(argv[1]);  // read end

    // open MQ
    mq = mq_open(QUEUE_NAME, O_WRONLY);
    if (mq == -1) {
        perror("mq_open");
        exit(1);
    }
    
    pthread_mutex_init(&lock, NULL);

    pthread_t t1, t2;

    pthread_create(&t1, NULL, logger_thread, &fd);
    pthread_create(&t2, NULL, alert_thread, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    
    //destroy mutex
     pthread_mutex_destroy(&lock);

    return 0;
}
