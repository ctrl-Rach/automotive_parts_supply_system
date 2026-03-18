# Automotive Parts Supply System

## 1. Scenario Summary

This project simulates an automotive parts supply system where orders for tires and engines are generated, processed, and monitored. The system uses multiple processes and threads to handle order generation, stock management, alert generation, and logging.

Orders are generated randomly and sent to a monitoring process, which updates stock and triggers alerts when stock becomes low. The system shuts down automatically when inventory is exhausted.

---

## 2. Architecture Diagram

Process Level:

        mymain (Controller)
                |
                |  Message Queue (Alerts)
                v
        mymonitor (Processing + Threads)
                |
                |  Pipe (Orders)
                v
        myorder (Order Generator)


Thread Level (inside mymonitor):

Logger Thread  -> Processes orders and updates stock
Alert Thread   -> Sends low stock alerts

---

## 3. Build Steps

To compile the project, run:

make

---

## 4. Run Instructions

To execute the system, run:

make run

---

## 5. System Call Usage

File I/O:

* open: Creates log file
* write: Writes alerts to file
* lseek: Appends data to file
* close: Closes file descriptor

Process Control:

* fork: Creates child processes
* exec: Executes order and monitor programs
* waitpid: Waits for child termination

IPC:

* pipe: Transfers orders from myorder to mymonitor
* mq_open: Creates message queue
* mq_send: Sends alerts
* mq_receive: Receives alerts

Signals:

* signal: Registers signal handlers
* kill: Sends shutdown signal

Threads:

* pthread_create: Creates threads
* pthread_mutex: Synchronizes shared data

---

## 6. Sample Output


----------------------------------------
Order 1 :
Tire = 4
Engine = 2

Supplied :
Tire = 4
Engine = 2

Total Stock Remaining :
Tire = 96
Engine = 98

----------------------------------------
Order 2 :
Tire = 6
Engine = 3

Supplied :
Tire = 6
Engine = 3

Total Stock Remaining :
Tire = 90
Engine = 95

[ALERT]: LOW STOCK! Reorder required

