#ifndef UNICODE
#define UNICODE
#endif

#define WM_LBUTTONDOWN    0x0201

#include <windows.h>
#include <stdio.h>

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK childWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
    const wchar_t CLASS_NAME[]  = L"Sample Window Class";

    WNDCLASS wc = { };

    wc.lpfnWndProc   = WindowProc;
    wc.hInstance     = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    // Create the window.

    HWND hwnd = CreateWindowEx(
            0,                              // Optional window styles.
            CLASS_NAME,                     // Window class
            L"Learn to Program Windows",    // Window text
            WS_OVERLAPPEDWINDOW,            // Window style

            // Size and position
            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

            NULL,       // Parent window
            NULL,       // Menu
            hInstance,  // Instance handle
            NULL        // Additional application data
    );

    if (hwnd == NULL)
    {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);

    // Run the message loop.

    MSG msg = { };

    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_CLOSE:
            DestroyWindow(hwnd);
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_LBUTTONDOWN:
        {
            const wchar_t childClassName[] = L"Child Window Class";
            if (FindWindow(childClassName, NULL) != NULL) {
                DestroyWindow(FindWindow(childClassName, NULL));
            }

                WNDCLASS wc = {};

                wc.lpfnWndProc = childWindowProc;
                wc.hInstance = NULL;
                wc.lpszClassName = childClassName;

                RegisterClass(&wc);
                POINT p;
                GetCursorPos(&p);
                HWND hwndChild = CreateWindowEx(0, childClassName, L"Coordinates", WS_SIZEBOX | WS_CLIPSIBLINGS | WS_SYSMENU,
                                                p.x, p.y, 60, 60, hwnd, NULL, NULL, NULL);

                if (hwndChild == NULL) {
                    return 0;
                }

                ShowWindow(hwndChild, SW_SHOWNORMAL);

        }
            return 0;
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            FillRect(hdc, &ps.rcPaint, (HBRUSH) (COLOR_WINDOW+9));

            EndPaint(hwnd, &ps);
        }
            return 0;

    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK childWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
    switch (uMsg){
        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            POINT point;
            GetCursorPos(&point);

            char letter[256];
            sprintf(letter, "X = %ld, Y = %ld", point.x, point.y);
//            wchar_t unicodeLetter[256];
//            swprintf(unicodeLetter, 256, L"X = %ld, Y = %ld", point.x, point.y);
//            TextOut(hdc, CW_USEDEFAULT, CW_USEDEFAULT, unicodeLetter, 256);
            TextOutA(hdc, CW_USEDEFAULT, CW_USEDEFAULT, letter, 256);
            EndPaint(hwnd, &ps);
            ReleaseDC(hwnd, hdc);

            FillRect(hdc, &ps.rcPaint, (HBRUSH) (COLOR_WINDOW+1));

            EndPaint(hwnd, &ps);
        }
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}