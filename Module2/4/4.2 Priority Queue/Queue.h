#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Package {    // структура пакета данных
    char* data;
    int priority;
} Package;

Package makePackage(const char* data, int priority);    // создать пакет
int comparePackage(Package* p1, Package* p2);   // сравнить приоритеты

typedef struct Node {   // узел списка
    Package value;
    struct Node* next;
    struct Node* prev;
} Node;

Node* createNode(Package pack); // создать узел

typedef struct PriorityQueue {   // структура очереди
    size_t size;
    Node* head;
    Node* tail;
} PriorityQueue;

PriorityQueue* makeQueue(); // cоздать очередь
void push(PriorityQueue* queue, Package pack);  // добавить в очередь с приоритетом
int pop(PriorityQueue* queue);  // удалить из очереди
Package* top(PriorityQueue* queue); // первый в очереди
Package* topByPriority(PriorityQueue* queue, int priority); // первый по приоритету
int popByPriority(PriorityQueue* queue, int priority);  // удалить по определенному приоритету
