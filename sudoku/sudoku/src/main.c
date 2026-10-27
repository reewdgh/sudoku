/*----------------------------------------------------------
Sudoku GUI in c lang with win32
HWND : handle to a window/ HWND identifies the actual instance of some Window Class
WPARAM  : use it to pass things like handles and integers
LPARAM  : use it  pass pointers.
Msg :  message information from a thread's message queue
RECT : RECT struct that store the left,right,bottom and top value to pass it to rectangle drawing function
PAINTSTRUCT : PAINTSTRUCT structure contains information for window
*/

#include "../include/sudoku_gui.h"
#include "../include/sudoku_game.h"


void runGame(int level, Sudoku *ctx)
{
    generateValidSudoku(ctx->sudokuGrid, 0, 0);
    solveSudoku(ctx->sudokuGrid, 0, 0);
    fillWithZero(ctx->userGrid, ctx->sudokuGrid, level * 2);
    memcpy(ctx->unsolvedGrid, ctx->userGrid, TOTAL_CELLS * sizeof(uint8_t));

    ctx->isStart = false;
}

/*
Function:LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
PURPOSE: Processes messages for the main window
Messages

*   FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
*
*   WM_COMMAND  - posted when user interacts with a child window
*   WM_PAINT    - posted when window requests to Paint the main window
*   WM_CLOSE    - posted when  window requests quit message
*   WM_CREATE   - posted when window requests that a window be created
*   WM_DRAWITEM - posted when an button request custom draw
*   WM_LBUTTONDOWN - Posted when the user presses the left mouse button
*   WM_KEYDOWN    - Posted when the user clicks any key.

*/

LRESULT CALLBACK cb(HWND hwnd, UINT msg, WPARAM wparm, LPARAM lparm)
{
    int number_id[] = NUMBER_BUTTONS_ID;
    int level_id[] = LEVEL_IDS;
    // Retrieve a pointer that was previously passed from the callback also here so every window message can acces it
    Sudoku *ctx = (Sudoku *)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    switch (msg)
    {
    case WM_CREATE:
    {
        
        // retrive it from a lparam
        CREATESTRUCT *cs = (CREATESTRUCT *)lparm;
        // asgin the struct
        ctx = (Sudoku *)cs->lpCreateParams;
        // save it forever in WM_CREATE then u can access this for all messages
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)ctx);
        // load the icon
        HICON hicon;
        hicon = (HICON)LoadImage(NULL, SUDOKU_ICON_PATH, IMAGE_ICON, 1000, 1000, LR_LOADFROMFILE);
        SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hicon);
        if (ctx->isStart)
        {
            RECT rect;
            // Get the inner drawing client area dimension
            GetClientRect(hwnd, &rect);
            // calculate X and Y center
            ctx->centerX = (rect.left + rect.right) / 2;
            ctx->centerY = (rect.bottom + rect.top) / 2;
            createLevelButtons(ctx, hwnd);
        }
        break;
    }
    case WM_CLOSE:
    {
        // Close the window and end the process
        PostQuitMessage(0);
        return 0;
    }
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        // The BeginPaint function prepares the specified window for painting and fills a PAINTSTRUCT structure with
        //  information about the window
        HDC hdc = BeginPaint(hwnd, &ps);
        if (ctx->isStart)
        {
            // if its the start of the game display image and title acording to centerX and centerY
            HFONT font = CreateFont(startPoint - LINES_TO_DRAW, 0, 0, 0, FW_MEDIUM, 0, 0, 0, 0, 0, 0, 0, 0, MAIN_FONT);
            SelectObject(hdc, font);
            SetTextAlign(hdc, TA_CENTER | TA_RIGHT);
            TextOut(hdc, ctx->centerX - SUDOKU_DIMENSIONS * 2, ctx->centerY - startPoint * 2, "Sudoku", strlen("Sudoku"));
            drawImage(hdc, SUDOKU_BMP_PATH, ctx->centerX - startPoint * 1.75, startPoint / 4, startPoint * 3, startPoint * 3);
            break;
        }

        drawSudokuGrid(hdc, ctx);
        displayUserStats(hdc, ctx);
        EndPaint(hwnd, &ps);
        break;
    }

    case WM_DRAWITEM:
    {
        // retrive the drawing struct which has the id, coordinates etc
        LPDRAWITEMSTRUCT draw = (LPDRAWITEMSTRUCT)lparm;

        int buttonID = LOWORD(wparm);
        if (buttonID == HINT_ID)
        {
            drawImage(draw->hDC, HINT_BMP_PATH, 0, 0, startPoint, startPoint);
        }
        else if (buttonID >= number_id[0] && buttonID <= number_id[SUDOKU_DIMENSIONS - 1])
        {
            colorNumberButtons(draw);
        }
        else if (buttonID >= level_id[0] && buttonID <= level_id[LEVEL_COUNT - 1])
        {
            colorLevelButtons(draw);
        }
        break;
    }

    case WM_COMMAND:
    {
        int buttonID = LOWORD(wparm);
        if (ctx->isStart == 1 && buttonID >= level_id[0] && buttonID <= level_id[LEVEL_COUNT - 1])
        {
            // run the sudoku logic
            runGame((buttonID - (level_id[0] - 1)), ctx);
            DeleteLevelButtons(hwnd);
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
            handleHintButton(hwnd, ctx);
        }

        SetFocus(hwnd);
        break;
    }

    case WM_LBUTTONDOWN:
    {
        if (checkEmptyBoxClick(hwnd, ctx) != 0 && curIndex != ctx->prevIndex)
        {
            redrawClickedArea(0, ctx, hwnd);
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

int main(void)
{
    srand((unsigned int)time(NULL));
    Sudoku ctx = {};
    ctx.isStart = true;
    HINSTANCE hInstance = GetModuleHandle(0);
    WNDCLASS class = {};

    char classTitle[] = "my class";
    class.lpszClassName = classTitle;
    class.lpfnWndProc = cb;
    class.hInstance = hInstance;
    class.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    class.hCursor = LoadCursor(NULL, IDC_ARROW);
    startPoint = (int)(DEFAULT_START_POINT / DEFAULT_WIDTH * GetSystemMetrics(SM_CXSCREEN));

    RegisterClass(&class);
    HWND hwnd = CreateWindowEx(0, classTitle, "Sudoku",
                               WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, 0, 0, 1000, 1000, NULL,
                               NULL, hInstance, &ctx);
    if (hwnd == NULL)
    {
        MessageBox(NULL, "Something Went wrong", "Error", MB_OK);
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
