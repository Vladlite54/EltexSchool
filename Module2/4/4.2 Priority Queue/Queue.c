#include "Queue.h"

// Создать пакет
Package makePackage(const char *data, int priority) {
    Package newPackage;
    newPackage.data = (char*)malloc(sizeof(char) * strlen(data));
    strcpy(newPackage.data, data);
    newPackage.priority = priority;
    return newPackage;
}

// Сравнить приоритет пакетов
int comparePackage(Package *p1, Package *p2) {
    if (p1->priority < p2->priority) return -1;
    else if (p1->priority > p2->priority) return 1;
    else return 0;
}

// Создвть узел
Node *createNode(Package pack) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->value = pack;
    newNode->next = newNode->prev = NULL;
    return newNode;
}

// Создать очередь
PriorityQueue* makeQueue() {
    PriorityQueue* newQueue = (PriorityQueue*)malloc(sizeof(PriorityQueue));
    newQueue->size = 0;
    newQueue->head = newQueue->tail = NULL;
    return newQueue;
}

// Добавить в очередь с учетом приоритета
void push(PriorityQueue* queue, Package pack) {
    Node* item = createNode(pack);
    if (queue->head == NULL) {
        queue->head = item;
        queue->tail = item;
        queue->size++;
        return;
    }
    if (comparePackage(&pack, &queue->head->value) < 0) {
        item->next = queue->head;
        queue->head->prev = item;
        queue->head = item;
        queue->size++;
        return;
    }
    if (comparePackage(&pack, &queue->tail->value) >= 0) {
        item->prev = queue->tail;
        queue->tail->next = item;
        queue->tail = item;
        queue->size++;
        return;
    }
    Node* current = queue->head;
    while (current != NULL) {
        if (comparePackage(&pack, &current->value) < 0) {
            Node* temp = current->prev;
            temp->next = item;
            item->prev = temp;
            current->prev = item;
            item->next = current;
            queue->size++;
            return;
        }
        else current = current->next;
    }
}

// Удалить из очереди
int pop(PriorityQueue *queue) {
    if (queue->size == 0) return -1;
    Node* toDelete = queue->head;
    queue->head = toDelete->next;
    if (queue->head != NULL) queue->head->prev = NULL;
    free(toDelete);
    queue->size--;
    return 0;
}

// Первый в очереди
Package *top(PriorityQueue *queue) {
    if (queue->size == 0) return NULL;
    return &queue->head->value;
}

// Первый по определенному приоритету
Package *topByPriority(PriorityQueue *queue, int priority) {
    for (Node* current = queue->head; current != NULL; current = current->next) {
        if (current->value.priority == priority) return &current->value;
    }
    return NULL;
}

// Удалить первого по определенному приоритету
int popByPriority(PriorityQueue *queue, int priority) {
    if (queue->size == 0) return -1;
    if (queue->head->value.priority == priority) {
        Node* toDelete = queue->head;
        queue->head = toDelete->next;
        if (queue->head != NULL ) queue->head->prev = NULL;
        free(toDelete);
        queue->size--;
        return 0;
    }
    if (queue->tail->value.priority == priority) {
        Node* toDelete = queue->tail;
        queue->tail= toDelete->prev;
        if (queue->tail != NULL ) queue->tail->next = NULL;
        free(toDelete);
        queue->size--;
        return 0;
    }
    Node* toDelete = queue->head;
    while (toDelete != NULL) {
        if (toDelete->value.priority == priority) {
            Node* temp = toDelete->next;
            temp->prev = toDelete->prev;
            toDelete->prev->next = temp;
            free(toDelete);
            queue->size--;
            return 0;
        }
        else toDelete = toDelete->next;
    }
    return -1;
}
