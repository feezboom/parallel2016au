#ifndef TEST_H_
#define TEST_H_

#include <time.h>
#include "functions.h"

struct timespec begin, end;
double elapsed;


int testPerformance() {
    sem_init(&sem, 0, 1);
    int THREADS_NUMBER;
    const int MAX_THREADS = 20;
    const int MAX_POINTS = 1000000000;
    pthread_t threads[MAX_THREADS];

    POINTS_AT_ALL;
    for (POINTS_AT_ALL = 1000; POINTS_AT_ALL <= MAX_POINTS; POINTS_AT_ALL*=10) {
        printf("\nPOINTS AT ALL == %d\n", POINTS_AT_ALL);
        for (THREADS_NUMBER = 1; THREADS_NUMBER <= MAX_THREADS; ++THREADS_NUMBER) {
            goodPoints = 0, result = 0;

            int pointsToGen = POINTS_AT_ALL / THREADS_NUMBER;
            clock_gettime(CLOCK_REALTIME, &begin);
            initThreads(threads, THREADS_NUMBER, &pointsToGen);

            double **returnedValues = (double **) joinThreads(threads, THREADS_NUMBER);
            clock_gettime(CLOCK_REALTIME, &end);

            elapsed = end.tv_sec - begin.tv_sec; // время в секундах
            elapsed += (end.tv_nsec - begin.tv_nsec) / 1000000000.0;

            double localResult = 0;
            int localGoodPoints = 0;
            for (int i = 0; i < THREADS_NUMBER; ++i) {
                localGoodPoints += (int) returnedValues[i][0];
                localResult += returnedValues[i][1];
                free(returnedValues[i]);
            }
 	    free(returnedValues);

            printf("%d\t%lf\t%f\n", THREADS_NUMBER, (result * 2.) / goodPoints, elapsed);
        }
    }
}

#endif // TEST_H_
