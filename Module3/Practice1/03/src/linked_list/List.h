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

typedef struct List {   // структура списка
    size_t size;
    Node* head;
    Node* tail;
} List;

List* createList();  // создать список
void pushToList(List* list, Contact contact);    // добавить элемент с сохранением порядка
int deleteFromListById(List* list, int id);   // удалить из списка
Contact* findById(const List* list, int id);    // найти в списке по id
int findMaxId(const List* list);
void serializeList(List* list, const char* filename);
void deserializeList(List* list, const char* filename);
