#include <stdio.h>
#include <windows.h>

#define export __declspec(dllexport)

export void PrintHello(){
    printf("Hello, world!");
}

export void DrawRectangle(HDC hdc, HBRUSH brush, HPEN pen, RECT rect){
    if (hdc != NULL){
        SelectObject(hdc, brush);
        SelectObject(hdc, pen);
        Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);
    }
}

export void DrawCircle(HDC hdc, HBRUSH brush, HPEN pen, RECT rect){
    if (hdc != NULL){
        SelectObject(hdc, brush);
        SelectObject(hdc, pen);
        Ellipse(hdc, rect.left, rect.top, rect.right, rect.bottom);
    }
}

export void DrawLine(HDC hdc, HPEN pen, RECT rect){
    if (hdc != NULL){
        SelectObject(hdc, pen);
        MoveToEx(hdc, rect.left, rect.top, NULL);
        LineTo(hdc, rect.right, rect.bottom);
    }
}

export BOOL GetColor(HWND hwnd, COLORREF *clrref)
{
    CHOOSECOLOR cc;
    COLORREF colorRefs[16];

    // Подготавливаем массив цветов
    // "Custom Colors"
    for (int i = 0; i < 16; i++)
        colorRefs[i] = RGB(255, 255, 255);

    // Записываем нулевые значения во все
    // неиспользуемые поля структуры CHOOSECOLOR
    memset(&cc, 0, sizeof(CHOOSECOLOR));

    // Заполняем необходимые поля
    cc.lStructSize = sizeof(CHOOSECOLOR);
    cc.hwndOwner = hwnd;
    cc.rgbResult = RGB(0, 0, 0);
    cc.lpCustColors = colorRefs;
    cc.Flags = 0;

    // Выбираем цвет и возвращаем результат
    if (ChooseColor(&cc))
    {
        *clrref = cc.rgbResult;
        return TRUE;
    }
    else
        return FALSE;
}