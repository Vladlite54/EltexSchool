#include "Contact.h"

// Создать новый контакт
Contact createContact(int id, const char *name, const char *surname, const char *phone) {
    Contact newContact;
    newContact.id = id;
    strcpy(newContact.name, name);
    strcpy(newContact.surname, surname);
    strcpy(newContact.phone, phone);
    return newContact;
}

// Изменить имя
void editName(Contact *contact, const char *name) {
    strcpy(contact->name, name);
}

// Изменить фамилию
void editSurname(Contact *contact, const char *surname) {
    strcpy(contact->surname, surname);
}

// Изменить номер телефона
void editPhone(Contact *contact, const char *phone) {
    strcpy(contact->phone, phone);
}

// Сравнить контакты
int compareContacts(Contact *left, Contact *right) {
    return strcmp(left->name, right->name);
}
