#ifndef UNICODE
#define UNICODE
#endif

#define CM_NEWMDIWND             24331

#define CM_FILEOPEN            24332


#include <stdio.h>
#include "windows.h"


// Идентификатор меню верхнего уровня
HMENU hmenu;

// Идентификаторы временных меню
HMENU hmenuFile; // "File"
HWND hwndMDIClient;
HINSTANCE hInstance;

LRESULT CALLBACK FrameWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK ChildMdiWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

HWND createMdiChildWindow(){
//    const wchar_t mdiChildName[] = L"MDI child";
        HWND hwnd;
        MDICREATESTRUCT mcs;

        mcs.szClass = L"Mdi Client Area";
        mcs.hOwner  = hInstance;

        // Use the default size for the child window.

        mcs.x = mcs.cx = CW_USEDEFAULT;
        mcs.y = mcs.cy = CW_USEDEFAULT;


        mcs.style = CW_USEDEFAULT;

        // Tell the MDI client window to create the child window.

        hwnd = (HWND) SendMessage (hwndMDIClient, WM_MDICREATE, 0,
                                   (LONG) (LPMDICREATESTRUCT) &mcs);

        return hwnd;

}

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR lpCmdLine,
                     int nCmdShow){

    WNDCLASS wc = {};

    wc.lpfnWndProc = (WNDPROC) FrameWndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"MainWindow";

    wc.style         = 0;
    wc.lpfnWndProc   = (WNDPROC) FrameWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInstance;
//    wc.hIcon         = LoadIcon(hInstance, IDMULTIPAD);
    wc.hCursor       = LoadCursor((HANDLE) NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH) (COLOR_APPWORKSPACE + 1);
//    wc.lpszMenuName  = IDMULTIPAD;
//    wc.lpszClassName = szFrame;

    RegisterClass(&wc);

    wc.lpszClassName = L"Mdi Client Area";
    wc.lpfnWndProc   = (WNDPROC) ChildMdiWndProc;
//    wc.hIcon         = LoadIcon(hInstance, IDNOTE);
    wc.lpszMenuName  = (LPCTSTR) NULL;
//    wc.cbWndExtra    = CBWNDEXTRA;
//    wc.lpszClassName = szChild;

    RegisterClass(&wc);

    HWND hWnd = CreateWindowEx(
            0,
            L"MainWindow",
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

LRESULT CALLBACK FrameWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
    switch(uMsg){
        case WM_CREATE:
        {
            hmenu = CreateMenu();
            hmenuFile = CreatePopupMenu();

            // Подключаем меню к главному окну приложения
            SetMenu(hWnd, hmenu);
            // Добавляем временные меню к меню верхнего уровня
            AppendMenu(hmenu, MF_ENABLED | MF_POPUP,
                       (UINT)hmenuFile, L"File");

            // Создаем два временных меню - "File" и "Help"

            // Добавляем строки к меню "File"
            AppendMenu(hmenuFile, MF_ENABLED | MF_STRING,
                       CM_NEWMDIWND,    L"New MDI Window");

            // Добавляем разделительную линию
            AppendMenu(hmenuFile, MF_SEPARATOR, 0, NULL);

            AppendMenu(hmenuFile, MF_ENABLED | MF_STRING,
                       CM_FILEOPEN,   L"idk");

            DrawMenuBar(hWnd);

            // Перерисовываем меню

            CLIENTCREATESTRUCT ccs;

            // Retrieve the handle to the window menu and assign the
            // first child window identifier.

            ccs.hWindowMenu = GetSubMenu(GetMenu(hWnd), 1);
            ccs.idFirstChild = 1000;

            // Create the MDI client window.

            hwndMDIClient = CreateWindow( "MDICLIENT", (LPCTSTR) NULL,
                                          WS_CHILD | WS_CLIPCHILDREN | WS_VSCROLL | WS_HSCROLL,
                                          0, 0, 0, 0, hWnd, (HMENU) 0xCAC, hInstance, (LPSTR) &ccs);


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

LRESULT CALLBACK ChildMdiWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
    switch(uMsg){

    }
    return DefMDIChildProc(hWnd, uMsg, wParam, lParam);
}