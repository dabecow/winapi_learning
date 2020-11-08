#ifndef UNICODE
#define UNICODE
#endif

#define CM_NEWMDIWND             24331

#include <stdio.h>
#include "windows.h"

HINSTANCE           hinstance;
HWND                hwndMDIClient;
HWND                array[10];

wchar_t             szFrame[] = L"Main Window Frame Class";
wchar_t             szChild[] = L"MDI Window Child Class";

BOOL addHwndToArr(HWND hWnd){
    for (int i = 0; i < 10; ++i) {
        if (array[i] == 0){
            array[i] = hWnd;
            return TRUE;
        }
    }

    SendMessage(hwndMDIClient, WM_MDIDESTROY, (HWND) hWnd, 0);
    return FALSE;
}

BOOL delHwndFromArr(HWND hwnd){
    for (int i = 0; i < 10; ++i) {
        if (array[i] == hwnd){
            array[i] = 0;
            SendMessage(hwndMDIClient, WM_MDIDESTROY, hwnd, 1);

            return TRUE;
        }
    }
    return FALSE;
}

HWND createMdiChildWindow();
BOOL WINAPI InitializeApplication();

LRESULT CALLBACK FrameWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK MDIChildWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR lpCmdLine,
                     int nCmdShow){

    hinstance = hInstance;
    
    if (!InitializeApplication()){
        printf("An error occurred");
        return 0;
    }

    HWND hWnd = CreateWindowEx(
            0,
            szFrame,
            L"MDI learning",
            WS_OVERLAPPEDWINDOW,

            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

            NULL,
            NULL,
            hInstance,
            NULL
    );

    if (hWnd == NULL){
        printf("An error occurred");
        return 0;
    }

    ShowWindow(hWnd, nCmdShow);

    MSG msg = { };

    while (GetMessage(&msg, NULL, 0, 0))
    {
        if(!TranslateMDISysAccel(hwndMDIClient, &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return 0;
}

BOOL WINAPI InitializeApplication()
{
    WNDCLASS wc = {};

    // Register the frame window class.

    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc   = (WNDPROC) FrameWndProc;

    wc.hInstance     = hinstance;
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH) (COLOR_APPWORKSPACE + 1);
    wc.lpszClassName = szFrame;

    if (!RegisterClass (&wc) )
        return FALSE;

    // Register the MDI child window class.

    wc.lpfnWndProc   = (WNDPROC) MDIChildWndProc;
    wc.lpszMenuName  = (LPCTSTR) NULL;
    wc.lpszClassName = szChild;

    if (!RegisterClass(&wc))
        return FALSE;
    return TRUE;
}

LRESULT CALLBACK FrameWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam){

    switch(uMsg){
        case WM_CREATE:
        {
            HMENU hmenu = CreateMenu();

            SetMenu(hWnd, hmenu);

            AppendMenu(hmenu, MF_ENABLED | MF_POPUP,
                       CM_NEWMDIWND, L"New MDI Window");

            DrawMenuBar(hWnd);

            CLIENTCREATESTRUCT ccs;

            ccs.hWindowMenu = GetSubMenu(GetMenu(hWnd), 1);
            ccs.idFirstChild = 1000;

            hwndMDIClient = CreateWindow( L"MDICLIENT", (LPCTSTR) NULL,
                                          WS_CHILD | WS_CLIPCHILDREN | WS_VSCROLL | WS_HSCROLL | WS_VISIBLE | MDIS_ALLCHILDSTYLES,
                                          0, 0, 0, 0, hWnd, 0, hinstance, (LPSTR) &ccs);

            ShowWindow(hwndMDIClient, SW_SHOW);

        }
        break;

        case WM_COMMAND:{
            switch (wParam) {
                case CM_NEWMDIWND: {
                    HWND mdiChildWindow = createMdiChildWindow();
                    if (addHwndToArr(mdiChildWindow))
                        ShowWindow(mdiChildWindow, SW_SHOW);
                    else MessageBox(hWnd, L"You can't create more MDI windows.", NULL, MB_OK);
                }
                    break;

            }
        }
        break;
        case WM_CLOSE:
            DestroyWindow(hWnd);
        case WM_DESTROY:
            PostQuitMessage(0);
        case WM_PAINT:
        {

            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);


            EndPaint(hWnd, &ps);
        }
            return 0;
    }

    return DefFrameProc(hWnd, hwndMDIClient, uMsg, wParam, lParam);
}

LRESULT CALLBACK MDIChildWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam){

    switch(uMsg){

        case WM_CREATE:
            break;
        case WM_CHILDACTIVATE:
            break;
        case WM_PAINT: {

            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            FillRect(hdc, &ps.rcPaint, (HBRUSH) (COLOR_WINDOW + 1));
            EndPaint(hWnd, &ps);
        }
        break;
        case WM_CLOSE:
            delHwndFromArr(hWnd);
            SendMessage(hwndMDIClient, WM_MDIDESTROY, hWnd, 1);
            break;

    }
    return DefMDIChildProc(hWnd, uMsg, wParam, lParam);
}

HWND createMdiChildWindow(){

    MDICREATESTRUCT mcs;

    mcs.szClass = szChild;
    mcs.hOwner  = hinstance;
    mcs.szTitle = L"MDI child window";

    mcs.x = mcs.cx = CW_USEDEFAULT;
    mcs.y = mcs.cy = CW_USEDEFAULT;

    mcs.style = WS_CHILD | WS_OVERLAPPEDWINDOW | WS_VISIBLE;

    return (HWND) SendMessage (hwndMDIClient, WM_MDICREATE, 0,
                               (LONG) (LPMDICREATESTRUCT) &mcs);
}

