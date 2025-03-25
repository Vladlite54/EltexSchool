#include "ContactList.h"

// Создать новый контакт
Contact createContact(int id, const char *name, const char *surname, const char *phone) {
    Contact newContact;
    newContact.id = id;
    newContact.name = (char*)malloc(sizeof(char) * strlen(name));
    newContact.surname = (char*)malloc(sizeof(char) * strlen(surname));
    newContact.phone = (char*)malloc(sizeof(char) * strlen(phone));
    strcpy(newContact.name, name);
    strcpy(newContact.surname, surname);
    strcpy(newContact.phone, phone);
    return newContact;
}

// Изменить имя
void editName(Contact *contact, const char *name) {
    free(contact->name);
    contact->name = (char*)malloc(sizeof(char) * strlen(name));
    strcpy(contact->name, name);
}

// Изменить фамилию
void editSurname(Contact *contact, const char *surname) {
    free(contact->surname);
    contact->surname = (char*)malloc(sizeof(char) * strlen(surname));
    strcpy(contact->surname, surname);
}

// Изменить номер телефона
void editPhone(Contact *contact, const char *phone) {
    free(contact->phone);
    contact->phone = (char*)malloc(sizeof(char) * strlen(phone));
    strcpy(contact->phone, phone);
}

// Создать новый пустой список контактов
ContactList createContactList() {
    ContactList newList;
    newList.list = NULL;
    newList.size = 0;
    return newList;
}

// Добавить контакт
void addContact(ContactList *contacts, Contact contact) {
    if (contacts->list == NULL) {
        contacts->size = 1;
        contacts->list = (Contact*)malloc(sizeof(Contact) * contacts->size);
        contacts->list[0] = contact;
    }
    else {
        Contact* newList = (Contact*)malloc(sizeof(Contact) * (contacts->size + 1));
        for (size_t i = 0; i < contacts->size; ++i) {
            newList[i] = contacts->list[i];
        }
        newList[contacts->size] = contact;
        free(contacts->list);
        contacts->list = newList;
        contacts->size++;
    }
}

// Удалить контакт по id
int deleteContact(ContactList *contacts, int id) {
    int position = findPositionById(contacts, id);
    if (position == -1) return -1;
    Contact* newList = (Contact*)malloc(sizeof(Contact) * (contacts->size - 1));
    for (size_t i = 0, j = 0; i < contacts->size; ++i) {
        if (i == position) continue;
        newList[j] = contacts->list[i];
        ++j;
    }
    free(contacts->list);
    contacts->list = newList;
    contacts->size--;
    return 0;
}

// Найти контакт по id
Contact *findById(const ContactList *contacts, int id) {
    for (size_t i = 0; i < contacts->size; ++i) {
        if (contacts->list[i].id == id) return &contacts->list[i];
    }
    return NULL;
}

// Найти позицию в списке по id 
int findPositionById(const ContactList *contacts, int id) {
    for (size_t i = 0; i < contacts->size; ++i) {
        if (contacts->list[i].id == id) return i;
    }
    return -1;
}
