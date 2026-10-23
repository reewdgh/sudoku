// config define
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#define SUDOKU_DIMENSIONS 9
#define TOTAL_CELLS (SUDOKU_DIMENSIONS * SUDOKU_DIMENSIONS)
#define DEFAULT_START_POINT 100.0
#define SUBGRID_DIMENSTIONS sqrt(SUDOKU_DIMENSIONS)
#define LINES_TO_DRAW (SUDOKU_DIMENSIONS + 1)
#define END_POINT (LINES_TO_DRAW * startPoint)
#define DEFAULT_WIDTH 1920
#define DEFAULT_HEIGHT 1200
#define LEVEL_IDS {10, 11, 12}
#define NUMBER_BUTTONS_ID {1, 2, 3, 4, 5, 6, 7, 8, 9}
#define HINT_ID 40
#define MAIN_FONT "Segoe UI"
#define MAX_INCREMENT_SCORE 70  
#define LEVEL_COUNT 3
#define LEVELS {"Easy", "Medium", "Hard"}
#define WHITE RGB(255, 255, 255)
#define BLACK RGB(0, 0, 0)
#define BLUE RGB(0, 102, 190)
#define SKY_BLUE RGB(176, 209, 232)
#define DARK_BLUE RGB(71, 112, 202)
#define OFF_WHITE RGB(197, 203, 209)
#define LIGHT_BLUE RGB(234, 238, 244)
#define MAX_MISTAKES 5
#define MAX_HINT 3
#define SUDOKU_ICON_PATH "images/icon.ico"
#define SUDOKU_BMP_PATH "images/icon.bmp"
#define HINT_BMP_PATH "images/hint.bmp"
#define MISTAKES_LIMIT 5
