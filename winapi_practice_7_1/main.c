#ifndef UNICODE
#define UNICODE
#endif

#define BUTTON_1_DOWN 2228
#define BUTTON_2_DOWN 1337

#include <uxtheme.h>
#include "windows.h"
#include "stdio.h"

CHAR displayedString[10];
LPSTR string;
INT stringLength;
INT maxStringLength;
INT currentPosition;
INT offset;

INT frame;
HINSTANCE currentHInstance;
LPHANDLE graphicsThread;
LPHANDLE textThread;

HWND MainWindowHandle;
HWND statusBar;
HBITMAP hBitMap;
LPWSTR CLASS_NAME = L"MainWindow";

BOOL isGraphicsThreadActive;
BOOL isTextThreadActive;

_Noreturn DWORD WINAPI GraphicsThreadFunc(LPVOID LpParametr);

_Noreturn DWORD WINAPI WritingThreadFunc(LPVOID LpParametr);

void changeThreadState(LPHANDLE thread, BOOL *state);

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
            L"Paint",
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
        case WM_COMMAND: {
            switch (wParam) {
                case BUTTON_1_DOWN:
                    changeThreadState(graphicsThread, &isGraphicsThreadActive);
                    break;
                case BUTTON_2_DOWN:
                    changeThreadState(textThread, &isTextThreadActive);
                    break;
            }
        }
            break;
        case WM_CREATE: {

            HWND firstButtonHandle = CreateWindow(L"button", L"First Thread",
                                             WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 0, 0, 200, 50,
                                             hWnd, BUTTON_1_DOWN, currentHInstance, NULL);
            HWND secondButtonHandle = CreateWindow(L"button", L"Second Thread",
                                              WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 0, 100, 200, 50,
                                              hWnd, BUTTON_2_DOWN, currentHInstance, NULL);

            frame = 0;
            hBitMap = NULL;

            graphicsThread = CreateThread(NULL, 0, GraphicsThreadFunc, NULL, 0, NULL);
            textThread = CreateThread(NULL, 0, WritingThreadFunc, NULL, 0, NULL);

            isGraphicsThreadActive = TRUE;
            isTextThreadActive = TRUE;

            maxStringLength = 19;
            stringLength = 10;
            currentPosition = 0;
            offset = 0;
            string = malloc(maxStringLength * sizeof(CHAR));
            string = "I'm running string!";
            statusBar = CreateWindow(STATUSCLASSNAME, L"String", WS_VISIBLE | WS_CHILD,
                                     10, 100, 120, 20, hWnd, NULL, currentHInstance, NULL);            // no window creation data

        }
            break;

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            HDC memDC;
            BITMAP bm;

            FillRect(memDC, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 2));
            memDC = CreateCompatibleDC(hdc);

            if (hBitMap != NULL) {
                GetObject(hBitMap, sizeof(bm), &bm);
                SelectObject(memDC, hBitMap);
                BitBlt(hdc, 200, 200, bm.bmWidth, bm.bmHeight, memDC, 0, 0, SRCCOPY);
            }

            DeleteDC(memDC);
            EndPaint(hWnd, &ps);
        }
            break;

        case WM_CLOSE:
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);

}

DWORD WINAPI GraphicsThreadFunc(LPVOID LpParametr) {
    while (TRUE) {
        switch (frame) {
            case 0:
                hBitMap = (HBITMAP) LoadImageW(currentHInstance, L".\\images\\0001.bmp",
                                               IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
                break;
            case 1:
                hBitMap = (HBITMAP) LoadImageW(currentHInstance, L".\\images\\0002.bmp",
                                               IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
                break;
            case 2:
                hBitMap = (HBITMAP) LoadImageW(currentHInstance, L".\\images\\0003.bmp",
                                               IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
                break;
            case 3:
                hBitMap = (HBITMAP) LoadImageW(currentHInstance, L".\\images\\0004.bmp",
                                               IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
                break;
            case 4:
                hBitMap = (HBITMAP) LoadImageW(currentHInstance, L".\\images\\0005.bmp",
                                               IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
                break;
        }

        frame++;

        if (frame == 5)
            frame = 0;

        RECT paintRect;
        GetClientRect(MainWindowHandle, &paintRect);
        InvalidateRect(MainWindowHandle, &paintRect, FALSE);
        Sleep(75);
    }
}

_Noreturn DWORD WINAPI WritingThreadFunc(LPVOID LpParametr){
    while(TRUE){
        strncpy(displayedString, "                                                  ", stringLength);
        if(currentPosition + stringLength - 1 < maxStringLength){
            strncpy(displayedString, string + currentPosition, stringLength);
        } else {
            offset++;
            strncpy(displayedString, string + currentPosition, stringLength - offset);
            strncpy(displayedString + stringLength - offset, string, offset);
        }

        if(currentPosition + 1 < maxStringLength)
            currentPosition++;
        else{
            currentPosition = 0;
            offset = 0;
        }

        SetWindowTextA(statusBar, displayedString);
        Sleep(300);
    }
}

void changeThreadState(LPHANDLE thread, BOOL *state) {
    if (*state == TRUE) {
        *state = FALSE;
        SuspendThread(thread);
    } else {
        *state = TRUE;
        ResumeThread(thread);
    }
}