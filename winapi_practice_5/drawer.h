//
// Created by antom on 05.12.2020.
//

#ifndef WINAPI_PRACTICE_5_DRAWER_H
#define WINAPI_PRACTICE_5_DRAWER_H

#include "windows.h"
#include "FigureList.h"

void DrawLine(HDC hdc, HPEN pen, POINT p1, POINT p2);
void DrawFigure(struct Figure figure, HDC hdc, HPEN pen);
BOOL GetColor(HWND hwnd, COLORREF *clrref);

#endif //WINAPI_PRACTICE_5_DRAWER_H
