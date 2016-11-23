#ifndef _ASSERTIONS_H__
#define _ASSERTIONS_H__

#include <assert.h>
#include "structures.h"

void assert_piece_corners(struct Params params) {
	assert(params.piece[0][0][0] == -1);
	assert(params.piece[0][params.rows+1][params.columns+1] == -1);
	assert(params.piece[0][0][params.columns+1] == -1);
	assert(params.piece[0][params.rows+1][0] == -1);
}

int assert_piece(struct Params params) {
	int i, j;
	for (i = 0; i < params.rows; ++i) {
		for (j = 0; j < params.columns; ++j) {
			assert(params.piece[0][i][j] >= -1 && params.piece[0][i][j] <= 80);
			assert_piece_corners(params);
		}
	}
	return 0;
}

#endif // _ASSERTIONS_H__
