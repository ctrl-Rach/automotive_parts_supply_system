#ifndef COMMON_H
#define COMMON_H

#include <mqueue.h>   

// Order structure
struct order {
    int order_id;
    int tire;
    int engine;
};

// Message Queue
#define QUEUE_NAME "/test_queue"

// Initial Stock
#define INITIAL_TIRE_STOCK 100
#define INITIAL_ENGINE_STOCK 100

// Message size 
#define MAX_MSG_SIZE 128

#endif
