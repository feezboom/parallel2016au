#ifndef CONDUCTIVITY_H
#define CONDUCTIVITY_H

#include <mpi.h>
#include <assert.h>

int WORKERS_NUMBER = 10;


int update_piece(double* pValue);

int update_my_pieces(int dT, int i, int j, double** data);

int shareDistrubuteInfo(int myrank, int size, MPI_Status* pStatus, int pair[2]) {
		// Sending distribution information
	if (myrank == 0) {
		// Distributing plate:
		//dh = (Lx-0)/PARTS_NUMBER;
		unsigned long columnsForThread = PARTS_NUMBER/size;
		unsigned long remainder_ = PARTS_NUMBER - columnsForThread*size;
		
		// Firstly sending to each thread to calculate from
		int i, currentPosition = 0;
		for (i = 1; i < size; ++i) {
			int number = columnsForThread; // Columns for each thread
			if (remainder_ > 0) {
				number++;
				remainder_--;
			} 
			
			// Send position and number;
			pair[0] = currentPosition;
			pair[1] = number;
			MPI_Send(pair, 2, MPI_INT, i, i, MPI_COMM_WORLD);
			
			// Update current position
			currentPosition += number;
		}
		pair[0] = currentPosition;
		pair[1] = columnsForThread;
	} else /*receiving distribution information*/ {
		MPI_Recv(pair, 2, MPI_INT, 0, myrank, MPI_COMM_WORLD, pStatus);
	}
	
	// assert(dT < (dh*dh)/k);
	return 0;
} 

double** allocMemory_(int rows, int columns) {
	double** pointer = (double**)malloc(rows*sizeof(double*));
	int i; 
	for (i = 0; i < columns; ++i) {
		pointer[i] = (double*)malloc(columns*sizeof(double));
	}
	return pointer;
}

double run(int myrank, int size) {	
	double begin, end, workTime;
	MPI_Status status;
	
	begin = MPI_Wtime();

	
	
	int allocateParams[2];
	// params[0] will have start pos in table
	// params[1] will have number of rows in table
	shareDistrubuteInfo(myrank, size, &status, allocateParams);
	double** platePiece = platePiece = allocMemory_(1,1);
	
	
	
	


	end = MPI_Wtime();
	workTime = end - begin;
	return workTime;
}

#endif // CONDUCTIVITY_H



