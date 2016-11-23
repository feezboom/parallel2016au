#ifndef _STRUCTURES_H__
#define _STRUCTURES_H__

typedef struct Temps {
	int T0;
	int Tl;
	int Tr;
	int Td;
	int Tu;
} Temps;

typedef struct Params {
	int myrank;
	int columns;
	int rows;
	double*** piece;
} Params;

typedef struct Table {
	int rows;
	int columns;
	double** table;
} Table;

typedef struct Piece {
	double** main;
	double** med;
} Piece;

#endif // _STRUCTURES_H__
