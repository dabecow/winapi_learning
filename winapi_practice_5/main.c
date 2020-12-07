#ifndef UNICODE
#define UNICODE
#endif

#include <stdio.h>
#include <windowsx.h>
#include "windows.h"
#include "FigureList.h"
#include "drawer.h"

#define DC_SWITCHCOLOR 0x1337
#define DF_RESET 0x1341

struct LastLine{
    POINT p1;
    POINT p2;
    BOOL allow;
};

const wchar_t   CLASS_NAME[] = L"MainWindow";

HINSTANCE       currentHInstance;

boolean         mouseIsDown = FALSE;

HBRUSH          hbrush;
HPEN            hpen;

POINT           p;

struct FigureNode*     FIGURES_HEAD;


struct Figure*         currentFigure;
struct DotNode         *currentDotNode;
struct LastLine        lastLine;

BOOL WINAPI InitializeApplication();
void WINAPI createMenu(HWND hWnd);

LRESULT CALLBACK winProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

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
            WS_OVERLAPPEDWINDOW | WS_VISIBLE,

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
    wc.hCursor = LoadCursor(NULL, IDC_CROSS);

    hpen = CreatePen(PS_SOLID, 10, RGB(0,0,0));
    hbrush = CreateSolidBrush(GetSysColor(COLOR_ACTIVECAPTION));
    if (!RegisterClass (&wc) )
        return FALSE;

    return TRUE;
}

LRESULT CALLBACK winProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam){

    PAINTSTRUCT ps;
    HDC hdc, hCmpDC;
    HBITMAP hBmp;

    switch (uMsg) {
        case WM_COMMAND:
        {
            switch (wParam) {
                case DC_SWITCHCOLOR:{
                    COLORREF clrref;

                    if(GetColor(hWnd, &clrref))
                    {
                        hpen  = CreatePen(PS_SOLID, 10, clrref);
                        hbrush = CreateSolidBrush(clrref);

                    }

                    return 0;
                }

                case DF_RESET: {
                    ClearList();
                    RECT paintRect;
                    GetClientRect(hWnd, &paintRect);
                    InvalidateRect(hWnd, &paintRect, FALSE);
                    currentFigure = NULL;

                }
                    break;

            }
        }
            break;

        case WM_CREATE:
        {
            createMenu(hWnd);
            currentFigure = NULL;
            currentDotNode = NULL;
            FIGURES_HEAD = NULL;
            hpen  = CreatePen(PS_SOLID, 10, RGB(255,0,0));

//            hbrush = CreateSolidBrush(GetSysColor(COLOR_ACTIVECAPTION));
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

            struct FigureNode* node = FIGURES_HEAD;

            while(node != NULL){

                DrawFigure(&node->figure, hCmpDC, node->figure.pen);

                node = node->next;
            }
            if (currentFigure != NULL)
                DrawFigure(currentFigure, hCmpDC, currentFigure->pen);

            SetStretchBltMode(hdc, COLORONCOLOR);
            BitBlt(hdc, 0, 0, clientRect.right, clientRect.bottom,
                   hCmpDC, 0, 0, SRCCOPY);
            RestoreDC(hCmpDC, saveDC);

            DeleteDC(hCmpDC);
            DeleteObject(hBmp);
            hCmpDC = NULL;



            EndPaint(hWnd, &ps);
        }
            break;
        case WM_LBUTTONDOWN: {

            mouseIsDown = TRUE;
            if (currentFigure == NULL){

                p.x = GET_X_LPARAM(lParam);
                p.y = GET_Y_LPARAM(lParam);

                currentFigure = CreateFigure(p, hpen);
                currentDotNode = AddDot(p);


            } else {
                struct DotNode* node = GetLastDot();
                if(IsVectorIntersect(node, currentDotNode) == TRUE){
//                    currentDotNode->allow = 0;
                    MessageBox(NULL, L"Dots error", L"Error!", MB_OK | MB_ICONERROR);
//                    setup->currentNode = NULL;
                } else {

                    p.x = GET_X_LPARAM(lParam);
                    p.y = GET_Y_LPARAM(lParam);
                    currentDotNode->point = p;
                    currentDotNode = AddDot(p);
                }
            }

        }
            break;
        case WM_MOUSEMOVE:
        {
            if (mouseIsDown) {

                p.x = GET_X_LPARAM(lParam);
                p.y = GET_Y_LPARAM(lParam);

                currentDotNode->point = p;;

                RECT paintRect;
                GetClientRect(hWnd, &paintRect);
                InvalidateRect(hWnd, &paintRect, TRUE);

            }
        }
            break;

        case WM_RBUTTONDOWN:{
            if (currentFigure == NULL || currentFigure->dotsNumber < 3){
                MessageBoxA(NULL, "The figure is a dot or a line", "Error", MB_ICONSTOP | MB_OK);
                return 0;
            }
            if (mouseIsDown) {
                mouseIsDown = FALSE;
                FinishFigure();
                AddFigure();

                currentFigure = NULL;

                RECT paintRect;
                GetClientRect(hWnd, &paintRect);
                InvalidateRect(hWnd, &paintRect, TRUE);
            }
        }
        break;

        case WM_DESTROY:
        {
            // Удаляем созданные нами кисть и перо
            DeleteBrush(hbrush);
            DeletePen(hpen);
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
               DC_SWITCHCOLOR,    L"Change color");
    AppendMenu(hmenuFile, MF_ENABLED | MF_STRING,
               DF_RESET,    L"Reset");
    DrawMenuBar(hWnd);

}