#include "../include/sudoku_game.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

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

static int checkValidPlacement(uint8_t (*grid)[SUDOKU_DIMENSIONS], int num, int row, int col)
{
    for (int i = 0; i < SUDOKU_DIMENSIONS; i++)
    {
        if (i != col && grid[row][i] == num)
        {
            return false;
        }
    }

    for (int i = 0; i < SUDOKU_DIMENSIONS; i++)
    {
        if (i != row && grid[i][col] == num)
        {
            return false;
        }
    }

    int first = (row / 3) * SUBGRID_DIMENSTIONS;
    int second = (col / 3) * SUBGRID_DIMENSTIONS;

    for (int i = first; i < first + SUBGRID_DIMENSTIONS; i++)
    {
        for (int j = second; j < second + SUBGRID_DIMENSTIONS; j++)
        {
            if ((i != row || j != col) && grid[i][j] == num)
            {
                return false;
            }
        }
    }

    return true;
}

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
    for (int i = 0; i < SUDOKU_DIMENSIONS; i++)
    {
        if (checkValidPlacement(grid, numbers[i], row, col))
        {
            grid[row][col] = numbers[i];
            if (solveSudoku(grid, row, col + 1))
            {
                return 1;
            }
            grid[row][col] = 0;
        }
    }
    return 0;
}

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

    for (int i = 0; i < SUDOKU_DIMENSIONS; i++)
    {
        uint8_t num = numbers[i];

        if (checkValidPlacement(grid, num, row, col))
        {
            grid[row][col] = num;
            if (generateValidSudoku(grid, row, col + 1))
            {
                return 1;
            }
            grid[row][col] = 0;
        }
    }
    return 0;
}
