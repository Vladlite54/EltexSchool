#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>

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

// Преобразования цифрового представления прав доступа в битовое и буквенное
void octal_to_permissions(mode_t perm, char *permissions_str, char *binary_str) {
    if (perm < 0 || perm > 777) {
        strcpy(permissions_str, "Неверный формат");
        strcpy(binary_str, "Неверный формат");
        return;
    }

    int owner, group, others;

    // первод в восьмиричную систему, так как в perm права хранятся в десятичном виде
    owner = (perm >> 6) & 0x7;   
    group = (perm >> 3) & 0x7;   
    others = perm & 0x7;
 
    // буквенный вид
    permissions_str[0] = (owner & 4) ? 'r' : '-';
    permissions_str[1] = (owner & 2) ? 'w' : '-';
    permissions_str[2] = (owner & 1) ? 'x' : '-';
    permissions_str[3] = (group & 4) ? 'r' : '-';
    permissions_str[4] = (group & 2) ? 'w' : '-';
    permissions_str[5] = (group & 1) ? 'x' : '-';
    permissions_str[6] = (others & 4) ? 'r' : '-';
    permissions_str[7] = (others & 2) ? 'w' : '-';
    permissions_str[8] = (others & 1) ? 'x' : '-';
    permissions_str[9] = '\0';

    // битовый вид
    sprintf(binary_str, "%d%d%d%d%d%d%d%d%d",
            (owner & 4) ? 1 : 0, (owner & 2) ? 1 : 0, (owner & 1) ? 1 : 0,
            (group & 4) ? 1 : 0, (group & 2) ? 1 : 0, (group & 1) ? 1 : 0,
            (others & 4) ? 1 : 0, (others & 2) ? 1 : 0, (others & 1) ? 1 : 0);
}

// Преобразование буквенного представления прав доступа в цифровое
mode_t symbolic_to_octal(const char *permissions_str) {
    mode_t perm = 0;

    if (strlen(permissions_str) != 9) {
        return -1;
    }

    perm |= (permissions_str[0] == 'r') ? S_IRUSR : 0;
    perm |= (permissions_str[1] == 'w') ? S_IWUSR : 0;
    perm |= (permissions_str[2] == 'x') ? S_IXUSR : 0;

    perm |= (permissions_str[3] == 'r') ? S_IRGRP : 0;
    perm |= (permissions_str[4] == 'w') ? S_IWGRP : 0;
    perm |= (permissions_str[5] == 'x') ? S_IXGRP : 0;

    perm |= (permissions_str[6] == 'r') ? S_IROTH : 0;
    perm |= (permissions_str[7] == 'w') ? S_IWOTH : 0;
    perm |= (permissions_str[8] == 'x') ? S_IXOTH : 0;

    return perm;
}

// Отобразить права доступа файла
void display_file_permissions(const char *filename, mode_t* perm) {
    struct stat file_info;

    if (stat(filename, &file_info) == 0) {
        *perm = file_info.st_mode & 0777;
        char permissions_str[11];
        char binary_str[10];
        octal_to_permissions(*perm, permissions_str, binary_str);

        printf("Файл: %s\n", filename);
        printf("Буквенное представление: %s\n", permissions_str);
        printf("Цифровое представление: %03o\n", *perm);
        printf("Битовое представление: %s\n", binary_str);
    } else {
        perror("Ошибка при получении информации о файле");
    }
}

// Функция для имитации изменения прав доступа
void modify_permissions(mode_t* perm, char operation, char user_type, char permission) {
    mode_t inverse = 0;
    switch(user_type) {
        case 'u':
            if (permission == 'r' && operation == '+') *perm |= S_IRUSR;
            if (permission == 'r' && operation == '-') {
                inverse = ~S_IRUSR;
                *perm &= inverse;
            }
            if (permission == 'w' && operation == '+') *perm |= S_IWUSR;
            if (permission == 'w' && operation == '-') {
                inverse = ~S_IWUSR;
                *perm &= inverse;
            }
            if (permission == 'x' && operation == '+') *perm |= S_IXUSR;
            if (permission == 'x' && operation == '-') {
                inverse = ~S_IXUSR;
                *perm &= inverse;
            }
            break;
        case 'g':
            if (permission == 'r' && operation == '+') *perm |= S_IRGRP;
            if (permission == 'r' && operation == '-') {
                inverse = ~S_IRGRP;
                *perm &= inverse;
            }
            if (permission == 'w' && operation == '+') *perm |= S_IWGRP;
            if (permission == 'w' && operation == '-') {
                inverse = ~S_IWGRP;
                *perm &= inverse;
            }
            if (permission == 'x' && operation == '+') *perm |= S_IXGRP;
            if (permission == 'x' && operation == '-') {
                inverse = ~S_IXGRP;
                *perm &= inverse;
            }
            break;
        case 'o':
            if (permission == 'r' && operation == '+') *perm |= S_IROTH;
            if (permission == 'r' && operation == '-') {
                inverse = ~S_IROTH;
                *perm &= inverse;
            }
            if (permission == 'w' && operation == '+') *perm |= S_IWOTH;
            if (permission == 'w' && operation == '-') {
                inverse = ~S_IWOTH;
                *perm &= inverse;
            }
            if (permission == 'x' && operation == '+') *perm |= S_IXOTH;
            if (permission == 'x' && operation == '-') {
                inverse = ~S_IXOTH;
                *perm &= inverse;
            }
            break;
        default:
            break;
    }
}

int main() {
    int choice;
    char input[20];
    char permissions_str[11] = "---------";
    char binary_str[10] = "000000000";
    mode_t perm = 0;

    const char* info =
        "\nМеню:\n"
        "1 -> Ввести права доступа в цифровом обозначении\n"
        "2 -> Ввести права доступа в буквенном обозначении\n"
        "3 -> Ввести имя файла для отображения прав доступа\n"
        "4 -> Модифицировать права доступа\n"
        "5 -> Присвоить права доступа\n"
        "6 -> Показать сохраненные права\n"
        "0 -> Выход\n"
        "Введите команду -> ";

    do {
        printf("%s", info);
        choice = get_int();

        switch (choice) {
            case 1: {
                printf("Введите права доступа в цифровом обозначении (000-777): ");
                if (fgets(input, sizeof(input), stdin) == NULL) {
                    perror("Ошибка чтения ввода");
                    break;
                }
                mode_t octal_perm;
                if (sscanf(input, "%o", &octal_perm) != 1) {
                    fprintf(stderr, "Некорректный формат\n");
                    break;
                }
                octal_to_permissions(octal_perm, permissions_str, binary_str);
                printf("Буквенное представление: %s\n", permissions_str);
                printf("Битовое представление: %s\n", binary_str);
                break;
            }
            case 2: {
                printf("Введите права доступа в буквенном обозначении (например, rwxr-xr--): ");
                if (fgets(input, sizeof(input), stdin) == NULL) {
                    perror("Ошибка чтения ввода");
                    break;
                }

                input[strcspn(input, "\n")] = 0;

                if (strlen(input) != 9) {
                    printf("Некорректный ввод. Введите 9 символов (r, w, x, -).\n");
                    break;
                }

                mode_t octal_perm = symbolic_to_octal(input);
                if (octal_perm == -1) {
                    printf("Некорректный ввод. Используйте только r, w, x и -.\n");
                    break;
                }
                octal_to_permissions(octal_perm, permissions_str, binary_str);
                printf("Цифровое представление: %o\n", octal_perm);
                printf("Битовое представление: %s\n", binary_str);
                strcpy(permissions_str, input);
                break;
            }
            case 3: {
                printf("Введите имя файла: ");
                if (fgets(input, sizeof(input), stdin) == NULL) {
                    perror("Ошибка чтения ввода");
                    break;
                }
                input[strcspn(input, "\n")] = 0;

                display_file_permissions(input, &perm);
                break;
            }
            case 4: {
                printf("Введите команду изменения прав доступа (например, u+x, g-w, a+r): ");
                char operation, user_type, permission;
                if (scanf(" %c%c%c", &user_type, &operation, &permission) == 3) {
                    printf("До: %03o\n", perm);
                    modify_permissions(&perm, operation, user_type, permission);
                    printf("После: %03o\n", perm);
                    octal_to_permissions(perm, permissions_str, binary_str);
                    printf("Буквенное представление: %s\n", permissions_str);
                    printf("Цифровое представление: %03o\n", perm);
                    printf("Битовое представление: %s\n", binary_str);

                } else {
                    printf("Некорректный формат команды\n");
                    int c;
                    while ((c = getchar()) != '\n' && c != EOF);

                }
                break;
            }
            case 5: {
                printf("Введите права доступа в цифровом обозначении (000-777): ");
                if (fgets(input, sizeof(input), stdin) == NULL) {
                    perror("Ошибка чтения ввода");
                    break;
                }
                mode_t octal_perm;
                if (sscanf(input, "%o", &octal_perm) != 1) {
                    fprintf(stderr, "Некорректный формат\n");
                    break;
                }
                printf("До: %03o\n", perm);
                perm = octal_perm;
                printf("После: %03o\n", perm);
                octal_to_permissions(perm, permissions_str, binary_str);
                printf("Буквенное представление: %s\n", permissions_str);
                printf("Цифровое представление: %03o\n", perm);
                printf("Битовое представление: %s\n", binary_str);
                break;
            }
            case 6: {
                printf("Текущие права:\n");
                octal_to_permissions(perm, permissions_str, binary_str);
                printf("Буквенное представление: %s\n", permissions_str);
                printf("Цифровое представление: %03o\n", perm);
                printf("Битовое представление: %s\n", binary_str);
                break;
            }
            case 0:
                printf("Выход из программы.\n");
                break;
            default:
                printf("Некорректный выбор. Пожалуйста, выберите действие из меню.\n");
        }
    } while (choice != 0);

    return 0;
}