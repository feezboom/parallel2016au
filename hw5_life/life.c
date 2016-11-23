#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

typedef struct Table {
	int** first;
	int** second;
	int* fictive;
	int rows;
	int columns;
} Table;

#define bool int
#define true 1
#define false 0

#define fail_if(condition, message) if(condition) {fprintf(stderr, message); exit(EXIT_FAILURE);}
#define fail_if_bad_table(table,a,b,i,j) forn(i,a) forn(j,b) fail_if()
#define forn(i, n) for (i = 0; i < (int)(n); ++i)

void printNormal(FILE* stream, Table* table) {
	int i,j;
	forn(i,table->rows) forn(j,table->columns) 
		fprintf(stream, "%d%s", table->first[i][j], j==table->columns-1 ? "\n" : " ");
}

void printState(FILE* stream, Table* table) {
	int i,j;
	forn(i,table->rows) forn(j,table->columns) 
		if (table->first[i][j] == 1)
			fprintf(stream, "%d %d\n", i, j);
	printf("[%d %d]\n", table->rows, table->columns);
}

void alloc_table(Table* table) {
	table->first = (int**)malloc((table->rows+2)*sizeof(int*));
	table->second = (int**)malloc((table->rows+2)*sizeof(int*));
	
	fail_if(table->first == NULL, "malloc problem\n");
	fail_if(table->second == NULL, "malloc problem\n");
		
	#define array_set(ar,i,n,v) forn(i,n) (ar)[i] = v;
	int i,j;
	forn(i,table->rows+2) {
		table->first[i] = ((int*)malloc((table->columns+2)*sizeof(int)));
		table->second[i] = ((int*)malloc((table->columns+2)*sizeof(int)));
		
		fail_if(table->first[i] == NULL, "malloc problem\n");
		fail_if(table->second[i] == NULL, "malloc problem\n");

		array_set(table->first[i],j,table->columns+2,0);
		array_set(table->second[i],j,table->columns+2,0);
		// Borders
		table->first[i][0] = 2; table->first[i][table->columns + 1] = 2;
		table->second[i][0] = 2; table->second[i][table->columns + 1] = 2;
	}
	// Fictive
	table->fictive = (int*)malloc((table->columns+2)*sizeof(int));
	fail_if(table->fictive == NULL, "with *fictive malloc problem\n"); 
	array_set(table->fictive,i,table->columns + 2,2);
	// Borders first
	array_set(table->first[0], i, table->columns+2, 2);
	array_set(table->first[table-> columns+1], i, table->columns + 2, 2);
	// Borders second
	array_set(table->second[0], i, table->columns+2, 2);
	array_set(table->second[table->rows+1], i, table->columns + 2, 2);
	// move
	forn(i, table->rows+2) {
		table->first[i]++;
		table->second[i]++;
	}
	table->first++;
	table->second++;
	table->fictive++;
}

void count_cells(Table* t, int* count, int i, int j) {
	count[t->first[i-1][j-1]]++;
	count[t->first[i-1][j]]++;
	count[t->first[i-1][j+1]]++;
	count[t->first[i+1][j-1]]++;
	count[t->first[i+1][j]]++;
	count[t->first[i+1][j+1]]++;
	count[t->first[i][j-1]]++;
	count[t->first[i][j+1]]++;
}

int new_cell_value(int state, int count[3]) {
	if (state == 0) {
		if (count[1] == 3) return 1;
		else return 0;
	} else if (state == 1) {
		if (count[1] == 2 || count[2] == 3) return 1;
		else return 0;
	} else fail_if(false, "not possible cell value\n");
	return -1;
}

void from_second_to_first(Table* table) {
	int i,j;
	forn(i,table->rows) forn(j, table->columns) {
		table->first[i][j] = table->second[i][j];
		fail_if(table->first[i][j] < 0 || table->first[i][j] > 2, "wrong table values (update_table)\n");
	}
}

int life_iter(Table* table, bool isTorus) {
	table->first[-1] = isTorus ? table->first[table->columns-1] : table->fictive;
	table->first[table->columns] = isTorus ? table->first[0] : table->fictive;
	
	int i, j;
	#pragma omp parallel for collapse(2)
	forn(i,table->rows) forn(j,table->columns) {
		int count[3] = {0,0,0};
		count_cells(table, count, i, j);
		table->second[i][j] = new_cell_value(table->first[i][j], count);
	}
	
	from_second_to_first(table);
	
	return 0;
}

int main(int argc, char** argv) {
	FILE* in = fopen("state.dat", "r");
	FILE* out = fopen("life.dat", "w");
	fail_if(!in, "file state.dat wasn't found\n");
	fail_if(!out, "file life.dat wasn't created\n");
	
	
	int i=-1, j=-1, x, y; Table table;
	fscanf(in, "%d%d", &table.rows, &table.columns);
	fail_if(table.rows <= 0 || table.columns <= 0, "wrong initial sizes\n");
	alloc_table(&table);
	
	while(fscanf(in, "%d %d", &x, &y) != EOF) { // init table
		fail_if(x < 0 || x >= table.rows || y < 0 || y >= table.columns, "wrong input given in state.dat\n");
		table.first[x][y] = 1;
	}
	
	#define MAX_THREADS 20
	#define ITER 100000
	double begin,end;
	for(i = 1; i <= MAX_THREADS; ++i) {
		omp_set_num_threads(i);
		begin = omp_get_wtime();
		forn(j,ITER) life_iter(&table, false);
		end = omp_get_wtime();
		printf("%d\t%lf\n", i, end-begin);
	}
	return 0;
}
