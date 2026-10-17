#include "../include/sudoku_game.h"

void fillWithZero(int (*grid1)[9], int (*grid2)[9], int level)
{
    copyMatrix(grid1, grid2);
    for (int i = 0; i < 9; i++)
    {
        int *numbers = shuffle();
        for (int j = 0; j < level; j++)
        {
            grid1[i][numbers[j]] = 0;
            if (countSolutions(grid1) == 0)
            {
                grid1[i][numbers[j]] = grid2[i][numbers[j]];
            }
        }
    }
}

int countSolutions(int (*grid)[9])
{
    int solveGrid1[9][9] = {0}, solvedGrid2[9][9] = {0};
    copyMatrix(solveGrid1, grid);
    solveSudoku(solveGrid1, 0, 0);
    srand(4);
    for (int i = 0; i < 9; i++)
    {
        copyMatrix(solvedGrid2, grid);
        solveSudoku(solvedGrid2, 0, 0);
        for (int i = 0; i < 9; i++)
        {
            for (int j = 0; j < 9; j++)
            {
                if (solvedGrid2[i][j] != solveGrid1[i][j])
                {
                    return 0;
                }
            }
        }
    }
    return 1;
}

int solveSudoku(int (*grid)[9], int row, int col)
{
    if (row == 9)
    {
        return 1;
    }
    if (col == 9)
    {
        return solveSudoku(grid, row + 1, 0);
    }
    if (grid[row][col] != 0)
    {
        return solveSudoku(grid, row, col + 1);
    }
    int numbers[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    for (int i = 8; i > 0; i--)
    {
        int random = rand() % (i + 1);
        int temp = numbers[i];
        numbers[i] = numbers[random];
        numbers[random] = temp;
    }
    for (int i = 0; i < 9; i++)
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

int copyMatrix(int (*grid1)[9], int (*grid2)[9])
{
    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            grid1[i][j] = grid2[i][j];
        }
    }
    return 0;
}

int *shuffle()
{
    static int numbers[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    for (int i = 0; i < 9; i++)
    {
        int random = rand() % 9;
        int temp = numbers[i];
        numbers[i] = numbers[random];
        numbers[random] = temp;
    }
    return numbers;
}

int checkValidPlacement(int (*grid)[9], int num, int row, int col)
{
    return checkRows(grid, num, row, col) &&
           checkCols(grid, num, row, col) &&
           checkSubGrid(grid, num, row, col);
}

int generateValidSudoku(int (*grid)[9], int row, int col)
{
    if (row == 9)
    {
        return 1;
    }
    if (col == 9)
    {
        return generateValidSudoku(grid, row + 1, 0);
    }
    int *numbers = shuffle();

    for (int i = 0; i < 9; i++)
    {
        int num = numbers[i];

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

int checkRows(int (*grid)[9], int number, int row, int col)
{
    for (int i = 0; i < 9; i++)
    {
        if (i != col && grid[row][i] == number)
        {
            return false;
        }
    }
    return true;
}

int checkCols(int (*grid)[9], int number, int row, int col)
{
    for (int i = 0; i < 9; i++)
    {
        if (i != row && grid[i][col] == number)
        {
            return false;
        }
    }
    return true;
}

int checkSubGrid(int (*grid)[9], int number, int row, int col)
{
    int first = (row / 3) * 3;
    int second = (col / 3) * 3;

    for (int i = first; i < first + 3; i++)
    {
        for (int j = second; j < second + 3; j++)
        {
            if ((i != row || j != col) && grid[i][j] == number)
            {
                return false;
            }
        }
    }
    return true;
}
