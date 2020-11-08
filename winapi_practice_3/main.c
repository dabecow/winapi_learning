#ifndef UNICODE
#define UNICODE
#endif

#define CM_NEWMDIWND             24331

#define CM_FILEOPEN            24332


#include <stdio.h>
#include "windows.h"


// Идентификатор меню верхнего уровня
//HMENU               hmenu, hmenuFile;

HWND                hwndMDIClient;

wchar_t             szFrame[] = L"Main Window Frame Class";
wchar_t             szChild[] = L"MDI Window Child Class";

BOOL WINAPI InitializeApplication();
LRESULT CALLBACK FrameWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK MDIChildWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
HWND createMdiChildWindow();

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR lpCmdLine,
                     int nCmdShow){

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

    wc.style         = 0;
    wc.lpfnWndProc   = (WNDPROC) FrameWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = GetModuleHandle(NULL);
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
            HMENU hmenuFile = CreatePopupMenu();

            SetMenu(hWnd, hmenu);

            AppendMenu(hmenu, MF_ENABLED | MF_POPUP,
                       (UINT)hmenuFile, L"File");

            AppendMenu(hmenuFile, MF_ENABLED | MF_STRING,
                       CM_NEWMDIWND,    L"New MDI Window");

            DrawMenuBar(hWnd);

            CLIENTCREATESTRUCT ccs;

            ccs.hWindowMenu = GetSubMenu(GetMenu(hWnd), 1);
            ccs.idFirstChild = 1000;

            hwndMDIClient = CreateWindow( L"MDICLIENT", (LPCTSTR) NULL,
                                          WS_CHILD | WS_CLIPCHILDREN | WS_VSCROLL | WS_HSCROLL,
                                          0, 0, 0, 0, hWnd, (HMENU) 0xCAC, GetModuleHandle(NULL), (LPSTR) &ccs);

            ShowWindow(hwndMDIClient, SW_SHOW);

        }
        break;

        case WM_COMMAND:{
            switch (wParam) {
                case CM_NEWMDIWND: {
                    HWND mdiChildWindow = createMdiChildWindow();

                    ShowWindow(mdiChildWindow, SW_SHOW);
                }
                    break;
                default:{
                    MessageBox(hWnd, L"Функция не реализована",
                               NULL, MB_OK);
                    return 0;
                }

            }
        }
        break;
        case WM_CLOSE:
            DestroyWindow(hWnd);
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
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

    }
    return DefMDIChildProc(hWnd, uMsg, wParam, lParam);
}

HWND createMdiChildWindow(){

    MDICREATESTRUCT mcs;

    mcs.szClass = L"MDICLIENT";
    mcs.hOwner  = GetModuleHandle(NULL);
    mcs.szTitle = L"MDI child window";

    mcs.x = mcs.cx = CW_USEDEFAULT;
    mcs.y = mcs.cy = CW_USEDEFAULT;

    mcs.style = WS_CHILD | WS_OVERLAPPEDWINDOW | WS_VISIBLE;

    return (HWND) SendMessage (hwndMDIClient, WM_MDICREATE, 0,
                               (LONG) (LPMDICREATESTRUCT) &mcs);
}

