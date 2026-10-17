#include "../include/sudoku_gui.h"
int i = 0;
#define X 0
#define Y 1
#define I 2
#define J 3
int START_POINT;

// Sudoku s1 = {};

void drawSudokuGrid(HDC hdc, Sudoku *ctx)
{
    isPointerNull(ctx);
    int y = START_POINT, x = START_POINT;
    HPEN defaultPen = CreatePen(PS_SOLID, 1, BLACK);
    HPEN boldPen = CreatePen(PS_SOLID, 2, BLACK);
    for (int i = 0; i < LINES_TO_DRAW; i++)
    {
        if (i % 3 == 0)
        {
            SelectObject(hdc, boldPen);
        }
        MoveToEx(hdc, y, START_POINT, 0);
        LineTo(hdc, y, END_POINT);
        SelectObject(hdc, defaultPen);
        y += START_POINT;
    }
    for (int j = 0; j < LINES_TO_DRAW; j++)
    {
        if (j % 3 == 0)
        {
            SelectObject(hdc, boldPen);
        }
        MoveToEx(hdc, START_POINT, x, 0);
        LineTo(hdc, END_POINT, x);
        SelectObject(hdc, defaultPen);
        x += START_POINT;
    }
    DeleteObject(defaultPen);
    DeleteObject(boldPen);
    HFONT font = CreateFont(START_POINT - 10, 0, 0, 0, FW_MEDIUM, 0, 0, 0, 0, 0, 0, 0, 0, MAIN_FONT);
    SelectObject(hdc, font);

    int a = 0;
    ctx->emptyCells = 0;
    SetBkMode(hdc, TRANSPARENT);

    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            ctx->boardInfo[a][X] = (START_POINT * (j + 1));
            ctx->boardInfo[a][Y] = (START_POINT * (i + 1));
            ctx->boardInfo[a][I] = i;
            ctx->boardInfo[a][J] = j;
            a++;
            if (ctx->userGrid[i][j] == 0)
            {
                ctx->emptyCells++;
                continue;
            }
            if (ctx->unsolvedGrid[i][j] == 0)
            {
                SetTextColor(hdc, BLUE);
            }
            char s[2];
            sprintf(s, "%d", ctx->userGrid[i][j]);

            RECT cellRect;
            cellRect.left = START_POINT + (START_POINT * j);
            cellRect.top = START_POINT + (START_POINT * i);
            cellRect.right = cellRect.left + START_POINT;
            cellRect.bottom = cellRect.top + START_POINT;

            DrawText(hdc, s, 1, &cellRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            SetTextColor(hdc, BLACK);
        }
    }
    DeleteObject(font);
}

void createNumberButtons(HWND hwnd)
{
    HDC hdc = GetDC(hwnd);
    HFONT font = CreateFont(START_POINT, 0, 0, 0, FW_MEDIUM, 0, 0, 0, 0, 0, 0, 0, 0, MAIN_FONT);
    SelectObject(hdc, font);

    int y = (START_POINT * 4), x = START_POINT * 11, j = 0, z = 0;

    for (int i = 0; i < 9; i++)
    {
        if (i % 3 == 0 && i != 0)
        {
            y = y + START_POINT * 2;
            z = 0;
            x = START_POINT * (z + 11);
        }
        char s[10];
        sprintf(s, "%d ", j + 1);
        HWND button = CreateWindow("button", s, WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, x, y,
                                   START_POINT * 3 / 2, START_POINT * 3 / 2, hwnd, (HMENU)(INT_PTR)(j + 1), 0, 0);
        SendMessage(button, WM_SETFONT, (WPARAM)font, TRUE);
        x = (START_POINT * ((z + 1) + 11)) + START_POINT * (z + 1);
        j++;
        z++;
    }
    ReleaseDC(hwnd, hdc);
}

void checkUserInput(HWND hwnd, int number, Sudoku *ctx)
{
    isPointerNull(ctx);

    if (ctx->sudokuGrid[CELL_I(i, ctx)][CELL_J(i, ctx)] == number)
    {
        ctx->userGrid[CELL_I(i, ctx)][CELL_J(i, ctx)] = number;
        ctx->score += MAX_INCREMENT_SCORE;
        ctx->emptyCells--;
        ctx->isEmptyCellClicked = 0;
    }
    else
    {
        PlaySound("fah.wav", NULL, SND_ASYNC);
        MessageBox(hwnd, "Invalid placement, Try another number", "Error!", MB_OK | MB_ICONQUESTION);
        ctx->mistakes++;
        if (ctx->mistakes == 5)
        {
            MessageBox(hwnd, "Max limit of Mistakes Reached(5)!", "msg", MB_OK | MB_ICONASTERISK);
            PostQuitMessage(0);
        }
    }

    drawSudokuGrid(GetDC(hwnd), ctx);
    displayUserStats(GetDC(hwnd), ctx);

    if (!ctx->emptyCells)
    {
        MessageBox(hwnd, "Congratulations, You solved the Sudoku puzzle!", "Message ", MB_OK | MB_ICONINFORMATION);
        PostQuitMessage(0);
    }
}

int checkEmptyBoxClick(HWND hwnd, Sudoku *ctx)
{
    isPointerNull(ctx);

    POINT p = {.x = 0, .y = 0};
    GetCursorPos(&p);
    ScreenToClient(hwnd, &p);

    if (p.x <= END_POINT && p.x >= START_POINT && p.y <= END_POINT && p.y >= START_POINT)
    {
        for (i = 0; i < 81; i++)
        {
            if ((p.x <= CELL_X(i, ctx) + START_POINT &&
                 p.y <= CELL_Y(i, ctx) + START_POINT) &&
                (p.x >= CELL_X(i, ctx) &&
                 p.y >= CELL_Y(i, ctx)))
            {
                if (ctx->userGrid[CELL_I(i, ctx)][CELL_J(i, ctx)] == 0)
                {
                    ctx->isEmptyCellClicked = 1;
                    return 1;
                }
                return 2;
            }
        }
    }
    ctx->isEmptyCellClicked = 0;
    return 0;
}

int fillEmptyCell(Sudoku *ctx, HWND hwnd)
{
    isPointerNull(ctx);

    HBRUSH hBrush2 = CreateSolidBrush(RGB(176, 209, 232));
    ctx->prevIndex = i;
    HDC hdc = GetDC(hwnd);

    if (checkEmptyBoxClick(hwnd, ctx) == 1)
    {
        RECT rect = {.left = CELL_X(i, ctx) + 1, .top = CELL_Y(i, ctx) + 1, .right = CELL_X(i, ctx) + START_POINT, .bottom = CELL_Y(i, ctx) + START_POINT};
        FillRect(hdc, &rect, hBrush2);
        ctx->isEmptyCellClicked = 1;
        ReleaseDC(hwnd, hdc);
        DeleteObject(hBrush2);
        return 0;
    }

    if (checkEmptyBoxClick(hwnd, ctx) == 2)
    {
        for (int j = 0; j < 81; j++)
        {
            if (ctx->userGrid[CELL_I(j, ctx)][CELL_J(j, ctx)] == 0)
            {
                continue;
            }
            if (ctx->sudokuGrid[CELL_I(j, ctx)][CELL_J(j, ctx)] == ctx->sudokuGrid[CELL_I(i, ctx)][CELL_J(i, ctx)])
            {
                RECT rect1 = {.left = CELL_X(j, ctx) + 1, .top = CELL_Y(j, ctx) + 1, .right = CELL_X(j, ctx) + START_POINT, .bottom = CELL_Y(j, ctx) + START_POINT};
                FillRect(hdc, &rect1, hBrush2);
            }
        }
        drawSudokuGrid(hdc, ctx);
    }

    ctx->isEmptyCellClicked = 0;
    DeleteObject(hBrush2);

    return 0;
}

void displayUserStats(HDC hdc, Sudoku *ctx)
{
    isPointerNull(ctx);

    HFONT font = CreateFont(START_POINT / 2, 0, 0, 0, FW_SEMIBOLD, 0, 0, 0, 0, 0, 0, 0, 0, MAIN_FONT);
    SelectObject(hdc, font);
    char scoreMsg[20], mistakeMsg[20];
    sprintf(scoreMsg, "Score : %d", ctx->score);
    sprintf(mistakeMsg, "Mistakes: %d", ctx->mistakes);
    SetTextColor(hdc, OFF_WHITE);
    TextOut(hdc, END_POINT + START_POINT, END_POINT / 4, scoreMsg, strlen(scoreMsg));
    TextOut(hdc, END_POINT + END_POINT / 2, END_POINT / 4, mistakeMsg, strlen(mistakeMsg));
    DeleteObject(font);
}

void colorLevelButtons(int buttonID, LPDRAWITEMSTRUCT draw)
{
    FillRect(draw->hDC, &draw->rcItem, CreateSolidBrush(DARK_BLUE));
    SetBkMode(draw->hDC, TRANSPARENT);
    SetTextColor(draw->hDC, WHITE);
    HFONT font = CreateFont(START_POINT - 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, MAIN_FONT);
    SelectObject(draw->hDC, font);
    char *levels[3] = LEVELS;
    int level[3] = LEVEL_IDS;
    for (int i = 0; i < 3; i++)
    {
        if (buttonID == level[i])
        {
            DrawText(draw->hDC, levels[i], -1, &draw->rcItem, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
            break;
        }
    }
}

void drawImage(HDC hdc, char *imagePath, int x, int y, int width, int height)
{

    HBITMAP hbitmap = (HBITMAP)LoadImage(
        NULL,
        imagePath,
        IMAGE_BITMAP,
        0,
        0,
        LR_LOADFROMFILE);

    if (hbitmap == NULL)
    {
        OutputDebugString("FILE NOT FOUND");
        return;
    }
    BITMAP bmp;
    GetObject(hbitmap, sizeof(bmp), &bmp);
    HDC newHDC = CreateCompatibleDC(hdc);

    HBITMAP oldbmp = SelectObject(newHDC, (HBITMAP)hbitmap);
    StretchBlt(hdc, x, y, width, height, newHDC, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);
    SelectObject(newHDC, oldbmp);
    DeleteDC(newHDC);
    DeleteObject(&bmp);
}

int DeleteLevelButtons(HWND hwnd)
{
    int id[3] = LEVEL_IDS;
    for (int i = 0; i < 3; i++)
    {
        DestroyWindow(GetDlgItem(hwnd, id[i]));
    }
    InvalidateRect(hwnd, NULL, TRUE);
    return 0;
}

int createLevelButtons(Sudoku *ctx, HWND hwnd)
{
    isPointerNull(ctx);

    int step = 5;
    int id[] = LEVEL_IDS;
    for (int i = 0; i < 3; i++)
    {
        CreateWindow("BUTTON", "", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_OWNERDRAW, ctx->centerY + START_POINT * 2, START_POINT * step, START_POINT * 5, START_POINT, hwnd, (HMENU)(INT_PTR)id[i], 0, NULL);
        step = step + 2;
    }
    return 0;
}

void colorNumberButtons(LPDRAWITEMSTRUCT draw, int buttonID)
{
    char s[2];
    sprintf(s, "%d", buttonID);
    FillRect(draw->hDC, &draw->rcItem, CreateSolidBrush(LIGHT_BLUE));
    SetTextColor(draw->hDC, BLUE);
    SetBkColor(draw->hDC, LIGHT_BLUE);
    DrawText(draw->hDC, s, -1, &draw->rcItem, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
}

void createHintButton(HWND hwnd)
{
    CreateWindowEx(0, "button", "Hint", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_OWNERDRAW, END_POINT + END_POINT / 3, START_POINT * 2,
                   START_POINT, START_POINT, hwnd, (HMENU)(INT_PTR)HINT_ID, 0, 0);
}

void createStartScreen(HDC hdc, Sudoku *ctx)
{

    HFONT font = CreateFont(START_POINT - 10, 0, 0, 0, FW_MEDIUM, 0, 0, 0, 0, 0, 0, 0, 0, MAIN_FONT);
    SelectObject(hdc, font);
    SetTextAlign(hdc, TA_CENTER | TA_RIGHT);
    TextOut(hdc, ctx->centerX - SUDOKU_DIMENSIONS * 2, ctx->centerY - START_POINT * 2, "Sudoku", strlen("Sudoku"));
    drawImage(hdc, SUDOKU_BMP_PATH, ctx->centerX - START_POINT * 1.75, START_POINT / 4, START_POINT * 3, START_POINT * 3);
}

void HandleKeyBoardInput(WPARAM wparm, Sudoku *ctx, HWND hwnd)
{
    isPointerNull(ctx);

    int press = (int)wparm - '0';
    if (press >= 1 && press <= 9)
    {
        if (ctx->isEmptyCellClicked)
        {
            checkUserInput(hwnd, press, ctx);
        }
    }
}

void isPointerNull(void *ptr)
{
    if (ptr == NULL)
    {
        MessageBox(0, "Data passed is Null, Kindly check", "message", MB_OK | MB_ICONERROR);
        PostQuitMessage(0);
    }
}
