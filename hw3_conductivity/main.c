#include <stdio.h>
#include <stdlib.h>
#include "assertions.h"
#include "constants.h"
#include "structures.h"
#include "workers.h"
#include <string.h>
#include <limits.h>
#include <math.h>


int howManyRowsForMe(int myrank, int threadsNumber);

int alloc_(double*** piece, int rows, int columns);
int dealloc_(double*** piece, int rows);

int initPiece(double** piece, Params params, Temps temps);
int updateMyPiece(double*** piece, int rows, int columns, int myrank);
int updateTopPiece(double*** piece, int params[3]);
int updateBottomPiece(double*** piece, int params[3]);
int updateMiddlePiece(double*** piece, int params[3]);

int bottomShareValues(double*** piece, int params[3]);
int topShareValues(double*** piece, int params[3]);
int middleShareValues(double*** piece, int params[3]);

int topUpdateValues(double*** piece, int params[3]);
int middleUpdateValues(double*** piece, int params[3]);
int bottomUpdateValues(double*** piece, int params[3]);

double calcBorderNewValue(double oldT, double borderT, double notBorderT, int is_dQ);
double calcNewValue(double oldT, double neigh1T, double neigh2T);


int howManyRowsForMe(int myrank, int threadsNumber) {
	int rowsForThread = ROWS_NUMBER / threadsNumber;
	int remainder = ROWS_NUMBER - rowsForThread*threadsNumber;
	return myrank < remainder ? rowsForThread + 1 : rowsForThread;
}

int alloc_(double*** piece, int rows, int columns) {
	assert(piece != NULL);
	// Main plate
	piece[0] = (double**)malloc((rows)*sizeof(double*));
	piece[1] = (double**)malloc((rows)*sizeof(double*));
	
	int i;
	for (i = 0; i < rows; ++i) {
		piece[0][i] = (double*)malloc((columns)*sizeof(double));
		piece[1][i] = (double*)malloc((columns)*sizeof(double));
	}
	
	return 0;
}

int dealloc_(double*** piece, int rows) {
	// Dealloc main&aux plates
	int i;
	for (i = 0; i < rows; ++i) {
		free(piece[0][i]);
		free(piece[1][i]);
	}
	free(piece[0]);
	free(piece[1]);
	
	return 0;
}

int initPiece(double** piece, Params params, Temps temps) {
	int i,j;
	
	for (i = 0; i < params.rows + 2; ++i) {
		for (j = 0; j < params.columns + 2; ++j) {
			piece[i][j] = temps.T0;
		}
	}
	
	piece[0][0] = -1;
	piece[params.rows+1][params.columns+1] = -1;
	piece[0][params.columns+1] = -1;
	piece[params.rows+1][0] = -1;
	
	
	// Init top/bottom boundaries
	if (params.myrank == 0) {
		for (i = 1; i <= params.columns; ++i) {
			piece[0][i] = temps.Tu;
		}
	} else if (params.myrank == MAX_RANK) {
		for (i = 1; i <= params.columns; ++i) {
			piece[params.rows+1][i] = temps.Td;
		}
	} else if (params.myrank > 0 && params.myrank < MAX_RANK) {
		// do nothing
	} else {
		assert(0);
	}
	
	// Init left/right boundaries
	for (i = 1; i <= params.rows; ++i) {
		piece[i][0] = temps.Tl;
		piece[i][params.columns+1] = temps.Tr;
	}

	assert_piece_corners(params);
	
	return 0;
}

int updateMyPiece(double*** piece, int rows, int columns, int myrank) {	
	int params[3];
	params[0] = myrank;
	params[1] = rows;
	params[2] = columns;
	
	
	if (myrank > 0 && myrank < MAX_RANK) {
		updateMiddlePiece(piece, params);
	} else if (myrank == 0) {
		updateTopPiece(piece, params);
	} else if (myrank == MAX_RANK) {
		updateBottomPiece(piece, params);
	} else {
		assert(0);
	}
	return 0;
}

int updateMiddlePiece(double*** piece, int params[3]) {
	middleShareValues(piece, params);
	middleUpdateValues(piece, params);
	return 0;
}
int updateTopPiece(double*** piece, int params[3]) {
	topShareValues(piece, params);
	topUpdateValues(piece, params);
	return 0;
}
int updateBottomPiece(double*** piece, int params[3]) {
	bottomShareValues(piece, params);
	bottomUpdateValues(piece, params);
	return 0;
}

int topShareValues(double*** piece, int params[3]) {
	int myrank = params[0], 
		rows = params[1], 
		columns = params[2];
	double** oldPiece = piece[0];
	
	MPI_Status Status;
	//MPI_Send(&oldPiece[rows][1], columns, MPI_DOUBLE, (myrank+1)%(MAX_RANK+1), 1, MPI_COMM_WORLD);
	
	// Receiving needed data
	//MPI_Recv(&oldPiece[rows+1][1], columns, MPI_DOUBLE, (myrank+1)%(MAX_RANK+1), 1, MPI_COMM_WORLD, &Status);
	
	MPI_Sendrecv(&oldPiece[rows][1], columns, MPI_DOUBLE, (myrank+1)%(MAX_RANK+1), 1,
					&oldPiece[rows+1][1], columns, MPI_DOUBLE, (myrank+1)%(MAX_RANK+1), 1, MPI_COMM_WORLD, &Status);
				
	return 0;
}
int middleShareValues(double*** piece, int params[3]) {
	int myrank = params[0], 
		rows = params[1], 
		columns = params[2];
	double** oldPiece = piece[0];
	
	MPI_Status Status;
	MPI_Sendrecv(&oldPiece[1][1], columns, MPI_DOUBLE, myrank-1, 1,
				&oldPiece[0][1], columns, MPI_DOUBLE, myrank-1, 1, MPI_COMM_WORLD, &Status);
	
	MPI_Sendrecv(&oldPiece[rows][1], columns, MPI_DOUBLE, myrank+1, 1,
				&oldPiece[rows+1][1], columns, MPI_DOUBLE, myrank+1, 1, MPI_COMM_WORLD, &Status);
				
	//MPI_Send(&oldPiece[1][1], columns, MPI_DOUBLE, myrank-1, 1, MPI_COMM_WORLD);
	//MPI_Send(&oldPiece[rows][1], columns, MPI_DOUBLE, myrank+1, 1, MPI_COMM_WORLD);
	
	// Receiving needed data
	//MPI_Recv(&oldPiece[0][1], columns, MPI_DOUBLE, myrank-1, 1, MPI_COMM_WORLD, &Status);
	//MPI_Recv(&oldPiece[rows+1][1], columns, MPI_DOUBLE, myrank+1, 1, MPI_COMM_WORLD, &Status);
	return 0;
}
int bottomShareValues(double*** piece, int params[3]) {
	int myrank = params[0],
		columns = params[2];
	double** oldPiece = piece[0];
	
	MPI_Status Status;
	MPI_Sendrecv(&oldPiece[1][1], columns, MPI_DOUBLE, myrank-1, 1,
					&oldPiece[0][1], columns, MPI_DOUBLE, myrank-1, 1, MPI_COMM_WORLD, &Status);
	
	//MPI_Send(&oldPiece[1][1], columns, MPI_DOUBLE, myrank-1, 1, MPI_COMM_WORLD);
	
	// Receiving needed data
	//MPI_Recv(&oldPiece[0][1], columns, MPI_DOUBLE, myrank-1, 1, MPI_COMM_WORLD, &Status);
	return 0;
}

double calcBorderNewValue(double oldT, double borderT, double notBorderT, bool is_dQ) {
	if (is_dQ == 1) {
		return (oldT + coef*(borderT - 2*oldT + notBorderT));
	} else {
		return (oldT + coef*(notBorderT - oldT));
	}
}
double calcTwoBordersNewValue(double oldT, double border1, double border2, bool is_dQ1, bool is_dQ2) {
	double res = oldT;
	if (is_dQ1 == 1) {
		res += coef*(border1 - oldT);
	}
	if (is_dQ2) {
		res += coef*(border2 - oldT);
	}
	return res;
}
double calcNewValue(double oldT, double neigh1T, double neigh2T) {
	return oldT + coef*(neigh1T - 2*oldT + neigh2T);
}

int topUpdateValues(double*** piece, int params[3]) {
	int rows = params[1],
		columns = params[2],
		i, j;
	double** old = piece[0];
	double** med/*mediate*/ = piece[1];
	
	for (i = 1; i <= rows; i++) {
		for (j = 1; j <= columns; ++j) {
			if (i == 1) {
				med[i][j] = calcBorderNewValue(old[i][j], old[i-1][j], old[i+1][j], dQtop);
			} else {
				med[i][j] = calcNewValue(old[i][j], old[i+1][j], old[i-1][j]);
			} 
			/*********/
			if (abs(med[i][j] - 0.58) < 1) {
				printf("HERE IS ERROR, RANK == %d\n", params[0]);
			}
			/*********/
		}
	}
	
	for (i = 1; i <= rows; i++) {
		for (j = 1; j <= columns; ++j) {
			if (j == 1) {
				old[i][j] = calcBorderNewValue(med[i][j], med[i][j-1], med[i][j+1], dQleft);
			} else if (j == columns) {
				old[i][j] = calcBorderNewValue(med[i][j], med[i][j+1], med[i][j-1], dQright);
			} else {
				old[i][j] = calcNewValue(med[i][j], med[i][j+1], med[i][j-1]);
			} 
			/*********/
			if (abs(old[i][j] - 0.58) < 1) {
				printf("HERE IS ERROR, RANK == %d\n", params[0]);
			}
			/*********/
		}
	}

	return 0;
}
int middleUpdateValues(double*** piece, int params[3]) {
	int rows = params[1],
		columns = params[2],
		i, j;
	double** old = piece[0];
	double** med/*mediate*/ = piece[1];
	
	for (i = 1; i <= rows; i++) {
		for (j = 1; j <= columns; ++j) {
			med[i][j] = calcNewValue(old[i][j], old[i+1][j], old[i-1][j]);
		}
	}
	for (i = 1; i <= rows; i++) {
		for (j = 1; j <= columns; ++j) {
			if (j == 1) {
				old[i][j] = calcBorderNewValue(med[i][j], med[i][j-1], med[i][j+1], dQleft);
			} else if (j == columns) {
				old[i][j] = calcBorderNewValue(med[i][j], med[i][j+1], med[i][j-1], dQright);
			} else {
				old[i][j] = calcNewValue(med[i][j], med[i][j+1], med[i][j-1]);
			} 
		}
	}
	return 0;
}
int bottomUpdateValues(double*** piece, int params[3]) {
	int rows = params[1],
		columns = params[2],
		i, j;
	double** old = piece[0];
	double** med/*mediate*/ = piece[1];
	
	for (i = 1; i <= rows; i++) {
		for (j = 1; j <= columns; ++j) {
			if (i == rows) {
				med[i][j] = calcBorderNewValue(old[i][j], old[i+1][j], old[i-1][j], dQbottom);
			} else {
				med[i][j] = calcNewValue(old[i][j], old[i+1][j], old[i-1][j]);
			} 
		}
	}
	for (i = 1; i <= rows; i++) {
		for (j = 1; j <= columns; ++j) {
			if (j == 1) {
				old[i][j] = calcBorderNewValue(med[i][j], med[i][j-1], med[i][j+1], dQleft);
			} else if (j == columns) {
				old[i][j] = calcBorderNewValue(med[i][j], med[i][j+1], med[i][j-1], dQright);
			} else {
				old[i][j] = calcNewValue(med[i][j], med[i][j+1], med[i][j-1]);
			} 
		}
	}
	return 0;
}

int printPiece(double** piece, int rows, int columns, int myrank) {
	char buf[32768]; buf[0] = '\0';
	char temp[1024]; temp[0] = '\0';
//	sprintf(temp, "(RANK): (%d)\n", myrank);
	strcat(buf, temp);
	
	int i,j;
	for (i = 0; i <= rows+1; ++i) {
		for (j = 0; j <= columns+1; ++j) {
			sprintf(temp, "%lf ", piece[i][j]);
			strcat(buf, temp);
		}
		sprintf(temp,"\n");
		strcat(buf, temp);
	}
	sprintf(temp, "\n");
	strcat(buf, temp);
	
	printf("%s", buf);
	fflush(stdout);
	return 0;
}

int allocWholeTable_(Table* params) {
	int i;
	params->table = (double**)malloc(params->rows*sizeof(double*));
	assert(params->table != NULL);
	for (i = 0; i < params->rows; ++i) {
		params->table[i] = (double*)malloc(params->columns*sizeof(double));
		assert(params->table[i] != NULL);
	}
	return 0;
}
int deallocWholeTable_(Table* tableParams) {
	assert(tableParams->table != NULL);
	int i;
	for (i = 0; i < (tableParams->rows); ++i) {
		assert(tableParams->table[i] != NULL);
		free(tableParams->table[i]);
	}
	free(tableParams->table);
	return 0;
}

int collectTable(Params params, Table tableParams) {
	double** piece = params.piece[0];
	int i;
	MPI_Send(&params.rows, 1, MPI_INT, 0, INT_MAX, MPI_COMM_WORLD); // send size;
	for (i = 1; i <= params.rows; ++i) {
		MPI_Send(&piece[i][1], params.columns, MPI_DOUBLE, 0, i, MPI_COMM_WORLD);
	}
	if (params.myrank == 0) {
		// then collecting
		int rows, currentRow = 1, i, j;
		MPI_Status status;
		for (i = 0; i <= MAX_RANK; ++i) {
			MPI_Recv(&rows, 1, MPI_INT, i, INT_MAX, MPI_COMM_WORLD, &status);
			for (j = 1; j <= rows; ++j) {
				MPI_Recv(&tableParams.table[currentRow++][1], params.columns, MPI_DOUBLE, i, j, MPI_COMM_WORLD, &status);
				assert(status.MPI_TAG == j);
				assert(status.MPI_SOURCE == i);
			}
		}
		return 0;
	} else if (params.myrank < 0 || params.myrank > MAX_RANK) {
		// can not be
		// something went wrong
		assert(0);
	}
	return 0;
}

int printWholeTable(Params params, Table tableParams) {
	if (params.myrank != 0) {
		return -1;
	}
	printf("Whole Temperatute table: \n");
	double** table = tableParams.table;
	assert(tableParams.table != NULL);
	
	int i,j;
	for (i = 1; i <= tableParams.rows-2; ++i) {
		assert(table[i] != NULL);
		for (j = 1; j <= tableParams.columns-2; ++j) {
			printf("%lf ", table[i][j]);
		}
		printf("\n");
	}
	printf("\n\n");
	return 0;
}

int printResult(Table tableParams) {
	int i;
	double currentOffset = 0; 
	// temporary
	tableParams.table[0][1] = Uu;
	tableParams.table[tableParams.rows-1][1] = Ud;
	for (i = 0; i <= tableParams.rows-1; ++i) {
		printf("%lf\t%lf\n", currentOffset, tableParams.table[i][1]);
		currentOffset += dh;
	}
	return 0;
}

// returns time
double runTest(int myrank, Params params, Table tableParams, double*** myPiece, int rowsForMe, int columns) {
	double time, begin, end = 0;
	begin = MPI_Wtime();
	
	double currentTime = 0; int iter = 0;
	while (currentTime < UP_TIME) {
		currentTime += dt;
		//assert_piece(params);
		//collectTable(params, tableParams);
		
		if (myrank == 0) {
			//printWholeTable(params, tableParams);
		}
		
		//printf("(RANK, ITER) : (%d, %d)\n", myrank, iter);
		//printPiece(myPiece[0], rowsForMe, COLUMNS_NUMBER, myrank);
		
		//printf("(iter, rank) (%d, %d) passed!\n", iter++, myrank);
		if (iter == 2 || iter == 3) {
			//printf("(RANK, ITER) : (%d, %d)\n", myrank, iter);
			//printPiece(myPiece[0], rowsForMe, COLUMNS_NUMBER, myrank);
		}
		
		updateMyPiece(myPiece, rowsForMe, columns, myrank);
		if (iter++ == 4) {
			//break;
		}
		
	}
	
	end = MPI_Wtime();
	time = end-begin;
	return time;
}

int main(int argc, char** argv) {
	int myrank;
	init(&argc, &argv, &PROCESSES_NUMBER, &myrank);
	initConstants();
	
	if (myrank == 0) {
		//printConst();
	}
	
	int rowsForMe = howManyRowsForMe(myrank, PROCESSES_NUMBER);
	//assert(rowsForMe > 1);
	
	double** myPiece[2]; // myPiece[0] - main, myPiece[1] - auxiliary;
	Table tableParams = {ROWS_NUMBER + 2, COLUMNS_NUMBER + 2, NULL};
	alloc_(myPiece, rowsForMe + 2, COLUMNS_NUMBER + 2);
	allocWholeTable_(&tableParams);
	
	Temps temps = {U0, Ul, Ur, Ud, Uu};		
	Params params = {myrank, COLUMNS_NUMBER, rowsForMe, myPiece};
	
	initPiece(myPiece[0], params, temps);
	initPiece(myPiece[1], params, temps);	
	
	double time = runTest(myrank, params, tableParams, myPiece, rowsForMe, COLUMNS_NUMBER);
	
	
	//printPiece(myPiece[0], rowsForMe, COLUMNS_NUMBER, myrank);
	
	if (myrank == 0) {
		//printWholeTable(params, tableParams);
		//printf("\n");
		//printColumn(tableParams);
		//printResult(tableParams);
		//printf("Work Time == %lf\n\n\n", time);
		printf("%d\t%lf\n", MAX_RANK+1, time);
	}
	
	
	deallocWholeTable_(&tableParams);
	dealloc_(myPiece, rowsForMe + 2);
	
	finalize();

	return 0;
}

