#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../contacts/Contact.h"

typedef struct Node {   // узел списка
    Contact value;
    struct Node* next;
    struct Node* prev;
} Node;

Node* createNode(Contact contact); // создать узел

typedef struct List {
    size_t size;
    Node* head;
    Node* tail;
} List;

List* createList();  // создать список
void pushToList(List* list, Contact contact);    // добавить в начало
int deleteFromListById(List* list, int id);   // удалить из списка
Contact* findById(const List* list, int id);    // найти в списке по id
void printList(List* list);

