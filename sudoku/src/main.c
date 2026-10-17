
#include <stdio.h>
#include <windows.h>
#include <time.h>
#include "../include/sudoku_gui.h"
#include "../include/sudoku_game.h"
LRESULT CALLBACK cb(HWND hwnd, UINT msg, WPARAM wparm, LPARAM lparm)
{
    Sudoku *ctx = (Sudoku *)GetWindowLongPtr(hwnd, GWLP_USERDATA);

    int number_id[] = NUMBER_BUTTONS_ID;
    int level_id[] = LEVEL_IDS;
    switch (msg)
    {
    case WM_CLOSE:
    {
        PostQuitMessage(0);
        return 0;
    }
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        if (ctx->isStart)
        {
            createStartScreen(hdc, ctx);
            break;
        }
        drawSudokuGrid(hdc, ctx);
        displayUserStats(hdc, ctx);
        EndPaint(hwnd, &ps);
        break;
    }

    case WM_CREATE:
    {
        CREATESTRUCT *cs = (CREATESTRUCT *)lparm;
        ctx = (Sudoku *)cs->lpCreateParams;
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)ctx);
        HICON hicon;
        hicon = (HICON)LoadImage(NULL, SUDOKU_ICON_PATH, IMAGE_ICON, 500, 500, LR_LOADFROMFILE);
        SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hicon);
        if (ctx->isStart)
        {
            RECT rect;
            GetClientRect(hwnd, &rect);
            ctx->centerX = (rect.left + rect.right) / 2;
            ctx->centerY = (rect.bottom + rect.top) / 2;
            createLevelButtons(ctx, hwnd);
        }
        break;
    }
    case WM_DRAWITEM:
    {
        LPDRAWITEMSTRUCT draw = (LPDRAWITEMSTRUCT)lparm;
        int buttonID = LOWORD(wparm);
        if (buttonID == HINT_ID)
        {
            drawImage(draw->hDC, HINT_BMP_PATH, 0, 0, START_POINT, START_POINT);
        }
        else if (buttonID >= number_id[0] && buttonID <= number_id[SUDOKU_DIMENSIONS - 1])
        {
            colorNumberButtons(draw, buttonID);
        }
        else if (buttonID >= level_id[0] && buttonID <= level_id[LEVEL_COUNT - 1])
        {
            colorLevelButtons(buttonID, draw);
        }
        break;
    }

    case WM_COMMAND:
    {
        int buttonID = LOWORD(wparm);
        if (ctx->isStart == 1 && buttonID >= level_id[0] && buttonID <= level_id[LEVEL_COUNT - 1])
        {
            DeleteLevelButtons(hwnd);
            ctx->isStart = 0;
            fillWithZero(ctx->userGrid, ctx->sudokuGrid, (buttonID - (level_id[0] - 1)) * 2);
            copyMatrix(ctx->unsolvedGrid, ctx->userGrid);
            createNumberButtons(hwnd);
            createHintButton(hwnd);
            break;
        }
        if (buttonID >= number_id[0] && buttonID <= number_id[SUDOKU_DIMENSIONS - 1] && ctx->isEmptyCellClicked)
        {
            checkUserInput(hwnd, buttonID, ctx);
        }
        if (buttonID == HINT_ID && ctx->isEmptyCellClicked)
        {
            if (ctx->hintUsed != MAX_HINT)
            {
                checkUserInput(hwnd, ctx->sudokuGrid[ctx->boardInfo[i][I]][ctx->boardInfo[i][J]], ctx);
                ctx->hintUsed++;
            }
            else
            {
                MessageBox(hwnd, "Max Limit of Hint Used(3)", "Message", MB_OK | MB_ICONQUESTION);
            }
        }
        SetFocus(hwnd);
        break;
    }

    case WM_LBUTTONDOWN:
    {
        int check = checkEmptyBoxClick(hwnd, ctx);
        if (check != 0)
        {
            if (i == ctx->prevIndex)
            {
                break;
            }
            for (int j = 0; j < 81; j++)
            {
                if (ctx->sudokuGrid[CELL_I(j, ctx)][CELL_J(j, ctx)] == ctx->sudokuGrid[CELL_I(ctx->prevIndex, ctx)][CELL_J(ctx->prevIndex, ctx)])
                {
                    RECT rect = {.left = CELL_X(j, ctx) + 1, .top = CELL_Y(j, ctx) + 1, .right = CELL_X(j, ctx) + START_POINT, .bottom = CELL_Y(j, ctx) + START_POINT};
                    HRGN rgn = CreateRectRgn(rect.left, rect.top, rect.right, rect.bottom);
                    InvalidateRgn(hwnd, rgn, TRUE);
                    UpdateWindow(hwnd);
                }
            }
        }

        fillEmptyCell(ctx, hwnd);
        break;
    }

    case WM_KEYDOWN:
    {
        if (ctx->isStart == 0)
        {
            HandleKeyBoardInput(wparm, ctx, hwnd);
        }
        break;
    }
    default:
        return DefWindowProc(hwnd, msg, wparm, lparm);
    }
    return 0;
}

int main()
{
    srand((unsigned int)time(NULL));
    Sudoku ctx = {};
    ctx.isStart = 1;
    generateValidSudoku(ctx.sudokuGrid, 0, 0);
    solveSudoku(ctx.sudokuGrid, 0, 0);

    HINSTANCE hInstance = GetModuleHandle(0);
    WNDCLASS class = {};

    char classTitle[] = "my class";
    class.lpszClassName = classTitle;
    class.lpfnWndProc = cb;
    class.hInstance = hInstance;
    class.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    class.hCursor = LoadCursor(NULL, IDC_ARROW);
    int width = GetSystemMetrics(SM_CXSCREEN);
    float cal = ((float)100 / DEFAULT_WIDTH) * width;
    START_POINT = (int)cal;
    RegisterClass(&class);
    HWND hwnd = CreateWindowEx(0, classTitle, "Sudoku",
                               WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, 0, 0, 1000, 1000, NULL,
                               NULL, hInstance, &ctx);
    if (hwnd == NULL)
    {
        MessageBox(NULL, "Something Went worng", "Error", MB_OK);
        return 0;
    }
    ShowWindow(hwnd, SHOW_FULLSCREEN);
    MSG msg;
    RECT rect;
    GetClientRect(hwnd, &rect);
    ctx.centerX = (rect.left + rect.right) / 2;
    ctx.centerY = (rect.bottom + rect.top) / 2;
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
