#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define N 1001

int main(int argc, char** argv) {

	int i, myrank, size;
	MPI_Status status;
	MPI_Init(&argc, &argv);
	
	setbuf(stdout, 0);
	
	MPI_Comm_size(MPI_COMM_WORLD, &size); // Processes number
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);	

	int buf[N];

	if (myrank == 0) {
		// here the place to send elements and then to receive
		// partly-calculated sums 
		for (i = 0; i < N; ++i) {
			buf[i] = i;
		}
		for (i = 1; i < size; ++i) {
			// for each process
			 MPI_Send(buf+((i-1)*(N/size)), N/size, MPI_INT, i, i, MPI_COMM_WORLD);
		}
		unsigned long long sum = 0; ///// ###### SUM
		for (i = (size-1)*(N/size); i < N; ++i) {
			// calculating remainder
			sum += buf[i];
			//printf("CHECK bufi= %d", buf[i]);
		}
		printf("MyRank == %d; MySum == %lld\n", myrank, sum);
		for (i = 1; i < size; ++i) {
			int temp;
			MPI_Recv(&temp, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
			sum += temp;
		}
		//sleep(1);
		printf("S==%lld\n", sum);
		unsigned long long slowSum = 0; ///// ###### SLOW SUM
		for (i = 0; i < N; ++i) {
			slowSum += buf[i];
		}
		printf("S0 == %lld\n", sum);
		assert(slowSum == sum);
	} else {
		// here is the place for other to receive elements, 
		// count sum and then send sum back to sender
		int localBuf[N/size];
		MPI_Recv(localBuf, N/size, MPI_INT, 0, myrank, MPI_COMM_WORLD, &status);
		int j, locSum = 0;
		for (j = 0; j < N/size; ++j) {
			locSum += localBuf[j];
		}
		printf("MyRank == %d; MySum == %d\n", myrank, locSum);
		fflush(stdout);
		MPI_Send(&locSum, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
	}
	
	MPI_Finalize();
	return 0;
}

