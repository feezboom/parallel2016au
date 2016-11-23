#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

static int* borderRow;
static int** ttable;


int _i_,_j_;
#define ITER 10000
#define STAR "\x1B[33m* \x1B[0m"
#define bool int
#define forn(i, n) for (i = 0; i < (int)(n); ++i)
#define forfor(i,j,n,k) forn(i,n) forn(j,k)
#define fail_if(condition, message) if(condition) {fprintf(stderr, message); exit(EXIT_FAILURE);}
#define init_table(in,table,a,b) forn(i,a) { forn(j,b) { fscanf(in, "%d", &table[i][j]); } }
#define print_table(stream,table,a,b) { forn(_i_,a) { fprintf(stream,"|"); forn(_j_,b) { fprintf(stream,"%s", (table[_i_][_j_] == 1) ? STAR : "  ");} fprintf(stream,"|\n");} }
#define update_cell(t,nt,i,j) { nt[i][j] = ( (t[i][j]==1) ? ((count[1] == 3) ? 1 : 2) : ((count[1] == 2 || count[1] == 3) ? 1 : 2) );}
#define copy_table(t,nt,a,b) forfor(_i_,_j_,a,b) { t[_i_][_j_] = nt[_i_][_j_]; }

#define print_normal(t,i,j,a,b) forn(i,a){ forn(j,b){printf("%d ", t[i][j]);} printf("\n");}
#define print_row(r,n,i) {forn(i,n) {printf("%d ", r[i]);} printf("\n");}
#define set_row(r,n,v,i) forn(i,n) {r[i] = v;}

#define assert_table(table,i,j,a,b) { forfor(i,j,rows,columns) {fail_if(table[i][j] != 1 && table[i][j] != 2, "something went wrong\n");}}

// 0 - fake, not considered
// 1 - alive
// 2 - dead
void count_cells(int** t, int*count, int i, int j) {
	count[t[i-1][j-1]]++;
	count[t[i-1][j]]++;
	count[t[i-1][j+1]]++;
	count[t[i+1][j-1]]++;
	count[t[i+1][j]]++;
	count[t[i+1][j+1]]++;
	count[t[i][j-1]]++;
	count[t[i][j+1]]++;
}

int** alloc_table(int a, int b) {
	int** table = ((int**)malloc((a+2)*sizeof(int*))), i;
	fail_if(table == NULL, "malloc problem\n"); table++;
	forn(i,a) {
		table[i] = ((int*)malloc((b+2)*sizeof(int)));
		fail_if(table[i] == NULL, "malloc problem\n"); table[i]++;
	}
	return table;
}

void printState(FILE* stream, int** table, int rows, int columns) {
	int i,j;
	forfor(i,j,rows,columns) {
		if (table[i][j] == 1) {
			fprintf(stream, "%d %d\n", i, j);
		}
	}
}

int life_iteration(int** table, int rows, int columns, bool isTorus) {
	int k,l;
	int count[3] = {0,0,0};

	omp_set_num_threads(1);

	print_normal(table,k,l,rows,columns);
	printf("\n");

	table[-1] = isTorus ? table[columns-1] : borderRow;
	table[columns] = isTorus ? table[0] : borderRow;
	
	print_normal(table,k,l,rows,columns);
	
	
	#define reset_count(count) {count[0] = 0; count[1] = 0; count[2] = 0;}
	
	int i,j, iter = 0;
	
	
	#pragma omp parallel for collapse(2)
	forfor(i,j,rows,columns) {
		int k,l;
		printf("iter%d\n", iter++);
		//print_normal(table,k,l,rows,columns);
		assert_table(table,k,l,rows,columns);
		reset_count(count);
		count_cells(table,count,i,j);
		update_cell(table,ttable,i,j);
	}
	copy_table(table,ttable,rows,columns);

	return 0;
}

int main(int argc, char** argv) {
	int rows = atoi(argv[1]), columns = atoi(argv[2]), i, j;
	char* inputFile = argv[3]; // File with start data
	
	FILE* in = fopen(inputFile, "r");
	FILE * out = fopen("life.dat", "w");
	
	fail_if(argc < 4, "wrong number of arguments\n");
	fail_if(rows <= 0 || columns <= 0, "wrong input\n");
	fail_if(!in, "no such input file\n");
	
	int** table = alloc_table(rows, columns);
	init_table(in,table,rows,columns);
	
	assert_table(table,i,j,rows,columns);
	
	borderRow = (int*)calloc((columns+2), sizeof(int)) + 1;
	ttable = alloc_table(rows, columns);
	
	forn(i,ITER) {
		life_iteration(table, rows, columns, false);
	}
	
	printState(out, table, rows,columns);
	fclose(out);
	return 0;
}
