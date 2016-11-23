#ifndef WORKERS_H
#define WORKERS_H

#include <mpi.h>

int init(int* argc, char*** argv, int* size, int* myrank) {
	MPI_Init(argc, argv);
	MPI_Comm_size(MPI_COMM_WORLD, size); // size equals to number of processes
	MPI_Comm_rank(MPI_COMM_WORLD, myrank); // Unique number for each process
	return 0;
}

int finalize() {
	MPI_Finalize();
	return 0;
}


#endif // WORKERS_H
