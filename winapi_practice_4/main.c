#ifndef UNICODE
#define UNICODE
#endif

#include <stdio.h>
#include <windowsx.h>
#include "windows.h"
#include "FigureList.h"
#define DC_SWITCHCOLOR 0x1337
#define DF_CHOOSERECT 0x1338
#define DF_CHOOSECIRCLE 0x1339
#define DF_CHOOSELINE 0x1340
#define DF_RESET 0x1341

const wchar_t   CLASS_NAME[] = L"MainWindow";
enum FigureType currentFigureType;
struct Node*    currentNode;
HINSTANCE       currentHInstance;
HINSTANCE       dllFile;
boolean         mouseIsDown = FALSE;
HBRUSH          hbrush;
HPEN            hpen;
POINT           p;

BOOL WINAPI InitializeApplication();
void WINAPI createMenu(HWND hWnd);
LRESULT CALLBACK winProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void (*dllFunc) ();
typedef BOOL (*GetColor)(HWND, COLORREF*);

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR     lpCmdLine,
                   int       nCmdShow){

    currentHInstance = hInstance;

    if (!InitializeApplication()){
        printf("An error occurred");
        return 0;
    }

    HWND hWnd = CreateWindowEx(
            0,
            CLASS_NAME,
            L"Paint",
            WS_OVERLAPPEDWINDOW,

            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

            NULL,
            NULL,
            hInstance,
            NULL
    );


    if (hWnd == NULL){
        printf("An error occurred: %lu", GetLastError());
        return 0;
    }

    createMenu(hWnd);

    ShowWindow(hWnd, nCmdShow);

    MSG msg = { };

    while (GetMessage(&msg, NULL, 0, 0))
    {
        if(!TranslateMDISysAccel(hWnd, &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return 0;
}

BOOL WINAPI InitializeApplication()
{
    WNDCLASS wc = {};

    wc.lpfnWndProc   = (WNDPROC) winProc;
    wc.hInstance     = currentHInstance;
    wc.hbrBackground = (HBRUSH) (COLOR_APPWORKSPACE + 1);
    wc.lpszClassName = CLASS_NAME;

    hpen = CreatePen(PS_SOLID, 10, RGB(0,0,0));
    hbrush = CreateSolidBrush(GetSysColor(COLOR_ACTIVECAPTION));
    if (!RegisterClass (&wc) )
        return FALSE;

    return TRUE;
}

LRESULT CALLBACK winProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
//    HDC hdc;

    PAINTSTRUCT ps;
    HDC hdc, hCmpDC;
    HBITMAP hBmp;

    switch (uMsg) {
        case WM_COMMAND:
        {
            switch (wParam) {
                case DC_SWITCHCOLOR:{
                    COLORREF clrref;
                    GetColor getcolor;
                    getcolor = (GetColor) GetProcAddress(dllFile, "GetColor");

                    if(getcolor(hWnd, &clrref))
                    {
                        hpen  = CreatePen(PS_SOLID, 10, clrref);
                        hbrush = CreateSolidBrush(clrref);

                    }

                    return 0;
                }
                case DF_CHOOSECIRCLE:
                    currentFigureType = isCircle;
                    break;
                case DF_CHOOSELINE:
                    currentFigureType = isLine;
                    break;
                case DF_CHOOSERECT:
                    currentFigureType = isRectangle;
                    break;
                case DF_RESET: {
                    ClearList();
                    RECT paintRect;
                    GetClientRect(hWnd, &paintRect);
                    InvalidateRect(hWnd, &paintRect, FALSE);
                }
                break;

            }
        }
        break;
        case WM_CREATE:
        {
            currentNode = NULL;
            dllFile = LoadLibrary(L"library.dll");
            hpen  = CreatePen(PS_SOLID, 10, RGB(0,0,0));
            hbrush = CreateSolidBrush(GetSysColor(COLOR_ACTIVECAPTION));
            return 0;
        }
        case WM_CLOSE:
            DestroyWindow(hWnd);
        case WM_ERASEBKGND:
            return 1;
        case WM_PAINT:
        {
            RECT clientRect;
            GetClientRect(hWnd, &clientRect);

            hdc = BeginPaint(hWnd, &ps);

            hCmpDC = CreateCompatibleDC(hdc);
            hBmp = CreateCompatibleBitmap(hdc, clientRect.right, clientRect.bottom);
            SelectObject(hCmpDC, hBmp);

            int saveDC = SaveDC(hCmpDC);

            FillRect(hCmpDC, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

            struct Node* node = HEAD;

            while(node != NULL){
                switch (node->figure.figureType){
                    case isRectangle: {
                        dllFunc = (void (*)(HDC, HBRUSH, HPEN, RECT)) GetProcAddress(dllFile, "DrawRectangle");
                        dllFunc(hCmpDC, node->figure.brush, node->figure.pen, node->figure.positionRect);
                    }
                    break;
                    case isCircle: {
                        dllFunc = (void(*)(HDC, HBRUSH, HPEN, RECT)) GetProcAddress(dllFile, "DrawCircle");
                        dllFunc(hCmpDC, node->figure.brush, node->figure.pen, node->figure.positionRect);
                    }
                    break;
                    case isLine: {
                        dllFunc = (void(*)(HDC, HPEN, RECT)) GetProcAddress(dllFile, "DrawLine");
                        dllFunc(hCmpDC, node->figure.pen, node->figure.positionRect);
                    }
                    break;
                }
                node = node->next;
            }

// Копируем изображение из теневого контекста на экран
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
        case WM_LBUTTONDOWN: {

            mouseIsDown = TRUE;

            p.x = GET_X_LPARAM(lParam);
            p.y = GET_Y_LPARAM(lParam);
            struct Figure figure;
            figure.brush = hbrush;
            figure.pen = hpen;

            figure.positionRect.top = p.y;
            figure.positionRect.left = p.x;
            figure.positionRect.bottom = p.y;
            figure.positionRect.right = p.x;
            figure.figureType = currentFigureType;

            currentNode = AddNode(figure);
        }
            break;
        case WM_MOUSEMOVE:
        {
            if (mouseIsDown) {

                p.x = GET_X_LPARAM(lParam);
                p.y = GET_Y_LPARAM(lParam);

                currentNode->figure.positionRect.right = p.x;
                currentNode->figure.positionRect.bottom = p.y;

                RECT paintRect;
                GetClientRect(hWnd, &paintRect);
                InvalidateRect(hWnd, &paintRect, TRUE);
            }
        }
            break;

        case WM_LBUTTONUP:
        {
            mouseIsDown = FALSE;
            p.x = GET_X_LPARAM(lParam);
            p.y = GET_Y_LPARAM(lParam);

            currentNode = NULL;
        }
            break;

        case WM_DESTROY:
        {
            // Удаляем созданные нами кисть и перо
            DeleteBrush(hbrush);
            DeletePen(hpen);
            FreeLibrary(dllFile);
            PostQuitMessage(0);
        }

        default:
            break;
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void WINAPI createMenu(HWND hWnd){
    HMENU hmenu = CreateMenu();
    HMENU hmenuFile = CreatePopupMenu();

    SetMenu(hWnd, hmenu);

    AppendMenu(hmenu, MF_ENABLED | MF_POPUP,
                       (UINT)hmenuFile, L"File");

    AppendMenu(hmenuFile, MF_ENABLED | MF_STRING,
                       DF_CHOOSELINE,    L"New Line");

    AppendMenu(hmenuFile, MF_ENABLED | MF_STRING,
               DF_CHOOSERECT,    L"New Rectangle");
    AppendMenu(hmenuFile, MF_ENABLED | MF_STRING,
               DF_CHOOSECIRCLE,    L"New Circle");
    AppendMenu(hmenuFile, MF_ENABLED | MF_STRING,
               DC_SWITCHCOLOR,    L"Change color");
    AppendMenu(hmenuFile, MF_ENABLED | MF_STRING,
               DF_RESET,    L"Reset");
    DrawMenuBar(hWnd);

}