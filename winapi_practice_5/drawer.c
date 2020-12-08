
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

void FillFigure(struct Figure *figure, HDC hdc){
        if (figure->filled == 1) {
            POINT *array = malloc(sizeof(POINT) * figure->dotsNumber);
            struct DotNode *dotNode = figure->DOTS_HEAD;
            for (int i = 0; i < figure->dotsNumber; ++i) {
                array[i] = dotNode->point;
                dotNode = dotNode->next;
            }
            HBRUSH brush = CreateSolidBrush(figure->color);
            SelectObject(hdc, brush);
            Polygon(hdc, array, figure->dotsNumber);
        } else if (figure->toBeFilled == 1){
            FillFigureAlgorithm(figure, hdc);
        }
//        TransparentBlt(NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
}

void FillFigureAlgorithm(struct Figure* figure, HDC hdc) {

    RECT paintRect = getPaintRect(figure);
    for (int x = 0; x < paintRect.right; x++) {
        for (int y = 0; y < paintRect.bottom; y++) {
            if (IsPointInsideFigure(x, y, figure) == TRUE) {
                SetPixel(hdc, x, y, figure->color);
            }
        }
    }
    figure->toBeFilled = 0;
    figure->filled = 1;
}


void TransparentBlit(HDC dest_dc, int xPos, int yPos, int img_w, int img_h,
                     HDC src_dc, int xStart, int yStart, UINT transparency) {
    BYTE *img_bits;
    BYTE *back_bits;
    BITMAPINFO bmp_info = {0};
    HBITMAP img_bmp, img2_bmp, oldBmp, old2_bmp;
    HDC cmp_dc, cmp2_dc;
    bmp_info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmp_info.bmiHeader.biCompression = BI_RGB;
    bmp_info.bmiHeader.biHeight = img_h;
    bmp_info.bmiHeader.biWidth = img_w;
    bmp_info.bmiHeader.biBitCount = 24;
    bmp_info.bmiHeader.biClrUsed = 0;
    bmp_info.bmiHeader.biPlanes = 1;
    img_bmp = CreateDIBSection(dest_dc, &bmp_info, DIB_RGB_COLORS, (void **) &img_bits, 0, 0);
    img2_bmp = CreateDIBSection(dest_dc, &bmp_info, DIB_RGB_COLORS, (void **) &back_bits, 0, 0);
    cmp_dc = CreateCompatibleDC(dest_dc);
    oldBmp = (HBITMAP) SelectObject(cmp_dc, img_bmp);
    cmp2_dc = CreateCompatibleDC(src_dc);
    old2_bmp = (HBITMAP) SelectObject(cmp2_dc, img2_bmp);
    BitBlt(cmp_dc, 0, 0, img_w, img_h, dest_dc, xPos, yPos, SRCCOPY);
    BitBlt(cmp2_dc, 0, 0, img_w, img_h, src_dc, xStart, yStart, SRCCOPY);
    for (int i = 0; i < img_w * img_h * 3; i += 3) {
        if ((back_bits != GetBValue(transparency)) || (back_bits != GetGValue(transparency)) ||
            (back_bits != GetRValue(transparency))) {
            img_bits = back_bits;
            img_bits = back_bits;
            img_bits = back_bits;
        }
        BitBlt(dest_dc, xPos, yPos, img_w, img_h, cmp_dc, 0, 0, SRCCOPY);
        SelectObject(cmp_dc, oldBmp);
        SelectObject(cmp2_dc, old2_bmp);
        DeleteObject(img_bmp);
        DeleteObject(img2_bmp);
        DeleteDC(cmp_dc);
        DeleteDC(cmp2_dc);
    }
}
