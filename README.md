# KSU-Operating-Systems-Assignment-1-Producer-Consumer-
Assignment 1 - Producer - Consumer

## **Program Description**
This program shows how a **Producer** and **Consumer** interact with eachother by accessing the **Semaphores** and **Shared Memory**, ensuring **Mutex**.
These components work together to prevent race conditions — semaphores control access, the mutex ensures only one process uses the buffer at a time, and shared memory lets both processes communicate through the same data space.
The **producer** will put the item in the buffer and the **consumer** will take it out the item.

## **Compile**
To compile, you will enter

`gcc producer.c -pthread -lrt -o producer`
`gcc consumer.c -pthread -lrt -o consumer`

## **Run**
After compiling, you will run 

`./producer & ./consumer &`

## **Output**
After running in the terminal, it should display

![Program Output](Screenshot%202025-10-26%20105826.png)

## **Files**
- producer.c  - puts item in buffer
- consumer.c  - takes item from buffer
- shared.c    - structure and shared resources
- Makefile    - compiles and runs programs
- README.md   - describes the use and background of entire program
