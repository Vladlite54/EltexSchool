#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <signal.h>

#define PORT 12345
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 2

typedef struct ClientInfo {
    struct sockaddr_in address;
    socklen_t addr_len;
    int id;
} ClientInfo;

ClientInfo clients[MAX_CLIENTS];
int client_count = 0;

int add_client(struct sockaddr_in address, socklen_t addr_len); // добавить клиента в список
void remove_client(int id); // удалить клиента
void broadcast_message(int socket_fd, char *message, struct sockaddr_in sender_address, socklen_t sender_addr_len); // отправить сообщение остальным клиентам

int main(int argc, char* argv[]) {
    int server;
    int socket_fd;
    struct sockaddr_in server_address, client_address;
    socklen_t client_addr_len = sizeof(client_address);
    char buffer[BUFFER_SIZE];
    int bytes_received;

    socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd < 0) {
        perror("socket() failed\n");
        exit(EXIT_FAILURE);
    }

    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);

    if (bind(socket_fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("bind() failed\n");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server is running. Listening port %d...\n", PORT);

    while(1) {
        bytes_received = recvfrom(socket_fd, buffer, BUFFER_SIZE - 1, 0, (struct sockaddr *)&client_address, &client_addr_len);
        if (bytes_received < 0) {
            perror("recvfrom() failed\n");
            continue;
        }

        buffer[bytes_received] = '\0';

        int client_id = -1;
        for (int i = 0; i < client_count; i++) {
            if (memcmp(&clients[i].address, &client_address, client_addr_len) == 0) {
                client_id = clients[i].id;
                break;
            }
        }

        if (client_id == -1) {
            int new_client_id = add_client(client_address, client_addr_len); // добавляем нового клиента в список
            if (new_client_id != -1) {
                printf("New client has been connected. ID: %d, IP: %s, Port: %d\n", new_client_id, inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));

                 // отправляем новому клиенту его ID
                char id_message[BUFFER_SIZE];
                snprintf(id_message, BUFFER_SIZE, "SERVER: Ваш ID: %d\n", new_client_id);
                if (sendto(socket_fd, id_message, strlen(id_message), 0, (struct sockaddr *)&client_address, client_addr_len) < 0) {
                    perror("sendto() failed sending ID\n");
                }

                // оповещаем всех остальных клиентов о новом подключении
                char join_message[BUFFER_SIZE];
                snprintf(join_message, BUFFER_SIZE, "SERVER: Client %d has been\n", new_client_id);
                broadcast_message(socket_fd, join_message, client_address, client_addr_len);
            } else {
                printf("Client limit\n");
                char reject_message[BUFFER_SIZE] = "SERVER: Client limin\n";
                if (sendto(socket_fd, reject_message, strlen(reject_message), 0, (struct sockaddr *)&client_address, client_addr_len) < 0) {
                    perror("sendto() failed sending rejection message\n");
                }
            }
        } else {
            printf("From client %d (IP: %s, Port: %d): %s\n", client_id, inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port), buffer);

            char formatted_message[BUFFER_SIZE];
            snprintf(formatted_message, BUFFER_SIZE, "Client %d: %s", client_id, buffer);

            broadcast_message(socket_fd, formatted_message, client_address, client_addr_len);
        }
    }

    close(socket_fd);
    return 0;
}

int add_client(struct sockaddr_in address, socklen_t addr_len) {
    if (client_count < MAX_CLIENTS) {
        clients[client_count].address = address;
        clients[client_count].addr_len = addr_len;
        clients[client_count].id = client_count + 1;  // ID клиента (начинается с 1)
        client_count++;
        return client_count;
    } else {
        return -1;
    }
}

void remove_client(int id) {
    for (int i = 0; i < client_count; i++) {
        if (clients[i].id == id) {
            for (int j = i; j < client_count - 1; j++) {
                clients[j] = clients[j + 1];
            }
            client_count--;
            break;
        }
    }
}

void broadcast_message(int socket_fd, char *message, struct sockaddr_in sender_address, socklen_t sender_addr_len) {
    for (int i = 0; i < client_count; i++) {
        // Не отправляем сообщение отправителю
        if (memcmp(&clients[i].address, &sender_address, sender_addr_len) != 0) {
            if (sendto(socket_fd, message, strlen(message), 0, (struct sockaddr *)&clients[i].address, clients[i].addr_len) < 0) {
                perror("sendto() failed\n");
            }
        }
    }
}
