#include "List.h"

// Создать узел списка
Node *createNode(Contact contact) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->value = contact;
    newNode->next = newNode->prev = NULL;
    return newNode;
}

// Создать новый пустой список
List* createList() {
    List* newList = (List*)malloc(sizeof(List));
    newList->size = 0;
    newList->head = newList->tail = NULL;
    return newList;
}

// Добавить элемент в список с сохранение порядка
void pushToList(List *list, Contact contact) {
    Node* item = createNode(contact);
    if (list->head == NULL) {
        list->head = item;
        list->tail = item;
        list->size++;
        return;
    }
    if (compareContacts(&contact, &list->head->value) < 0) {
        item->next = list->head;
        list->head->prev = item;
        list->head = item;
        list->size++;
        return;
    }
    if (compareContacts(&contact, &list->tail->value) > 0) {
        item->prev = list->tail;
        list->tail->next = item;
        list->tail = item;
        list->size++;
        return;
    }
    Node* current = list->head;
    while (current != NULL) {
        if (compareContacts(&contact, &current->value) < 0) {
            Node* temp = current->prev;
            temp->next = item;
            item->prev = temp;
            current->prev = item;
            item->next = current;
            list->size++;
            return;
        }
        else current = current->next;
    }
}

// Удалить элемент из списка по id
int deleteFromListById(List *list, int id) {
    if (list->head->value.id == id) {
        Node* toDelete = list->head;
        list->head = toDelete->next;
        list->head->prev = NULL;
        free(toDelete);
        list->size--;
        return 0;
    }
    if (list->tail->value.id == id) {
        Node* toDelete = list->tail;
        list->tail= toDelete->prev;
        list->tail->next = NULL;
        free(toDelete);
        list->size--;
        return 0;
    }
    Node* toDelete = list->head;
    while (toDelete != NULL) {
        if (toDelete->value.id == id) {
            Node* temp = toDelete->next;
            temp->prev = toDelete->prev;
            toDelete->prev->next = temp;
            free(toDelete);
            list->size--;
            return 0;
        }
        else toDelete = toDelete->next;
    }
    return -1;
}

// Найти элемент по ID
Contact *findById(const List *list, int id) {
    for (Node* current = list->head; current != NULL; current = current->next) {
        if (current->value.id == id) return &current->value;
    }
    return NULL;
}
