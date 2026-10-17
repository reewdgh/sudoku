#pragma once

#include <stdio.h>
#include <windows.h>
#include <time.h>
#include "../src/config.h"

extern int i;
#define X 0
#define Y 1
#define I 2
#define J 3
extern int START_POINT;
extern int POINT_GAP;
typedef struct
{
    int boardInfo[100][4];
    int emptyCells;
    int isEmptyCellClicked;
    int userGrid[9][9];
    int sudokuGrid[9][9];
    int score;
    int mistakes;
    int isStart;
    int unsolvedGrid[9][9];
    int prevIndex;
    int centerX;
    int centerY;
    int hintUsed;
} Sudoku;

#define CELL_I(index, ctx) ctx->boardInfo[index][I]
#define CELL_J(index, ctx) ctx->boardInfo[index][J]
#define CELL_X(index, ctx) ctx->boardInfo[index][X]
#define CELL_Y(index, ctx) ctx->boardInfo[index][Y]

void createNumberButtons(HWND hwnd);
void drawSudokuGrid(HDC hdc, Sudoku *ctx);
int fillEmptyCell(Sudoku *ctx, HWND hwnd);
void displayUserStats(HDC hdc, Sudoku *ctx);
int checkEmptyBoxClick(HWND hwnd, Sudoku *ctx);
void checkUserInput(HWND hwnd, int number, Sudoku *ctx);
void colorLevelButtons(int buttonID, LPDRAWITEMSTRUCT draw);
void drawImage(HDC hdc, char *imagePath, int x, int y, int width, int height);
int DeleteLevelButtons(HWND hwnd);
int createLevelButtons(Sudoku *ctx, HWND hwnd);
void colorNumberButtons(LPDRAWITEMSTRUCT draw, int buttonID);
void createHintButton(HWND hwnd);
void createStartScreen(HDC hdc, Sudoku *ctx);
void HandleKeyBoardInput(WPARAM wparm, Sudoku *ctx, HWND hwnd);
void isPointerNull(void *ptr);
