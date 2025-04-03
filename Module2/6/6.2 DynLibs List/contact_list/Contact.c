#include "Contact.h"

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

// Сравнить контакты
int compareContacts(Contact *left, Contact *right) {
    return strcmp(left->name, right->name);
}
