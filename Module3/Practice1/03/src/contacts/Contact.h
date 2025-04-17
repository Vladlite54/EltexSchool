#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

typedef struct Contact {    // структура контакта
    int id;
    char name[100];
    char surname[100];
    char phone[100];
} Contact;

Contact createContact(int id, const char* name, const char* surname, const char* phone); // создать новый контакт
void editName(Contact* contact, const char* name);    // изменить имя
void editSurname(Contact* contact, const char* surname);    // изменить фамилию
void editPhone(Contact* contact, const char* phone);    // изменить номер телефона
int compareContacts(Contact* left, Contact* right);    // сравнить контакты
