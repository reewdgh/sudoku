#include "../include/sudoku_game.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
// get the usedVals and switch 1  for the values that are used
static int getUsedVals(uint8_t (*grid)[SUDOKU_DIMENSIONS], int row, int col)
{
    int bitCollection = 0;
    // Rows
    for (int i = 0; i < SUDOKU_DIMENSIONS; i++)
    {
        if (grid[row][i] != 0)
        {
            bitCollection |= (1 << grid[row][i]);
        }
    }
    // Cols
    for (int i = 0; i < SUDOKU_DIMENSIONS; i++)
    {
        if (grid[i][col] != 0)
        {
            bitCollection |= (1 << grid[i][col]);
        }
    }
    // subgrid 
    int first = (row / 3) * 3;
    int second = (col / 3) * 3;
    for (int i = first; i < first + 3; i++)
    {
        for (int j = second; j < second + 3; j++)
        {
            if (grid[i][j] != 0)
            {
                bitCollection |= (1 << grid[i][j]);
            }
        }
    }

    return bitCollection;
}
// count solution for the generated sudoku 
static int countSolutions(uint8_t (*grid)[SUDOKU_DIMENSIONS])
{
    uint8_t solveGrid1[SUDOKU_DIMENSIONS][SUDOKU_DIMENSIONS] = {0}, solvedGrid2[SUDOKU_DIMENSIONS][SUDOKU_DIMENSIONS] = {0};
    memcpy(solveGrid1, grid, TOTAL_CELLS * sizeof(uint8_t));

    solveSudoku(solveGrid1, 0, 0);
    for (int i = 0; i < SUDOKU_DIMENSIONS; i++)
    {
        memcpy(solvedGrid2, grid, TOTAL_CELLS * sizeof(uint8_t));
        solveSudoku(solvedGrid2, 0, 0);
        for (int i = 0; i < SUDOKU_DIMENSIONS; i++)
        {
            for (int j = 0; j < SUDOKU_DIMENSIONS; j++)
            {
                if (solvedGrid2[i][j] != solveGrid1[i][j])
                {
                    return false;
                }
            }
        }
    }
    return true;
}
// shuff;e
static uint8_t *shuffle(void)
{
    static uint8_t numbers[] = NUMBER_BUTTONS_ID;
    for (int i = 0; i < SUDOKU_DIMENSIONS; i++)
    {
        int random = rand() % SUDOKU_DIMENSIONS;
        int temp = numbers[i];
        numbers[i] = numbers[random];
        numbers[random] = temp;
    }
    return numbers;
}

// fill zero while cautiously checking for one solution 
void fillWithZero(uint8_t (*grid1)[SUDOKU_DIMENSIONS], uint8_t (*grid2)[SUDOKU_DIMENSIONS], int level)
{
    memcpy(grid1, grid2, TOTAL_CELLS * sizeof(uint8_t));

    for (int i = 0; i < SUDOKU_DIMENSIONS; i++)
    {
        uint8_t *numbers = shuffle();
        for (int j = 0; j < level; j++)
        {
            grid1[i][numbers[j]] = 0;
            if (countSolutions(grid1) == false)
            {
                grid1[i][numbers[j]] = grid2[i][numbers[j]];
            }
        }
    }
}
// solve the sudoku 
int solveSudoku(uint8_t (*grid)[SUDOKU_DIMENSIONS], int row, int col)
{
    if (row == SUDOKU_DIMENSIONS)
    {
        return 1;
    }
    if (col == SUDOKU_DIMENSIONS)
    {
        return solveSudoku(grid, row + 1, 0);
    }
    if (grid[row][col] != 0)
    {
        return solveSudoku(grid, row, col + 1);
    }

    uint8_t numbers[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    for (int i = 8; i > 0; i--)
    {
        int random = rand() % (i + 1);
        int temp = numbers[i];
        numbers[i] = numbers[random];
        numbers[random] = temp;
    }
    int bit = getUsedVals(grid, row, col);
    for (int i = 0; i < SUDOKU_DIMENSIONS; i++)
    {
        if (bit & (1 << numbers[i]))
        {
            continue;
        }
        grid[row][col] = numbers[i];
        if (solveSudoku(grid, row, col + 1))
        {
            return 1;
        }
        grid[row][col] = 0;
    }
    return 0;
}
// generate the sudoku 
int generateValidSudoku(uint8_t (*grid)[SUDOKU_DIMENSIONS], int row, int col)
{
    if (row == SUDOKU_DIMENSIONS)
    {
        return 1;
    }
    if (col == SUDOKU_DIMENSIONS)
    {
        return generateValidSudoku(grid, row + 1, 0);
    }
    uint8_t *numbers = shuffle();
    int bit = getUsedVals(grid, row, col);

    for (int i = 0; i < SUDOKU_DIMENSIONS; i++)
    {
        uint8_t num = numbers[i];
        if (bit & (1 << numbers[i]))
        {
            continue;
        }
        grid[row][col] = num;
        if (generateValidSudoku(grid, row, col + 1))
        {
            return 1;
        }
        grid[row][col] = 0;
    }
    return 0;
}
