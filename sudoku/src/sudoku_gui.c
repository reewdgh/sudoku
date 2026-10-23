#include "../include/sudoku_gui.h"
int curIndex;
int startPoint;

// Sudoku s1 = {};
void UpdateOrFill(int index, Sudoku *ctx, HWND hwnd, int updateOrFill)
{
    int x = CELL_X(index, ctx), y = CELL_Y(index, ctx);
    RECT rect = {.left = x + 1, .top = y + 1, .right = x + startPoint, .bottom = y + startPoint};

    if (updateOrFill == 0)
    {
        HRGN rgn = CreateRectRgn(rect.left, rect.top, rect.right, rect.bottom);
        InvalidateRgn(hwnd, rgn, TRUE);
        DeleteObject(rgn);
    }
    else
    {
        FillRect(GetDC(hwnd), &rect, CreateSolidBrush(SKY_BLUE));
    }
}

void drawSudokuGrid(HDC hdc, Sudoku *ctx)
{
    isPointerNull(ctx);
    int y = startPoint, x = startPoint;
    HPEN defaultPen = CreatePen(PS_SOLID, 1, BLACK);
    HPEN boldPen = CreatePen(PS_SOLID, 3, BLACK);
    // Vertical Lines
    for (int i = 0; i < LINES_TO_DRAW; i++)
    {
        // if its a third line of the subgrid then use the bold brush
        if (i % 3 == 0)
        {
            SelectObject(hdc, boldPen);
        }
        // move the previous brush position to the specifed position
        MoveToEx(hdc, y, startPoint, 0);
        LineTo(hdc, y, END_POINT);
        SelectObject(hdc, defaultPen);
        y += startPoint;
    }
    // Horizontal Lines
    for (int j = 0; j < LINES_TO_DRAW; j++)
    {
        if (j % 3 == 0)
        {
            SelectObject(hdc, boldPen);
        }
        MoveToEx(hdc, startPoint, x, 0);
        LineTo(hdc, END_POINT, x);
        SelectObject(hdc, defaultPen);
        x += startPoint;
    }
    // delete pens/fon to prevent GDI/memory leaks.
    DeleteObject(defaultPen);
    DeleteObject(boldPen);
    HFONT font = CreateFont(startPoint - 10, 0, 0, 0, FW_MEDIUM, 0, 0, 0, 0, 0, 0, 0, 0, MAIN_FONT);
    SelectObject(hdc, font);
    // add
    int a = 0;
    ctx->emptyCells = 0;
    // set Bk mode to transparent so it dosent show white behind the text
    SetBkMode(hdc, TRANSPARENT);

    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            // save the board cordinates positions and index (X,Y,I,J)
            ctx->boardInfo[a][X] = (startPoint * (j + 1));
            ctx->boardInfo[a][Y] = (startPoint * (i + 1));
            ctx->boardInfo[a][I] = i;
            ctx->boardInfo[a][J] = j;
            a++;
            if (ctx->userGrid[i][j] == 0)
            {
                ctx->emptyCells++;
                continue;
            }
            // if user has solved this cell then use blue text color
            if (ctx->unsolvedGrid[i][j] == 0)
            {
                SetTextColor(hdc, BLUE);
            }
            // turn int to string
            char s[2] = {'0' + ctx->userGrid[i][j], '\0'};
            // define cell rectangle;
            RECT cellRect;
            cellRect.left = startPoint + (startPoint * j);
            cellRect.top = startPoint + (startPoint * i);
            cellRect.right = cellRect.left + startPoint;
            cellRect.bottom = cellRect.top + startPoint;
            DrawText(hdc, s, 1, &cellRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            SetTextColor(hdc, BLACK);
        }
    }
    DeleteObject(font);
}
void createNumberButtons(HWND hwnd)
{

    HDC hdc = GetDC(hwnd);
    HFONT font = CreateFont(startPoint, 0, 0, 0, FW_MEDIUM, 0, 0, 0, 0, 0, 0, 0, 0, MAIN_FONT);
    SelectObject(hdc, font);
    int y = (startPoint * (SUBGRID_DIMENSTIONS + 1)), x = startPoint * (LINES_TO_DRAW + 1), j = 0, z = 0;

    for (int i = 0; i < SUDOKU_DIMENSIONS; i++)
    {
        if (i % (int)SUBGRID_DIMENSTIONS == 0 && i != 0)
        {
            y = y + startPoint * (SUBGRID_DIMENSTIONS - 1);
            z = 0;
            x = startPoint * (z + (LINES_TO_DRAW + 1));
        }
        char s[2] = {'0' + j + 1, '\0'};
        HWND button = CreateWindow("button", s, WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, x, y,
                                   startPoint * SUBGRID_DIMENSTIONS / 2, startPoint * SUBGRID_DIMENSTIONS / 2, hwnd, (HMENU)(INT_PTR)(j + 1), 0, 0);
        SendMessage(button, WM_SETFONT, (WPARAM)font, TRUE);
        x = (startPoint * ((z + 1) + (LINES_TO_DRAW + 1))) + startPoint * (z + 1);
        j++;
        z++;
    }

    ReleaseDC(hwnd, hdc);
}

void checkUserInput(HWND hwnd, int number, Sudoku *ctx)
{
    isPointerNull(ctx);

    if (ctx->sudokuGrid[CELL_I(curIndex, ctx)][CELL_J(curIndex, ctx)] == number)
    {
        ctx->userGrid[CELL_I(curIndex, ctx)][CELL_J(curIndex, ctx)] = number;
        ctx->score += MAX_INCREMENT_SCORE;
        ctx->emptyCells--;
        redrawClickedArea(1, ctx, hwnd);
        ctx->isEmptyCellClicked = 0;
    }
    else
    {
        PlaySound("sound.wav", NULL, SND_ASYNC);
        if (ctx->mistakes == MISTAKES_LIMIT + 1)
        {
            MessageBox(hwnd, "Max limit of Mistakes Reached!", "msg", MB_OK | MB_ICONASTERISK);
            PostQuitMessage(0);
        }
        MessageBox(hwnd, "Invalid placement, Try another number", "Error!", MB_OK | MB_ICONQUESTION);
        ctx->mistakes++;
    }
    HDC hdc = GetDC(hwnd);
    displayUserStats(hdc, ctx);

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

    if (p.x <= END_POINT && p.x >= startPoint && p.y <= END_POINT && p.y >= startPoint)
    {
        for (curIndex = 0; curIndex < TOTAL_CELLS; curIndex++)
        {
            if ((p.x <= CELL_X(curIndex, ctx) + startPoint &&
                 p.y <= CELL_Y(curIndex, ctx) + startPoint) &&
                (p.x >= CELL_X(curIndex, ctx) &&
                 p.y >= CELL_Y(curIndex, ctx)))
            {
                if (ctx->userGrid[CELL_I(curIndex, ctx)][CELL_J(curIndex, ctx)] == 0)
                {
                    ctx->isEmptyCellClicked = true;
                    return 1;
                }
                return 2;
            }
        }
    }

    ctx->isEmptyCellClicked = false;
    return 0;
}

int fillEmptyCell(Sudoku *ctx, HWND hwnd)
{
    isPointerNull(ctx);

    ctx->prevIndex = curIndex;
    HDC hdc = GetDC(hwnd);

    if (checkEmptyBoxClick(hwnd, ctx) == 1)
    {
        UpdateOrFill(curIndex, ctx, hwnd, 1);
        ctx->isEmptyCellClicked = true;
        return 0;
    }

    if (checkEmptyBoxClick(hwnd, ctx) == 2)
    {
        for (int j = 0; j < TOTAL_CELLS; j++)
        {
            if (ctx->userGrid[CELL_I(j, ctx)][CELL_J(j, ctx)] == 0)
            {
                continue;
            }
            if (ctx->sudokuGrid[CELL_I(j, ctx)][CELL_J(j, ctx)] == ctx->sudokuGrid[CELL_I(curIndex, ctx)][CELL_J(curIndex, ctx)])
            {
                UpdateOrFill(j, ctx, hwnd, 1);
            }
        }
        drawSudokuGrid(hdc, ctx);
    }

    ctx->isEmptyCellClicked = false;

    return 0;
}
// display score and mistake
void displayUserStats(HDC hdc, Sudoku *ctx)
{
    isPointerNull(ctx);
    HFONT font = CreateFont(startPoint / 2, 0, 0, 0, FW_SEMIBOLD, 0, 0, 0, 0, 0, 0, 0, 0, MAIN_FONT);
    SelectObject(hdc, font);
    char scoreMsg[20], mistakeMsg[20];
    sprintf(scoreMsg, "Score : %d", ctx->score);
    sprintf(mistakeMsg, "Mistakes: %d", ctx->mistakes);
    SetTextColor(hdc, OFF_WHITE);
    TextOut(hdc, END_POINT + startPoint, END_POINT / (SUBGRID_DIMENSTIONS + 1), scoreMsg, strlen(scoreMsg));
    TextOut(hdc, END_POINT + END_POINT / (SUBGRID_DIMENSTIONS - 1), END_POINT / (SUBGRID_DIMENSTIONS + 1), mistakeMsg, strlen(mistakeMsg));
    DeleteObject(font);
}

void colorLevelButtons(LPDRAWITEMSTRUCT draw)
{
    FillRect(draw->hDC, &draw->rcItem, CreateSolidBrush(DARK_BLUE));
    SetBkMode(draw->hDC, TRANSPARENT);
    SetTextColor(draw->hDC, WHITE);
    HFONT font = CreateFont(startPoint - LINES_TO_DRAW, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, MAIN_FONT);
    SelectObject(draw->hDC, font);
    char *levels[3] = LEVELS;
    int level[3] = LEVEL_IDS;
    int index = draw->CtlID - level[0];
    DrawText(draw->hDC, levels[index], strlen(levels[index]), &draw->rcItem, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
}

void drawImage(HDC hdc, char *imagePath, int x, int y, int width, int height)
{
    // Load the bitmap from the given image path
    HBITMAP hbitmap = (HBITMAP)LoadImage(
        NULL,
        imagePath,
        IMAGE_BITMAP,
        0,
        0,
        LR_LOADFROMFILE);
    if (hbitmap == NULL)
    {
        OutputDebugString("File was not Found \n OR \n Given image was not a .bmp");
        return;
    }
    //  Retrieve Bitmap Dimensions
    BITMAP bmp;
    GetObject(hbitmap, sizeof(bmp), &bmp);
    // create the drawing area for image
    HDC newHDC = CreateCompatibleDC(hdc);
    // save the newDC with our image
    SelectObject(newHDC, (HBITMAP)hbitmap);
    // resize the bitmap into given formats
    StretchBlt(hdc, x, y, width, height, newHDC, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);
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
    // create the level buttons
    int step = 5;

    int id[] = LEVEL_IDS;
    for (int i = 0; i < LEVEL_COUNT; i++)
    {
        // this buttons are custom color/draw , so use BS_OWNERDRAW. this tells
        // window to not draw the default buttons style
        CreateWindow("BUTTON", "", WS_VISIBLE | WS_CHILD | BS_OWNERDRAW, ctx->centerX + startPoint * 2, startPoint * step, startPoint * 5, startPoint, hwnd, (HMENU)(INT_PTR)id[i], 0, NULL);
        step = step + 2;
    }
    return 0;
}

void colorNumberButtons(LPDRAWITEMSTRUCT draw)
{
    // int to char  
    char s[2] = {'0' + draw->CtlID, '\0'};
    FillRect(draw->hDC, &draw->rcItem, CreateSolidBrush(LIGHT_BLUE));
    SetTextColor(draw->hDC, BLUE);
    SetBkColor(draw->hDC, LIGHT_BLUE);
    DrawText(draw->hDC, s, -1, &draw->rcItem, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
}

void createHintButton(HWND hwnd)
{
    CreateWindowEx(0, "button", "Hint", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_OWNERDRAW, END_POINT + END_POINT / 3, startPoint * 2,
                   startPoint, startPoint, hwnd, (HMENU)(INT_PTR)HINT_ID, 0, 0);
}

void HandleKeyBoardInput(WPARAM wparm, Sudoku *ctx, HWND hwnd)
{
    isPointerNull(ctx);

    int press = (int)wparm - '0';
    int numbers[] = NUMBER_BUTTONS_ID;
    if (press >= numbers[0] && press <= SUDOKU_DIMENSIONS)
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

void redrawClickedArea(int isCell, Sudoku *ctx, HWND hwnd)
{
    int prev = ctx->prevIndex;

    if (isCell)
    {
        UpdateOrFill(prev, ctx, hwnd, 0);
    }
    else
    {
        int targetVal = ctx->sudokuGrid[CELL_I(prev, ctx)][CELL_J(prev, ctx)];
        for (int j = 0; j < SUDOKU_DIMENSIONS * 9; j++)
        {
            if (ctx->sudokuGrid[CELL_I(j, ctx)][CELL_J(j, ctx)] == targetVal)
            {
                UpdateOrFill(j, ctx, hwnd, 0);
            }
        }
    }
    UpdateWindow(hwnd);
}

int handleHintButton(HWND hwnd, Sudoku *ctx)
{
    if (ctx->hintUsed != MAX_HINT)
    {
        checkUserInput(hwnd, ctx->sudokuGrid[ctx->boardInfo[curIndex][I]][ctx->boardInfo[curIndex][J]], ctx);
        ctx->hintUsed++;
    }
    else
    {
        MessageBox(hwnd, "Max Limit of Hint Used", "Message", MB_OK | MB_ICONQUESTION);
    }
    return 0;
}