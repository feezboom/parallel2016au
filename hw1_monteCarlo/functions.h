#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <stdio.h>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>

sem_t sem;
static double result = 0;
static unsigned long goodPoints = 0;
static int POINTS_AT_ALL = 100000;


void initThreads(pthread_t thrds[], int THREADS_NUMBER, int* arg);
void** joinThreads(pthread_t thrds[], int THREADS_NUMBER);
void* threadRunner(void* smth);

unsigned int seedGeter;
unsigned int genSeed() {
    return rand_r(&seedGeter);
};

int checkPoint(long double x, long double y) ;
void throwPoint(double *resx, double* resy, double infx, double supx, double infy, double supy, unsigned int* mySeed) ;


// ## MONTE CARLO
void throwPoint(double* resx, double* resy, double infx, double supx, double infy, double supy, unsigned int* mySeed) {
    int x = abs(rand_r(mySeed));
    int y = abs(rand_r(mySeed));
    *resx = ((x % 1000000) / 1000000.) * (supx-infx);
    *resy = ((y % 1000000) / 1000000.) * (supy-infy);
}
int checkPoint(long double x, long double y) {
    if (y <= sin(x)) {
        return 1;
    } else {
        return 0;
    }
}
// ## MONTE CARLO


// ## THREADING
void initThreads(pthread_t thrds[], int THREADS_NUMBER, int* arg) {
    for (int i = 0; i < THREADS_NUMBER; ++i) {
        int rc = pthread_create(thrds+i, NULL, threadRunner, (void*)arg);
        if (rc) fprintf(stderr, "ERROR; return code from pthread_create() is %d \n", rc);
    }
}

void** joinThreads(pthread_t thrds[], int THREADS_NUMBER) {
    void** returnedValues = malloc(THREADS_NUMBER*(sizeof(double*)));
    for (int i = 0; i < THREADS_NUMBER; ++i) {
        int rc = pthread_join(thrds[i], (void**)(((double**)returnedValues) + i));
        if (rc) fprintf(stderr, "ERROR; return code from pthread_join() is %d \n", rc);
    }
    return returnedValues;
}

void *threadRunner(void* smth) {
    int pointsToGen = *(int*)smth;
    unsigned int mySeed = genSeed();
    int gp = 0;
    double r = 0;
    for (int i = 0; i < pointsToGen; ++i) {
        double x,y;
        throwPoint(&x, &y, 0, M_PI, 0, 1, &mySeed);
        int isBelongs = checkPoint(x, y);
        if (isBelongs) {
            gp++;
            r += x*y;
        }
    }
    sem_wait(&sem);
    goodPoints += gp;
    result += r;
    sem_post(&sem);
    // sending by pthread_exit()
    double* res = (double*)malloc(2*sizeof(double));
    res[0] = gp;
    res[1] = r;
//    printf("res from thread : gp = %d r = %f\n", gp, r);
    pthread_exit(res);
    return NULL;
}
// ## THREADING


#endif
