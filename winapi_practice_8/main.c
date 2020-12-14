#include "windows.h"
#include "stdio.h"
#include <time.h>
#include <stdlib.h>


#ifndef UNICODE
#define UNICODE
#endif
#define CELL_SIZE 40
#define BORDER_SIZE 2
#define SHIPS_DOTS_NUMBER 10

HINSTANCE           currentHInstance;
HWND                MainWindowHandle;
LPWSTR              CLASS_NAME = L"MainWindow";

enum CellState{
    Free,
    Ship,
    Missed,
    Bombed
};

//point = [0, 10]
struct Player;

BOOL turnIsPossible;
struct Cell{
//    BOOL isPlayer;
    POINT point;
    RECT pixelRect;
    POINT pixelTopLeftPoint;
    enum CellState cellState;
    struct Player *player;
};

//struct Ship{
//    byte length;
//    POINT startPoint;
//    POINT endPoint;
//    BOOL isDown;
//    struct Cell *cells;
//};

struct ChosenCell{
    struct Cell *cell;
    byte i, j;
};

struct Player{
    int shipDotsNumber;
    struct Cell battlefield[10][10];
    BOOL isPlayer;
};

struct Player player;
struct Player pc;

struct Player *currentPlayer;
//
//struct Cell pcBattleField[10][10];
//struct Cell playerBattleField[10][10];

//void showFields(HDC hdc, int offsetX, int offsetY, int size);
//void placeShips();

void showFields(HDC hdc, struct Cell battlefield[][10]);
BOOL changeChosenCell(WPARAM wParam);
void InitCells(int offsetX, int offsetY, int size, struct Cell battlefield[][10], struct Player *player);
void DrawCell(HDC hdc, struct Cell *cell);
void ProceedTurn();
void repaint(HWND hWnd);
BOOL MakeShot(struct Cell *cell);
void InitPlayers();
BOOL CheckWinner();
void placeRandomly(struct Cell battlefield[][10]);

int get_rand_range_int(const int min, const int max);

//int            randomSeed;
const COLORREF rgbRed   =  0x000000FF;
const COLORREF rgbGreen =  0x0000FF00;
const COLORREF rgbBlue  =  0x00FFCC66;
const COLORREF rgbBlack =  0x00000000;
const COLORREF rgbWhite =  0x00FFFFFF;

HPEN hBlackPen;
HPEN hWhitePen;
HPEN hRedPen;

HBRUSH hBlueBrush;
HBRUSH hWhiteBrush;
HBRUSH hRedBrush;
HBRUSH hBlackBrush;

struct ChosenCell chosenCell;

BOOL WINAPI InitializeApplication();

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR lpCmdLine,
                     int nCmdShow) {
    currentHInstance = hInstance;

    if (!InitializeApplication()) {
        printf("An error occurred");
        return 0;
    }

    MainWindowHandle = CreateWindowEx(
            0,
            CLASS_NAME,
            "Battle sea",
            WS_OVERLAPPEDWINDOW | WS_VISIBLE,

            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

            NULL,
            NULL,
            hInstance,
            NULL
    );


    if (MainWindowHandle == NULL) {
        printf("An error occurred: %lu", GetLastError());
        return 0;
    }

    MSG msg = {};

    while (GetMessage(&msg, NULL, 0, 0)) {
        if (!TranslateMDISysAccel(MainWindowHandle, &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return 0;
}

BOOL WINAPI InitializeApplication() {
    WNDCLASS wc = {};

    wc.lpfnWndProc = (WNDPROC) WindowProc;
    wc.hInstance = currentHInstance;
    wc.hbrBackground = (HBRUSH) (COLOR_APPWORKSPACE + 1);
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    if (!RegisterClass(&wc))
        return FALSE;

    return TRUE;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

    switch (uMsg) {
        case WM_KEYDOWN:
            changeChosenCell(wParam);
            break;
        case WM_COMMAND: {
            switch (wParam) {

            }
        }
            break;
        case WM_CREATE: {
            hBlackPen = CreatePen(PS_SOLID, BORDER_SIZE, rgbBlack);
            hWhitePen = CreatePen(PS_SOLID, 2, rgbWhite);
            hRedPen = CreatePen(PS_SOLID, 2, rgbRed);

            hBlueBrush = CreateSolidBrush(rgbBlue);
            hWhiteBrush = CreateSolidBrush(rgbBlue);
            hRedBrush = CreateSolidBrush(rgbRed);
            hBlackBrush = CreateSolidBrush(rgbBlack);

//            randomSeed = time(NULL);
//            srand(randomSeed);

            InitCells(500, 10, CELL_SIZE, pc.battlefield, &pc);

            InitCells(10, 10, CELL_SIZE, player.battlefield, &player);

            placeRandomly(player.battlefield);
            placeRandomly(pc.battlefield);

            chosenCell.cell = &pc.battlefield[0][0];
            chosenCell.i = 0;
            chosenCell.j = 0;
            turnIsPossible = TRUE;
//            int r = rand();      // Returns a pseudo-random integer between 0 and RAND_MAX.
            InitPlayers();
        }
            break;

        case WM_ERASEBKGND:
            return 1;

        case WM_PAINT: {
            RECT clientRect;
            PAINTSTRUCT ps;
            GetClientRect(hWnd, &clientRect);

            HDC hdc = BeginPaint(hWnd, &ps);

            HDC hCmpDC = CreateCompatibleDC(hdc);
            HBITMAP hBmp = CreateCompatibleBitmap(hdc, clientRect.right, clientRect.bottom);
            SelectObject(hCmpDC, hBmp);

            int saveDC = SaveDC(hCmpDC);

            FillRect(hCmpDC, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

//            showFields(hCmpDC, 20, 20, 10);


            showFields(hCmpDC, pc.battlefield);

            showFields(hCmpDC, player.battlefield);

            SetStretchBltMode(hdc, COLORONCOLOR);
            BitBlt(hdc, 0, 0, clientRect.right, clientRect.bottom,
                   hCmpDC, 0, 0, SRCCOPY);
            RestoreDC(hCmpDC, saveDC);
            // Удаляем ненужные системные объекты
            DeleteDC(hCmpDC);
            DeleteObject(hBmp);
            hCmpDC = NULL;



            EndPaint(hWnd, &ps);
        }
            break;

        case WM_CLOSE:
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);

}

void InitCells(int offsetX, int offsetY, int size, struct Cell battlefield[][10], struct Player *player){
    for (int i = 0; i < 10; ++i) {
        for (int j = 0; j < 10; ++j) {
            battlefield[i][j].cellState = Free;
            battlefield[i][j].point.x = j;
            battlefield[i][j].point.y = i;
            battlefield[i][j].pixelTopLeftPoint.x = offsetX + size * j + BORDER_SIZE*j;
            battlefield[i][j].pixelTopLeftPoint.y = offsetY + size * i + BORDER_SIZE*i;
            battlefield[i][j].player = player;
        }
    }

//    battlefield[0][0].cellState = Bombed;
//    battlefield[0][1].cellState = Missed;
//    battlefield[0][2].cellState = Ship;

}

void showFields(HDC hdc, struct Cell battlefield[][10]){

    for (int i = 0; i < 10; ++i) {
        for (int j = 0; j < 10; ++j) {
            DrawCell(hdc, &battlefield[i][j]);
        }
    }

    struct Cell *chosenCellToPaint = chosenCell.cell;
    SelectObject(hdc, hRedPen);
    SelectObject(hdc, GetStockObject( NULL_BRUSH ) );
    Rectangle(hdc, chosenCellToPaint->pixelTopLeftPoint.x, chosenCellToPaint->pixelTopLeftPoint.y, chosenCellToPaint->pixelTopLeftPoint.x + CELL_SIZE, chosenCellToPaint->pixelTopLeftPoint.y + CELL_SIZE);

}

void DrawCell(HDC hdc, struct Cell *cell){


    SelectObject(hdc, hBlackPen);
    SelectObject(hdc, hBlueBrush);

    POINT startPoint = cell->pixelTopLeftPoint;


    Rectangle(hdc, startPoint.x, startPoint.y, startPoint.x + CELL_SIZE, startPoint.y + CELL_SIZE);



    int paint_cell_size = CELL_SIZE * 0.75;

    startPoint.x += 0.11 * CELL_SIZE;
    startPoint.y += 0.11 * CELL_SIZE;

    if (cell->cellState == Ship){
        if (cell->player == &player) {
            SelectObject(hdc, hWhiteBrush);
            SelectObject(hdc, hWhitePen);

            Rectangle(hdc, startPoint.x, startPoint.y, startPoint.x + paint_cell_size, startPoint.y + paint_cell_size);
        }
    } else if (cell->cellState == Bombed) {
        SelectObject(hdc, hRedPen);
        SelectObject(hdc, hRedBrush);

        Ellipse(hdc, startPoint.x, startPoint.y, startPoint.x + paint_cell_size, startPoint.y + paint_cell_size);
    } else if (cell->cellState == Missed) {
        SelectObject(hdc, hBlackPen);
        SelectObject(hdc, hBlackBrush);

        Ellipse(hdc, startPoint.x, startPoint.y,startPoint.x + paint_cell_size, startPoint.y + paint_cell_size);
    }

}

BOOL changeChosenCell(WPARAM wParam){

    switch (wParam) {
        case VK_LEFT: {
            if (chosenCell.j == 0)
                chosenCell.j = 9;
            else
                chosenCell.j -= 1;
        }
            break;
        case VK_RIGHT: {
            if (chosenCell.j == 9)
                chosenCell.j = 0;
            else
                chosenCell.j += 1;
        }
            break;
        case VK_UP: {
            if (chosenCell.i == 0)
                chosenCell.i = 9;
            else
                chosenCell.i -= 1;
        }
            break;
        case VK_DOWN: {
            if (chosenCell.i == 9)
                chosenCell.i = 0;
            else
                chosenCell.i += 1;
        }
            break;
        case VK_SPACE: {
            ProceedTurn();
            break;
        }
    }

    chosenCell.cell = &pc.battlefield[chosenCell.i][chosenCell.j];
    repaint(MainWindowHandle);
    return TRUE;
}

void ProceedTurn(){
    if (turnIsPossible == FALSE)
        return;
//    srand(randomSeed);
    BOOL canMakeShot = TRUE;

    if (currentPlayer == &player){
        if (MakeShot(chosenCell.cell) == FALSE)
            currentPlayer = &pc;
    } else if (currentPlayer == &pc){
        while (canMakeShot == TRUE) {
            int ri = get_rand_range_int(0, 9);
            int rj = get_rand_range_int(0, 9);
            canMakeShot = MakeShot(&player.battlefield[ri][rj]);
        }
        currentPlayer = &player;
    }
}

BOOL MakeShot(struct Cell *cell){
    if (cell->cellState == Free) {
        cell->cellState = Missed;
        return FALSE;
    } else if (cell->cellState == Ship) {
        cell->cellState = Bombed;
        cell->player->shipDotsNumber-=1;
        if (CheckWinner() == TRUE)
            turnIsPossible = FALSE;
        return TRUE;
    } else if (cell->cellState == Missed){
        return TRUE;
    }
}

void InitPlayers(){
    player.isPlayer = TRUE;
    player.shipDotsNumber = SHIPS_DOTS_NUMBER;

    pc.isPlayer = FALSE;
    pc.shipDotsNumber = SHIPS_DOTS_NUMBER;

//    srand(randomSeed);
    int r = rand() % 1;
    if (r == 0)
        currentPlayer = &player;
    else
        currentPlayer = &pc;
}

BOOL CheckWinner(){
    if (player.shipDotsNumber == 0){
        MessageBoxA(MainWindowHandle, "Winner!", "PC won!", MB_OK);
        return TRUE;
    } else if (pc.shipDotsNumber == 0){
        MessageBoxA(MainWindowHandle, "Winner!", "Player won!", MB_OK);
        return TRUE;
    }
    return FALSE;
}

void Start(){

}

int get_rand_range_int(const int min, const int max) {
    return rand() % (max - min + 1) + min;
}

void repaint(HWND hWnd){
    RECT paintRect;
    GetClientRect(hWnd, &paintRect);
    InvalidateRect(hWnd, &paintRect, TRUE);
}

void placeRandomly(struct Cell battlefield[][10]){
    BOOL isPlaced = FALSE;
    int xi, xj;
    for (int i = 0; i < SHIPS_DOTS_NUMBER; ++i) {

        while (isPlaced == FALSE){
            xi = get_rand_range_int(0, 9);
            xj = get_rand_range_int(0, 9);
            if (battlefield[xi][xj].cellState == Free){
                battlefield[xi][xj].cellState = Ship;
                isPlaced = TRUE;
            }
        }
        isPlaced = FALSE;
    }
}