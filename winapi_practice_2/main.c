#ifndef UNICODE
#define UNICODE
#endif

#define WM_LBUTTONDOWN    0x0201
#define WM_RBUTTONDOWN    0x0204
#define WM_MOUSEWHEEL                   0x020A

#include <windows.h>
#include <stdio.h>

LRESULT CALLBACK winProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void showError(DWORD errorCode);

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR lpCmdLine,
                     int nCmdShow)
                     {

    const wchar_t CLASS_NAME[] = L"Error window class";

    WNDCLASS wc = { };

    wc.lpfnWndProc = winProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    HWND hWnd = CreateWindowEx(
            0,
            CLASS_NAME,
            L"Error messages",
            WS_CLIPSIBLINGS | WS_SYSMENU,

            CW_USEDEFAULT, CW_USEDEFAULT, 350, 250,

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
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

LRESULT CALLBACK winProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
    switch(uMsg){
        case WM_LBUTTONDOWN: {
            WNDCLASS fail = {};
            RegisterClass(&fail);
            showError(GetLastError());
            break;
        }
        case WM_RBUTTONDOWN: {
            HWND hErrorWnd = CreateWindowEx(
                    0,
                    L"Unregistered class",
                    L"Error messages",
                    WS_OVERLAPPEDWINDOW,

                    CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

                    NULL,
                    NULL,
                    NULL,
                    NULL
            );
            showError(GetLastError());
            break;
        }
        case WM_MOUSEWHEEL: {
            PAINTSTRUCT ps;
            HDC fakeHdc;
            FillRect(fakeHdc, &ps.rcPaint, (HBRUSH) (COLOR_WINDOW + 9));
            showError(GetLastError());
            break;
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
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void showError(DWORD errorCode){

    DWORD englishId = MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US);
    DWORD russianId = MAKELANGID(LANG_RUSSIAN, SUBLANG_RUSSIAN_RUSSIA);

    LPWSTR  sysLangMsg,
            nationalLangMsg;

    wchar_t finalMsg[256];
    wchar_t errorCodeString[20];

    FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER, 0, errorCode, LANG_SYSTEM_DEFAULT, &sysLangMsg, 0, NULL);
    FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER, 0, errorCode, russianId, &nationalLangMsg, 0, NULL);

    wsprintf(finalMsg,  L"%s\n%s", sysLangMsg, nationalLangMsg);
    wsprintf(errorCodeString, L"Error code: %d", errorCode);

    MessageBoxW(NULL,
                finalMsg,
                errorCodeString,
                MB_OK | MB_ICONERROR);
}