#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct Contact {    // структура контакта
    int id;
    char* name;
    char* surname;
    char* phone;
} Contact;

Contact createContact(int id, const char* name, const char* surname, const char* phone); // создать новый контакт
void editName(Contact* contact, const char* name);    // изменить имя
void editSurname(Contact* contact, const char* surname);    // изменить фамилию
void editPhone(Contact* contact, const char* phone);    // изменить номер телефона

typedef struct ContactList {    // струткура списка контактов
    Contact* list;
    size_t size;
} ContactList;

ContactList createContactList();    // создать новый пустой список контактов
void addContact(ContactList* contacts, Contact contact); // добавить контакт
int deleteContact(ContactList* contacts, int id); // удалить контакт по id
Contact* findById(const ContactList* contacts, int id);  // найти контакт по id
int findPositionById(const ContactList* contacts, int id);  // найти позицию в списке по id