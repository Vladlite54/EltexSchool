#include "Menu.h"

// Ввод целого с клавиатуры
int get_int() {
    int number;
    char t;
    while (scanf("%d%c", &number, &t) != 2 || t != '\n') {
        printf("Wrong format!\n");
        while (getchar() != '\n') {}
    }
    return number;
}

void menu()
{
    printf("---Welcome to Contacts---\n");
    const char* info =
    "Command list:\n"
    "-> -1: Close program\n"
    "-> 0: Show command list\n"
    "-> 1: Add new contact\n"
    "-> 2: Delete contact\n"
    "-> 3: Edit contact\n"
    "-> 4: Show contacts\n"
    "-> 5: Show tree\n";

    bool flag = true;
    int input, id;
    Contact newContact;
    Contact* editContact = NULL;
    char newName[100];
    char newSurname[100];
    char newPhone[100];
    BTree* tree = NULL;
    int delCheck = 0;
    int currentId = 0;

    printf("%s", info);
    while (flag) {
        printf("Enter command -> ");
        input = get_int();
        switch(input){
            case -1:
                flag = false;
                break;
            case 0:
                printf("%s", info);
                break;
            case 1:
                printf("Enter name: ");
                fgets(newName, sizeof(newName), stdin);
                newName[strlen(newName) - 1] = '\0';
                if (strlen(newName) == 0) {
                    printf("Name must be filled in!\n");
                    break;
                }
                printf("Enter surname: ");
                fgets(newSurname, sizeof(newSurname), stdin);
                newSurname[strlen(newSurname) - 1] = '\0';
                if (strlen(newSurname) == 0) {
                    printf("Surname must be filled in!\n");
                    break;
                }
                printf("Enter phone: ");
                fgets(newPhone, sizeof(newPhone), stdin);
                newPhone[strlen(newPhone) - 1] = '\0';
                currentId++;
                newContact = createContact(currentId, newName, newSurname, newPhone);
                tree = insertContact(tree, newContact);
                printf("Contact has been added!\n");
                break;
            case 2:
                printf("Enter id you want to delete -> ");
                id = get_int();
                tree = deleteContact(tree, id, &delCheck);
                if (delCheck == -1) printf("No such contact!\n");
                else printf("Contact has been deleted\n");
                break;
            case 3:
                printf("Enter id you want to edit -> ");
                id = get_int();
                editContact = findContact(tree, id);
                if (editContact == NULL) {
                    printf("No such contact!\n");
                    break;
                }
                printf("Select edit type:\n");
                printf("1) Name\n2) Surname\n3) Phone\n");
                printf("Enter type -> ");
                input = get_int();
                switch (input) {
                    case 1:
                        printf("Enter new name -> ");
                        fgets(newName, sizeof(newName), stdin);
                        newName[strlen(newName) - 1] = '\0';
                        editName(editContact, newName);
                        printf("Name has been changed\n");
                        break;
                    case 2:
                        printf("Enter new surname -> ");
                        fgets(newSurname, sizeof(newSurname), stdin);
                        newSurname[strlen(newSurname) - 1] = '\0';
                        editSurname(editContact, newSurname);
                        printf("Surname has been changed\n");
                        break;
                    case 3:
                        printf("Enter new phone -> ");
                        fgets(newPhone, sizeof(newPhone), stdin);
                        newPhone[strlen(newPhone) - 1] = '\0';
                        editPhone(editContact, newPhone);
                        printf("Phone has been changed\n");
                        break;
                    default:
                        printf("Unknown type!\n");
                        break;
                }
                break;
            case 4:
                printf("-----Contacts-----\n");
                printTree(tree);
                break;
            case 5:
                printTreeVisual(tree, 0);
                break;
            default:
                printf("Unknown command!\n");
                break;
        }
    }
}
