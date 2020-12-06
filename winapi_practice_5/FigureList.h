#ifndef UNTITLED_FIGURELIST_H
#define UNTITLED_FIGURELIST_H

struct FigureNode* FIGURES_HEAD;
struct Figure;

struct DotNode{
    POINT point;
    struct DotNode* next;
};

struct Figure{
    struct DotNode* DOTS_HEAD;

    HPEN pen;

};

struct FigureNode{
    struct FigureNode* next;
    struct Figure figure;
};

struct Figure* CreateFigure(POINT p, HPEN hpen);
struct FigureNode* AddFigure(struct Figure *figure);
struct DotNode* AddDot(POINT point, struct Figure *figure);
struct DotNode* GetLastDot(struct Figure *figure);
void FinishFigure(struct Figure *figure);
void ClearList();
#endif //UNTITLED_FIGURELIST_H
