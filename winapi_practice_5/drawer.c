
#include "drawer.h"

void DrawLine(HDC hdc, HPEN pen, POINT p1, POINT p2){
    if (hdc != NULL){
        SelectObject(hdc, pen);
        MoveToEx(hdc, p1.x, p1.y, NULL);
        LineTo(hdc, p2.x, p2.y);
    }
}

void DrawFigure(struct Figure *figure, HDC hdc, HPEN pen){
    struct DotNode* node = figure->DOTS_HEAD;
    if (node == NULL)
        return;

    POINT startPoint = node->point;

    while (node->next != NULL){
        DrawLine(hdc, pen, startPoint, node->next->point);
        startPoint = node->next->point;

        node = node->next;
        if (node == figure->DOTS_HEAD)
            break;
    }
}

BOOL GetColor(HWND hwnd, COLORREF *clrref)
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