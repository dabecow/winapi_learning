#include <windows.h>
#include "FigureList.h"

struct Figure* CreateFigure(POINT p, HPEN hpen){
    struct Figure *figure = malloc(sizeof(struct Figure));
    struct DotNode *HEAD = malloc(sizeof(struct DotNode));
    HEAD->point = p;
    HEAD->next = NULL;
    figure->DOTS_HEAD = HEAD;
    figure->pen = hpen;
    figure->finished = 0;
    figure->dotsNumber = 1;
    return figure;
}

struct FigureNode* AddFigure(){
        struct FigureNode* node;
        if (FIGURES_HEAD == NULL) {
            FIGURES_HEAD = malloc(sizeof(struct FigureNode));
            FIGURES_HEAD->next = NULL;
            FIGURES_HEAD->figure = *currentFigure;
            node = FIGURES_HEAD;
        } else {
            node = FIGURES_HEAD;
            while (node->next != NULL) {
                node = node->next;
            }
            node->next = malloc(sizeof(struct FigureNode));
            node = node->next;
            node->figure = *currentFigure;
            node->next = NULL;
        }
        return node;

}

struct DotNode* AddDot(POINT point){
        struct DotNode *node = currentFigure->DOTS_HEAD;
        while (node->next != NULL) {
            node = node->next;
        }
        node->next = malloc(sizeof(struct DotNode));
        node = node->next;
        node->point = point;
        node->next = NULL;
        currentFigure->dotsNumber = currentFigure->dotsNumber + 1;
        return node;
}

struct DotNode* GetLastDot(){
    struct DotNode* iterator = currentFigure->DOTS_HEAD;
    while (iterator->next != NULL){
        iterator = iterator->next;
    }
    return iterator;
}

void FinishFigure(){
    struct DotNode *dotNode = GetLastDot();
    dotNode->next = currentFigure->DOTS_HEAD;
    currentFigure->finished = 1;
}

void ClearList(){
    struct FigureNode* figureNode = FIGURES_HEAD;
    while (figureNode != NULL){
        struct DotNode* dotNode = figureNode->figure.DOTS_HEAD->next;
        while (dotNode != figureNode->figure.DOTS_HEAD){
            struct DotNode* nodeToFree = dotNode;
            dotNode = dotNode->next;
            free(nodeToFree);
        }
        free(figureNode->figure.DOTS_HEAD);
        struct FigureNode* nodeToFree = figureNode;
        figureNode = figureNode->next;
        free(nodeToFree);
    }
    FIGURES_HEAD = NULL;
}

WINBOOL Intersect(POINT pa, POINT pb, POINT pc, POINT pd){



    if((pa.x == pd.x && pa.y == pd.y) || (pb.x == pc.x && pb.y == pc.y))
        return FALSE;

    POINT projection1, projection2, length;
    projection1.x = pb.x - pa.x;
    projection1.y = pb.y - pa.y;
    projection2.x = pd.x - pc.x;
    projection2.y = pd.y - pc.y;

    length.x = pa.x - pc.x;
    length.y = pa.y - pc.y;

    int denominator;
    int numeratorA;
    int numeratorB;

    denominator = projection2.y * projection1.x - projection2.x * projection1.y;
    numeratorA = projection2.x * length.y - projection2.y * length.x;
    numeratorB = projection1.x * length.y - projection1.y * length.x;

    if(denominator == 0 && (numeratorA != 0 || numeratorB != 0))
        return FALSE;


    float residual1 = (float)numeratorA / (float)denominator;
    float residual2 = (float)numeratorB / (float)denominator;

    if(residual1 < 0 || residual1 > 1)
        return FALSE;

    if(residual2 < 0 || residual2 > 1)
        return FALSE;

    return TRUE;
}

WINBOOL IsVectorIntersect(POINT a, POINT b){
//    struct FigureNode* figureNode = FIGURES_HEAD;
//    if (FIGURES_HEAD == NULL && currentFigure != NULL)
//        figureNode = currentFigure;

    if (currentFigure->dotsNumber > 1){
        struct DotNode* prev = currentFigure->DOTS_HEAD;
        struct DotNode* next = prev->next;
        while(next != NULL){
            if(Intersect(a, b, prev->point, next->point) == TRUE)
                return TRUE;
            next = next->next;
            prev = prev->next;
        }

    }

    return FALSE;
}

//WINBOOL IsPointInside(int x, int y){
//    struct DotNode* vA1 = malloc(sizeof(struct DotNode));
//    struct DotNode* vA2 = malloc(sizeof(struct DotNode));
//    struct DotNode* vB1 = malloc(sizeof(struct DotNode));
//    struct DotNode* vB2 = malloc(sizeof(struct DotNode));
//
//    vA1->point.x = x;
//    vA1->point.y = y;
//    vA2->point.x = x + 2000;
//    vA2->point.y = y + 10;
//    vB1->point.x = x;
//    vB1->point.y = y;
//    vB2->point.x = x + 10;
//    vB2->point.y = y + 2000;
//
//    struct FigureNode* figureNode = FIGURES_HEAD;
//
//    int intersectsA = 0;
//    int intersectsB = 0;
//
//    while(figureNode  != NULL){
//        if(figureNode->figure.finished == 1){
//            struct DotNode* first = figureNode->figure.DOTS_HEAD;
//            struct DotNode* prev = figureNode->figure.DOTS_HEAD;
//            struct DotNode* next = prev->next;
//            while(next != NULL){
//                if(Intersect(vA1, vA2, prev, next) == TRUE)
//                    intersectsA++;
//                if(Intersect(vB1, vB2, prev, next) == TRUE)
//                    intersectsB++;
//                next = next->next;
//                prev = prev->next;
//            }
//            if(Intersect(vA1, vA2, first, prev) == TRUE)
//                intersectsA++;
//            if(Intersect(vB1, vB2, first, prev) == TRUE)
//                intersectsB++;
//            if(intersectsA % 2 != 0 && intersectsB % 2 != 0){
//                *currentFigure = figureNode->figure;
//                return TRUE;
//            }
//            intersectsA = 0;
//            intersectsB = 0;
//        }
//        figureNode = figureNode->next;
//    }
//
//    return FALSE;
//}