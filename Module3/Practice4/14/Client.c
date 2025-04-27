#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#define BUFFER_SIZE 1024
#define PORT 12345
#define SERVER_IP "127.0.0.1"

int socket_fd;
struct sockaddr_in server_address;
socklen_t server_addr_len = sizeof(server_address);
int client_id = -1;  // ID клиента

void *receive_messages(void *arg);  // функция для приема сообщений от сервера

int main() {
    pthread_t receive_thread;
    char message[BUFFER_SIZE];

    socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd < 0) {
        perror("socket() failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_address.sin_port = htons(PORT);

    if (pthread_create(&receive_thread, NULL, receive_messages, NULL) != 0) {
        perror("pthread_create() failed\n");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }

    printf("Client started. Enter 'exit' to stop\n");

    strcmp(message, "ping");

    if (sendto(socket_fd, message, strlen(message), 0, (struct sockaddr *)&server_address, server_addr_len) < 0) {
        perror("sendto() failed\n");
    }

    while (1) {
        fgets(message, BUFFER_SIZE, stdin);

        message[strcspn(message, "\n")] = 0;

        if (strcmp(message, "exit") == 0) {
            break;
        }

        if (sendto(socket_fd, message, strlen(message), 0, (struct sockaddr *)&server_address, server_addr_len) < 0) {
            perror("sendto() failed\n");
        }
    }

    close(socket_fd);
    return 0;
}

void *receive_messages(void *arg) {
    char buffer[BUFFER_SIZE];
    int bytes_received;

    while (1) {
        bytes_received = recvfrom(socket_fd, buffer, BUFFER_SIZE - 1, 0, (struct sockaddr *)&server_address, &server_addr_len);
        if (bytes_received < 0) {
            perror("recvfrom() failed");
            close(socket_fd);
            exit(EXIT_FAILURE);
        }

        buffer[bytes_received] = '\0';
        printf("%s\n", buffer);
    }
    return NULL;
}