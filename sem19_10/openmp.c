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
	
	omp_set_num_threads(4);
	
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
		#pragma omp for 
		for (i = 0; i < 10; ++i) {
			a[i] = a[i] * 2;
			printf("Thread %d has multiplied element %d\n", myid, i);
		}
	}
	myid = omp_get_thread_num();
	printf("consecutive part, myid = %d\n", myid);


	// Shared variables :
	// reduction (operator: var list)
	// Other operations are available :
	// +, -, *, ^, &, |, &&, ||, max, min.
	
	int b = 3;
	#pragma omp parallel reduction(+:b)
	{
		b = b * 7;
		printf("Current value, b = %d\n", b);
	}
	
	printf("Number of thread = %d\n", b);


	// #pragma barrier 
	// #pragma ...

	//double begin, end, total;
	//begin = omp_get_wtime();

	
	return 0;
}
 
