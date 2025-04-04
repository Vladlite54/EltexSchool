#include "Btree.h"

// Функция для создания нового узла дерева
BTree* createBTreeNode(Contact contact) {
    BTree* newNode = (BTree*)malloc(sizeof(BTree));
    newNode->contact = contact;
    newNode->left = NULL;
    newNode->right = NULL;
    newNode->height = 1;
    return newNode;
}

// Получение высоты узла
int getHeight(BTree* node) {
    if (node == NULL) {
        return 0;
    }
    return node->height;
}

// Обновление высоты узла
void updateHeight(BTree* node) {
    if (node != NULL) {
        int leftHeight = getHeight(node->left);
        int rightHeight = getHeight(node->right);
        node->height = (leftHeight > rightHeight ? leftHeight : rightHeight) + 1;
    }
}

// Вычислениe фактора баланса узла
int getBalanceFactor(BTree* node) {
    if (node == NULL) {
        return 0;
    }
    return getHeight(node->left) - getHeight(node->right);
}

// Правый поворот
BTree* rotateRight(BTree* y) {
    BTree* x = y->left;
    BTree* T2 = x->right;

    x->right = y;
    y->left = T2;

    updateHeight(y);
    updateHeight(x);

    return x;
}

// Левый поворот
BTree* rotateLeft(BTree* x) {
    BTree* y = x->right;
    BTree* T2 = y->left;

    y->left = x;
    x->right = T2;

    updateHeight(x);
    updateHeight(y);

    return y;
}

// Добавление контакта в дерево
BTree* insertContact(BTree* root, Contact contact) {
    if (root == NULL) {
        return createBTreeNode(contact);
    }

    if (contact.id < root->contact.id) {
        root->left = insertContact(root->left, contact);
    } else if (contact.id > root->contact.id) {
        root->right = insertContact(root->right, contact);
    } else {
        printf("Контакт с ID %d уже существует.\n", contact.id);
        return root;
    }

    updateHeight(root);

    int balance = getBalanceFactor(root);

    if (balance > 1 && contact.id < root->left->contact.id) {
        return rotateRight(root);
    }

    if (balance < -1 && contact.id > root->right->contact.id) {
        return rotateLeft(root);
    }

    if (balance > 1 && contact.id > root->left->contact.id) {
        root->left = rotateLeft(root->left);
        return rotateRight(root);
    }

    if (balance < -1 && contact.id < root->right->contact.id) {
        root->right = rotateRight(root->right);
        return rotateLeft(root);
    }

    return root;
}

// Поиск минимального узла в поддереве
BTree* minValueNode(BTree* node) {
    BTree* current = node;
    while (current->left != NULL) {
        current = current->left;
    }
    return current;
}

// Удаление контакта из дерева по ID
BTree* deleteContact(BTree* root, int id, int* check) {
    if (root == NULL) {
        *check = -1;
        return root;
    }

    if (id < root->contact.id) {
        root->left = deleteContact(root->left, id, check);
    } else if (id > root->contact.id) {
        root->right = deleteContact(root->right, id, check);
    } else {
        *check = 0;

        if ((root->left == NULL) || (root->right == NULL)) {
            BTree* temp = root->left ? root->left : root->right;

            if (temp == NULL) {
                temp = root;
                root = NULL;
            } else { 
                *root = *temp;
            }

            free(temp);
        } else {
            BTree* temp = minValueNode(root->right);
            root->contact = temp->contact;
            root->right = deleteContact(root->right, temp->contact.id, check);
        }
    }

    if (root == NULL) {
        return root;
    }

    updateHeight(root);

    int balance = getBalanceFactor(root);

    if (balance > 1 && getBalanceFactor(root->left) >= 0) {
        return rotateRight(root);
    }

    if (balance > 1 && getBalanceFactor(root->left) < 0) {
        root->left = rotateLeft(root->left);
        return rotateRight(root);
    }

    if (balance < -1 && getBalanceFactor(root->right) <= 0) {
        return rotateLeft(root);
    }

    if (balance < -1 && getBalanceFactor(root->right) > 0) {
        root->right = rotateRight(root->right);
        return rotateLeft(root);
    }

    return root;
}

// Поиска контакта в дереве по ID
Contact* findContact(BTree* root, int id) {
    if (root == NULL) {
        return NULL;
    }

    if (id == root->contact.id) {
        return &root->contact;
    } else if (id < root->contact.id) {
        return findContact(root->left, id);
    } else {
        return findContact(root->right, id);
    }
}

// Вывод содержимого дерева в виде таблицы
void printTree(BTree* root) {
    if (root != NULL) {
        printTree(root->left);
        printf("ID: %d, Name: %s, Surname: %s, Phone: %s\n",
               root->contact.id, root->contact.name, root->contact.surname, root->contact.phone);
        printTree(root->right);
    }
}

// Визуальное отображение дерева
void printTreeVisual(BTree* root, int space) {
    if (root == NULL)
        return;

    space += 10;

    printTreeVisual(root->right, space);

    printf("\n");
    for (int i = 10; i < space; i++)
        printf(" ");
    printf("ID: %d(%s)\n", root->contact.id, root->contact.name);

    printTreeVisual(root->left, space);
}
