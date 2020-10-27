#ifndef UNTITLED_FIGURELIST_H
#define UNTITLED_FIGURELIST_H

struct Node* HEAD;

enum FigureType{
    isRectangle,
    isCircle,
    isLine
};

struct Figure{
    enum FigureType figureType;
    HBRUSH brush;
    HPEN pen;
    RECT positionRect;
};
struct Node{
    struct Node* next;
    struct Figure figure;
};

struct Node* AddNode(struct Figure figure);
void ClearList();
#endif //UNTITLED_FIGURELIST_H
