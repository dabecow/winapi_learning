#ifndef UNICODE
#define UNICODE
#endif

#define WM_LBUTTONDOWN    0x0201
#define WM_RBUTTONDOWN    0x0204

#include <windows.h>
#include <stdio.h>

struct Errors{
    DWORD regErr;
    DWORD wndErr;
    DWORD usrErr;
} ;

struct Errors errors;

LRESULT CALLBACK winProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

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

    WNDCLASS fail = { };
    RegisterClass(&fail);
    errors.regErr = GetLastError();

    HWND hErrorWnd = CreateWindowEx(
                                 0,
                                 L"Unregistered class",
                                 L"Error messages",
                                 WS_OVERLAPPEDWINDOW,

                                 CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

                                 NULL,
                                 NULL,
                                 hInstance,
                                 NULL
                                 );

    errors.wndErr = GetLastError();

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
        case WM_CLOSE:
            DestroyWindow(hWnd);
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_PAINT:
        {

            PAINTSTRUCT ps;
            HDC fakeHdc;
            FillRect(fakeHdc, &ps.rcPaint, (HBRUSH) (COLOR_WINDOW+9));
            errors.usrErr = GetLastError();
            HDC hdc = BeginPaint(hWnd, &ps);

            LPTSTR  regErrStrEn, regErrStrRu,
                    wndErrStrEn, wndErrStrRu,
                    usrErrStrEn, usrErrStrRu;

            FillRect(hdc, &ps.rcPaint, (HBRUSH) (COLOR_WINDOW+9));

            FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER, 0, errors.regErr, LANG_SYSTEM_DEFAULT, &regErrStrRu, 0, NULL);
            FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER, 0, errors.wndErr, LANG_SYSTEM_DEFAULT, &wndErrStrRu, 0, NULL);
            FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER, 0, errors.usrErr, LANG_SYSTEM_DEFAULT, &usrErrStrRu, 0, NULL);

            DWORD russian = MAKELANGID(LANG_RUSSIAN, SUBLANG_RUSSIAN_RUS);

            FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER, 0, errors.regErr, english, &regErrStrEn, 0, NULL);
            FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER, 0, errors.wndErr, english, &wndErrStrEn, 0, NULL);
            FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER, 0, errors.usrErr, english, &usrErrStrEn, 0, NULL);

            LPWSTR kostyl1[228], kostyl2[228], kostyl3[228];

            wsprintf(kostyl1, L"Ошибка регистрации класса окон: %d", errors.regErr);
            wsprintf(kostyl2, L"Ошибка создания окна приложения: %d", errors.wndErr);
            wsprintf(kostyl3, L"Пользовательская ошибка: %d", errors.usrErr);

            TextOutW(hdc, CW_USEDEFAULT, CW_USEDEFAULT, kostyl1, wcslen(kostyl1));
            TextOutA(hdc, CW_USEDEFAULT, 20, regErrStrRu, strlen(regErrStrRu));
            TextOutA(hdc, CW_USEDEFAULT, 40, regErrStrEn, strlen(regErrStrEn));

            TextOutW(hdc, CW_USEDEFAULT, 70, kostyl2, wcslen(kostyl2));
            TextOutA(hdc, CW_USEDEFAULT, 90, wndErrStrRu, strlen(wndErrStrRu));
            TextOutA(hdc, CW_USEDEFAULT, 110, wndErrStrEn, strlen(wndErrStrEn));

            TextOutW(hdc, CW_USEDEFAULT, 140, kostyl3, wcslen(kostyl3));
            TextOutA(hdc, CW_USEDEFAULT, 160, usrErrStrRu, strlen(usrErrStrRu));
            TextOutA(hdc, CW_USEDEFAULT, 180, usrErrStrEn, strlen(usrErrStrEn));


            EndPaint(hWnd, &ps);
        }
            return 0;
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}