#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../contacts/Contact.h"

typedef struct BTree {  // дерево
    Contact contact;
    struct BTree* left;
    struct BTree* right;
    int height;
} BTree;

BTree* createBTreeNode(Contact contact);    // создать узел дерева
int getHeight(BTree* node); // получение высоты узла
void updateHeight(BTree* node); // обновление высоты узла
int getBalanceFactor(BTree* node);  // получить фактор баланса
BTree* rotateRight(BTree* y);   // левый поворот
BTree* rotateLeft(BTree* x);    // правый поворот
BTree* insertContact(BTree* root, Contact contact); // добавить контакт в дерево
BTree* minValueNode(BTree* node);   // нахождение минимального узла
BTree* deleteContact(BTree* root, int id, int* check);  // удалить контакт
Contact* findContact(BTree* root, int id);  // найти контакт по id
void printTree(BTree* root);    // вывод содержимого дерева в виде таблицы
void printTreeVisual(BTree* root, int space);   // визуальное отображение дерева
