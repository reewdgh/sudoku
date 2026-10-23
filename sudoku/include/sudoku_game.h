#ifndef SUDOKU_GAME_H
#define SUDOKU_GAME_H

#include "../src/config.h"

int solveSudoku(uint8_t (*grid)[SUDOKU_DIMENSIONS], int row, int col);
int generateValidSudoku(uint8_t (*grid)[SUDOKU_DIMENSIONS], int row, int col);
void fillWithZero(uint8_t (*grid1)[SUDOKU_DIMENSIONS], uint8_t (*grid2)[SUDOKU_DIMENSIONS], int level);
#endif