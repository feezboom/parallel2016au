#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

typedef struct Table {
    int** first;
    int** second;
    int rows;
    int columns;
} Table;

#define bool int
#define true 1
#define false 0

#define fail_if(condition, message) if(condition) {fprintf(stderr, message); exit(EXIT_FAILURE);}

void printState(FILE* stream, Table* table) {
    int i, j;
    for (i = 0; i < table->rows; ++i) for (j = 0; j < table->columns; ++j) {
            if (table->first[i][j] == 1)
                fprintf(stream, "%d %d\n", i, j);
    }
}

void alloc_table(Table* table) {
    table->first = (int**)malloc((table->rows)*sizeof(int*));
    table->second = (int**)malloc((table->rows)*sizeof(int*));

    fail_if(table->first == NULL, "malloc problem\n");
    fail_if(table->second == NULL, "malloc problem\n");

#define array_set(ar,i,n,v) for(i = 0; i < n; ++i) (ar)[i] = v;
    int i,j;
    for(i = 0; i < table->rows; ++i) {
        table->first[i] = ((int*)malloc((table->columns)*sizeof(int)));
        table->second[i] = ((int*)malloc((table->columns)*sizeof(int)));

        fail_if(table->first[i] == NULL, "malloc problem\n");
        fail_if(table->second[i] == NULL, "malloc problem\n");

        array_set(table->first[i],j,table->columns,0);
        array_set(table->second[i],j,table->columns,0);
    }
    // move
    for(i = 0; i < table->rows; ++i) {
        table->first[i]++;
        table->second[i]++;
    }
}

void swap_tables(Table* table) {
    int** temp = table->first;
    table->first = table->second;
    table->second = temp;
}

void count_cells(Table* t, int* count, int i, int j, bool isTorus) {
    int upi = i-1, downi = i+1, leftj = j-1, rightj = j+1;
    if (isTorus) {
        leftj = (j == 0) ? t->columns-1 : j-1;
        rightj = (j == t->columns-1) ? 0 : j+1;
        upi = (i == 0) ? t->rows-1 : i-1;
        downi = (i == t->rows-1) ? 0 : i+1;
    }

    if (upi >= 0 && upi < t->rows) /*->*/ count[t->first[upi][j]]++;
    if (downi >= 0 && downi < t->rows) /*->*/ count[t->first[downi][j]]++;

    if (upi >= 0 && upi < t->rows && leftj >= 0) /*->*/ count[t->first[upi][leftj]]++;
    if (downi >= 0 && downi < t->rows && leftj >= 0) /*->*/ count[t->first[downi][leftj]]++;
    if (leftj >= 0) /*->*/ count[t->first[i][leftj]]++;

    if (upi >= 0 && upi < t->rows && rightj < t->columns) /*->*/ count[t->first[upi][rightj]]++;
    if (downi >= 0 && downi < t->rows && rightj < t->columns) /*->*/ count[t->first[downi][rightj]]++;
    if (rightj < t->columns) /*->*/ count[t->first[i][rightj]]++;
}

int new_cell_value(int state, int* count) {
    if (state == 0) {
        if (count[1] == 3) return 1;
        else return 0;
    } else if (state == 1) {
        if (count[1] == 2 || count[1] == 3) return 1;
        else return 0;
    } else fail_if(false, "not possible cell value\n");
    return -1;
}

int life_iter(Table* table, bool isTorus) {

    int i, j;
	#pragma omp parallel for collapse(2)
    for(i = 0; i < table->rows; ++i) {
        for(j = 0; j < table->columns; ++j) {
            int count[2] = {0,0};
            count_cells(table, count, i, j, isTorus);
            table->second[i][j] = new_cell_value(table->first[i][j], count);
        }
    }

    swap_tables(table);
    return 0;
}

int main(int argc, char** argv) {
    FILE* in = fopen("state.dat", "r");
    FILE* out = fopen("life.dat", "w");

    fail_if(!in, "file state.dat wasn't found\n");
    fail_if(!out, "file life.dat wasn't created\n");

    int j=-1, x, y; Table table;
    fscanf(in, "%d %d", &table.rows, &table.columns);
    fail_if(table.rows <= 0 || table.columns <= 0, "wrong initial sizes\n");
    alloc_table(&table);

    while(fscanf(in, "%d %d", &x, &y) != EOF) { // init table
        fail_if(x < 0 || x >= table.rows || y < 0 || y >= table.columns, "wrong input given in state.dat\n");
        table.first[x][y] = 1;
    }

    fail_if(argc < 2, "at least 1 argument needed\n");
#define ITER atoi(argv[1])
    omp_set_num_threads(1);

    for(j = 0; j < ITER; ++j) {
        life_iter(&table, true);
    }

    printState(out, &table);
    return 0;
}
