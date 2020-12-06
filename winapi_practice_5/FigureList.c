#include <windows.h>
#include "FigureList.h"

struct Figure* CreateFigure(POINT p, HPEN hpen){
    struct Figure *figure = malloc(sizeof(struct Figure));
    struct DotNode *HEAD = malloc(sizeof(struct DotNode));
    HEAD->point = p;
    HEAD->next = NULL;
    figure->DOTS_HEAD = HEAD;
    figure->pen = hpen;
    return figure;
}

struct FigureNode* AddFigure(struct Figure *figure){
        struct FigureNode* node;
        if (FIGURES_HEAD == NULL) {
            FIGURES_HEAD = malloc(sizeof(struct FigureNode));
            FIGURES_HEAD->next = NULL;
            FIGURES_HEAD->figure = *figure;
            node = FIGURES_HEAD;
        } else {
            node = FIGURES_HEAD;
            while (node->next != NULL) {
                node = node->next;
            }
            node->next = malloc(sizeof(struct FigureNode));
            node = node->next;
            node->figure = *figure;
            node->next = NULL;
        }
        return node;

}

struct DotNode* AddDot(POINT point, struct Figure *figure){
        struct DotNode *node = figure->DOTS_HEAD;
        while (node->next != NULL) {
            node = node->next;
        }
        node->next = malloc(sizeof(struct DotNode));
        node = node->next;
        node->point = point;
        node->next = NULL;
        return node;
}

struct DotNode* GetLastDot(struct Figure *figure){
    struct DotNode* iterator = figure->DOTS_HEAD;
    while (iterator->next->next != NULL){
        iterator = iterator->next;
    }
    return iterator;
}

void FinishFigure(struct Figure *figure){
    struct DotNode *dotNode = GetLastDot(figure);
    *&dotNode->next = figure->DOTS_HEAD;
}

void ClearList(){
    struct FigureNode* figureNode = FIGURES_HEAD;
    while (figureNode != NULL){
        struct DotNode* dotNode = figureNode->figure.DOTS_HEAD;
        while (dotNode != NULL){
            struct DotNode* nodeToFree = dotNode;
            dotNode = dotNode->next;
            free(nodeToFree);
        }
        struct FigureNode* nodeToFree = figureNode;
        figureNode = figureNode->next;
        free(nodeToFree);
    }
    FIGURES_HEAD = NULL;
}
