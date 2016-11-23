#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int main(int argc, char** argv) {
	#ifdef _OPENMP
		printf("OpenMP is supported! %d \n", _OPENMP);
	#endif
	
	int a[10];
	int i = 0;
	int myid, num_procs, num_threads;
	
	num_procs = omp_get_num_procs();
	
	printf("Num of processors = %d \n", num_procs);
	
	omp_set_num_threads(2);
	
	num_threads = omp_get_num_threads();
	
	printf("Number of threads = %d \n", num_threads);
	
	for (i = 0; i < 10; ++i) {
		a[i] = i;
	}
	
	myid = omp_get_thread_num();
	printf("Consecutive part, myid = %d \n", myid);
	
	#pragma omp parallel shared(a) private(myid)
	{
		myid = omp_get_thread_num();
		printf("parallel part, myid = %d\n", myid);
		// place for #pragma omp for
	}
	myid = omp_get_thread_num();
	printf("consecutive part, myid = %d\n", myid);

	
	return 0;
}
 
