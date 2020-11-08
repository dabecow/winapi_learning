#include <windows.h>
#include "FigureList.h"


enum FigureType;

struct Figure;

struct Node;

struct Node* AddNode(struct Figure figure){
    if (HEAD == NULL){
        HEAD = malloc(sizeof(struct Node));
        HEAD->figure = figure;
        HEAD->next = NULL;
        return HEAD;
    } else {
        struct Node *node = HEAD;
        while (node->next != NULL) {
            node = node->next;
        }
        node->next = malloc(sizeof(struct Node));
        node = node->next;
        node->figure = figure;
        node->next = NULL;
        return node;
    }
}

void ClearList(){
    HEAD = NULL;
}