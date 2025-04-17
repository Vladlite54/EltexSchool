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
    if (list->size == 0) return -1;
    if (list->head->value.id == id) {
        Node* toDelete = list->head;
        list->head = toDelete->next;
        if (list->head != NULL ) list->head->prev = NULL;
        free(toDelete);
        list->size--;
        return 0;
    }
    if (list->tail->value.id == id) {
        Node* toDelete = list->tail;
        list->tail= toDelete->prev;
        if (list->tail != NULL ) list->tail->next = NULL;
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

int findMaxId(const List *list) {
    int max = 0;
    for (Node* current = list->head; current != NULL; current = current->next) {
        if (current->value.id > max) max = current->value.id;
    }
    return max;
}

void serializeList(List *list, const char *filename) {
    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("Open error\n");
        return;
    }
    size_t sizeToWrite = list->size;
    if (write(fd, &sizeToWrite, sizeof(size_t)) != sizeof(size_t)) {
        perror("Size write error\n");
        close(fd);
        return;
    }
    Node* current = list->head;
    while (current != NULL) {
        if (write(fd, &(current->value), sizeof(Contact)) != sizeof(Contact)) {
            perror("Write error\n");
            break;
        }
        current = current->next;
    }
    close(fd);
}

void deserializeList(List *list, const char *filename) {
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("Open error\n");
        return;
    }
    size_t size;
    if (read(fd, &size, sizeof(size_t)) != sizeof(size_t)) {
        perror("Size read error");
        close(fd);
        free(list);
        return;
    }
    for (size_t i = 0; i < size; i++) {
        Contact contact;
        if (read(fd, &contact, sizeof(Contact)) != sizeof(Contact)) {
            perror("Read error\n");
            break;
        }
        pushToList(list, contact);
    }
    close(fd);
}
