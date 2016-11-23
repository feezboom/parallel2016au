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

typedef int bool;
#define true 1
#define false 0

#define fail_if(condition, message) if(condition) {fprintf(stderr, message); exit(EXIT_FAILURE);}

void printState(FILE* stream, Table* table) {
	int i,j;
	for(i = 0; i < table->rows; ++i) for(j = 0; j < table->columns; ++j) 
		if (table->first[i][j] == 1)
			fprintf(stream, "%d %d\n", i, j);
	printf("[%d %d]\n", table->rows, table->columns);
}

void alloc_table(Table* table) {
	table->first = (int**)malloc((table->rows+2)*sizeof(int*));
	table->second = (int**)malloc((table->rows+2)*sizeof(int*));
	
	fail_if(table->first == NULL, "malloc problem\n");
	fail_if(table->second == NULL, "malloc problem\n");
		
	#define array_set(ar,i,n,v) for(i = 0; i < n; ++i) (ar)[i] = v;
	int i,j;
	for(i = 0; i < table->rows+2; ++i) {
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
	for(i = 0; i < table->rows+2; ++i) {
		table->first[i]++;
		table->second[i]++;
	}
	table->first++;
	table->second++;
	table->fictive++;
}

void swap_tables(Table* table) {
	int** temp = table->first;
	table->first = table->second;
	table->second = temp;
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

int life_iter(Table* table, bool isTorus) {
	table->first[-1] = isTorus ? table->first[table->columns-1] : table->fictive;
	table->first[table->columns] = isTorus ? table->first[0] : table->fictive;
	
	int i, j;
	#pragma omp parallel for collapse(2)
	for(i = 0; i < table->rows; ++i) for(j = 0; j < table->columns; ++j) {
		int count[3] = {0,0,0};
		count_cells(table, count, i, j);
		table->second[i][j] = new_cell_value(table->first[i][j], count);
	}
	
	swap_tables(table);
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
	#define ITER 4
	double begin,end;
	//for(i = 1; i <= MAX_THREADS; ++i) {
		omp_set_num_threads(4);
		begin = omp_get_wtime();
		for(j = 0; j < atoi(argv[1]); ++j) life_iter(&table, false);
		end = omp_get_wtime();
		printf("%d\t%lf\n", i, end-begin);
	//}
	printState(out, &table);
	return 0;
}
