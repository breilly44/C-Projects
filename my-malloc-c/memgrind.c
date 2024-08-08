#include "mymalloc.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


void test1() {
    
    struct timeval start, end;
    double totalTime = 0, avgTime, elapsedTime;
    int iterations = 50;

    for (int i = 0; i < iterations; i++) {
        gettimeofday(&start, NULL);
        for (int j = 0; j < 120; j++) {
            void* ptr = malloc(1);
            free(ptr);
        }
        gettimeofday(&end, NULL);
        elapsedTime = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
        totalTime += elapsedTime;
    }
    avgTime = totalTime / iterations;
    printf("Average time for test 1: %f seconds\n", avgTime);

}



void test2() {

    void* ptrArray[120];
    struct timeval start, end;
    double totalTime = 0, avgTime, elapsedTime;
    int iterations = 50;

    for (int i = 0; i < iterations; i++) {
        gettimeofday(&start, NULL);
        for (int j = 0; j < 120; j++) {
            ptrArray[j] = malloc(1);
        }
        for (int j = 0; j < 120; j++) {
            free(ptrArray[j]);
        }
        gettimeofday(&end, NULL);
        elapsedTime = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
        totalTime += elapsedTime;
    }
    avgTime = totalTime / iterations;
    printf("Average time for test 2: %f seconds\n", avgTime);

}


void test3() {

    void* ptrArray[120] = {NULL};
    struct timeval start, end;
    double totalTime = 0, avgTime, elapsedTime;
    int iterations = 50, allocated = 0;

    srand(time(NULL));

    for (int i = 0; i < iterations; i++) {
        allocated = 0;
        gettimeofday(&start, NULL);
        while (allocated < 120) {
            int action = rand() % 2;
            if (action == 0 || allocated == 0) {
                ptrArray[allocated++] = malloc(1);
            } else {
                free(ptrArray[--allocated]);
            }
        }
        for (int j = 0; j < 120; j++) {
            free(ptrArray[j]);
        }
        gettimeofday(&end, NULL);
        elapsedTime = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
        totalTime += elapsedTime;
    }
    avgTime = totalTime / iterations;
    printf("Average time for test 3: %f seconds\n", avgTime);

}



int main() {

    srand(time(NULL));

    test1();
    test2();
    test3();

    return 0;

}

