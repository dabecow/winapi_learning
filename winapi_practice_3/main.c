#ifndef UNICODE
#define UNICODE
#endif

#define CM_NEWMDIWND             24331
#define FM_SENDTEXT              24332
#define UPDATE                   24333
#define NOT_UPDATE               24334
#include <windows.h>
#include <winuser.h>
#include <commdlg.h>
#include <stdio.h>
#include "richedit.h"

HINSTANCE           hInstGlobal;
HWND                hwndMDIClient;

HWND                HEAD = NULL;
HWND                prevHwnd;
HWND                nextHwnd;
int                 num = 0;
wchar_t             szFrame[] = L"Main Window Frame Class";
wchar_t             szChild[] = L"MDI Window Child Class";

HWND createMdiChildWindow();
BOOL WINAPI InitializeApplication();

LRESULT CALLBACK FrameWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK MDIChildWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR lpCmdLine,
                     int nCmdShow){

    hInstGlobal = hInstance;
    
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

    wc.hInstance     = hInstGlobal;
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

    LPWSTR filePath = malloc(sizeof(WCHAR) * 256);
    HFILE hFile;
    LPSTR str = malloc(sizeof(CHAR) * 1024);
    DWORD bytesToRead;

    switch(uMsg){
        case WM_CREATE:
        {
            HMENU hmenuFile = CreateMenu();

            SetMenu(hWnd, hmenuFile);

            AppendMenu(hmenuFile, MF_ENABLED | MF_POPUP,
                       CM_NEWMDIWND, L"New MDI Window");

            DrawMenuBar(hWnd);

            CLIENTCREATESTRUCT ccs;

            ccs.hWindowMenu = GetSubMenu(GetMenu(hWnd), 1);
            ccs.idFirstChild = 1000;

            hwndMDIClient = CreateWindow(L"MDICLIENT", (LPCTSTR) NULL,
                                          WS_CHILD | WS_CLIPCHILDREN | WS_VSCROLL | WS_HSCROLL | WS_VISIBLE | MDIS_ALLCHILDSTYLES,
                                         0, 0, 0, 0, hWnd, 0, hInstGlobal, (LPSTR) &ccs);

            ShowWindow(hwndMDIClient, SW_SHOW);

        }
        break;

        case WM_COMMAND:{
            switch (wParam) {
                case CM_NEWMDIWND: {
                    if (FindWindowEx(hwndMDIClient, NULL, szChild, NULL) == NULL) {
                        OPENFILENAME ofn = {};
                        ofn.hInstance = hInstGlobal;
                        ofn.lStructSize = sizeof(OPENFILENAME);
                        ofn.hwndOwner = hwndMDIClient;
                        ofn.lpstrFile = filePath;
                        ofn.nMaxFile = sizeof(WCHAR) * 256;
                        ofn.lpstrFilter = L"all\0*.*\0";
                        ofn.nFilterIndex = 1;
                        ofn.lpstrFileTitle = NULL;
                        ofn.nMaxFileTitle = 0;
                        ofn.lpstrInitialDir = NULL;
                        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
                        ofn.lpstrTitle = L"Select file";
                        ZeroMemory(filePath, sizeof(filePath));
                        if (GetOpenFileNameW(&ofn) == TRUE) {
                            ZeroMemory(str, sizeof(str));
                            hFile = CreateFile(filePath, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,
                                               NULL);
                            ReadFile(hFile, str, 255, &bytesToRead, NULL);
                            str[bytesToRead] = '\0';

                            SetWindowLongPtr(hwndMDIClient, GWLP_USERDATA, (LONG_PTR) str);
                            CloseHandle(hFile);
                        } else {
                            break;
                        }
                    }
                    HWND mdiChildWindow = createMdiChildWindow();

                }
                    break;
            }
            }
        break;
//        case FM_SENDTEXT:{
//            for (int i = 0; i < num; i++) {
//                HWND hwndTemp = GetWindow(hwndMDIClient, GW_CHILD);
//                if (hwndTemp) {
//                    SendMessageA(hwndTemp, AWM_SENDTEXT, 0, 0);
//                }
//            }
//        }
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

    HWND textEdit;

    switch(uMsg){



        case WM_CREATE: {
            LoadLibrary(TEXT("riched32.dll"));

            textEdit = CreateWindow(RICHEDIT_CLASS, L"", WS_BORDER | WS_CHILD
                                                         | WS_VISIBLE | ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_MULTILINE, 100, 0, 300, 100,
                                    hWnd, NULL, hInstGlobal, NULL);

            SendMessage(textEdit, EM_SETEVENTMASK, 0, ENM_CHANGE);
            SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)textEdit);

        }
        break;

        case WM_COMMAND:

            switch(HIWORD(wParam)){

                case EN_CHANGE:
                {

                    textEdit = (HWND) GetWindowLongPtr(hWnd, GWLP_USERDATA);
                    LPSTR str = malloc(sizeof(CHAR) * 1024);


                    GetWindowTextA(textEdit, str, 1024);

                    SetWindowLongPtr(hwndMDIClient, GWLP_USERDATA, (LONG_PTR) str);
//                    SendMessageA(hwndMDIClient, WM_MDINEXT, (HWND) hWnd, 0);
                    SendMessageA(FindWindow(szFrame, NULL), FM_SENDTEXT, 0, 0);

                    HWND tmp = FindWindowEx(hwndMDIClient, hWnd,0, 0);
                    for (int i = 0; i < num-1; ++i) {
                        SendMessageA(tmp, UPDATE, NULL, NULL);
                        tmp = FindWindowEx(hwndMDIClient, tmp, NOT_UPDATE, NULL );
                    }

                }
                    break;
            }
            break;

        case WM_CHILDACTIVATE: {
            LPSTR str = (LPSTR) GetWindowLongPtr(hwndMDIClient, GWLP_USERDATA);
            textEdit = (HWND) GetWindowLongPtr(hWnd, GWLP_USERDATA);
            SetWindowTextA(textEdit, str);
        }
            break;
        case UPDATE:{
            LPSTR str = (LPSTR) GetWindowLongPtr(hwndMDIClient, GWLP_USERDATA);
            textEdit = (HWND) GetWindowLongPtr(hWnd, GWLP_USERDATA);
            SetWindowTextA(textEdit, str);
        }
        break;


        case WM_PAINT: {

            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            FillRect(hdc, &ps.rcPaint, (HBRUSH) (COLOR_WINDOW + 1));
            EndPaint(hWnd, &ps);
        }
        break;

        case WM_CLOSE:
            num--;
            SendMessage(hwndMDIClient, WM_MDIDESTROY, (WPARAM) hWnd, 1);
            break;

    }
    return DefMDIChildProc(hWnd, uMsg, wParam, lParam);
}

HWND createMdiChildWindow(){
    num++;

    MDICREATESTRUCT mcs;

    mcs.szClass = szChild;
    mcs.hOwner  = hInstGlobal;
    LPWSTR title[256];
    wsprintf(title, L"%hS:%d", L"MDI child", num);
    mcs.x = mcs.cx = CW_USEDEFAULT;
    mcs.y = mcs.cy = CW_USEDEFAULT;
    mcs.szTitle = (LPCWSTR) title;
    mcs.style = WS_CHILD | WS_OVERLAPPEDWINDOW | WS_VISIBLE;

    return (HWND) SendMessage (hwndMDIClient, WM_MDICREATE, 0,
                               (LONG) (LPMDICREATESTRUCT) &mcs);
}

