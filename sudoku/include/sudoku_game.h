#pragma once
#include <stdlib.h>
#include <stdbool.h>
int *shuffle();
int checkValidPlacement(int (*grid)[9], int num, int row, int col);
int solveSudoku(int (*grid)[9], int row, int col);
int copyMatrix(int (*grid1)[9], int (*grid2)[9]);
int countSolutions(int (*grid)[9]);
int checkRows(int (*grid)[9], int number, int row, int col);
int checkCols(int (*grid)[9], int number, int row, int col);
int checkSubGrid(int (*grid)[9], int number, int row, int col);
int generateValidSudoku(int (*grid)[9], int row, int col);
void fillWithZero(int (*grid1)[9], int (*grid2)[9], int level);
