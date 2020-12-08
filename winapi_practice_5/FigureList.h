#ifndef UNTITLED_FIGURELIST_H
#define UNTITLED_FIGURELIST_H

struct FigureNode*  FIGURES_HEAD;
struct Figure*      currentFigure;
struct Figure;

struct DotNode{
    POINT point;
    struct DotNode* next;
};

struct Figure{
    struct DotNode* DOTS_HEAD;
    int finished;
    int filled;
    int toBeFilled;
    int dotsNumber;
    HPEN pen;
    COLORREF color;
};

struct FigureNode{
    struct FigureNode* next;
    struct Figure figure;
};

struct Figure* CreateFigure(POINT p, HPEN hpen, COLORREF color);
struct FigureNode* AddFigure();
struct DotNode* AddDot(POINT point);
struct DotNode* GetLastDot();
void FinishFigure();

WINBOOL Intersect(POINT pa, POINT pb, POINT pc, POINT pd);
WINBOOL IsVectorIntersect(POINT a, POINT b);
WINBOOL IsPointInside(int x, int y);
WINBOOL IsPointInsideFigure(int x, int y, struct Figure* figure);

RECT getPaintRect(struct Figure* figure);
struct Figure* getFigureByPoint(POINT p);
void ClearList();
#endif //UNTITLED_FIGURELIST_H
