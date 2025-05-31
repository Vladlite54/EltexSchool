#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/udp.h>

#define BUFFER_SIZE 65536
#define SERVER_PORT 9090

typedef struct ClientInfo { // Структура для хранения данных подключенного клиента
    uint32_t ip;
    uint16_t port;
    int counter;
} ClientInfo;

ClientInfo *clients = NULL; // Пул клиентов
int client_count = 0;
int server_socket = -1;

int find_client(uint32_t ip, uint16_t port);    // Поиск клиента по адресу и порту
void add_client(uint32_t ip, uint16_t port);    // Добавить клиента в пул
void remove_client(uint32_t ip, uint16_t port); // Удалить клиента из пула

void signal_handler(int sig);   // Обработчик сигналов

void message_handler(unsigned char *buffer, int size); // Обработчик сообщений клиентов

int main(int argc, char *argv[]) {
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    server_socket = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    if (server_socket < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    int one = 1;
    if (setsockopt(server_socket, IPPROTO_IP, IP_HDRINCL, &one, sizeof(one)) < 0) {
        perror("setsockopt failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }
    
    printf("Server started on port %d\n", SERVER_PORT);

    unsigned char buffer[BUFFER_SIZE];

    while (1) {
        int packet_size = recvfrom(server_socket, buffer, BUFFER_SIZE, 0, NULL, NULL);
        if (packet_size < 0) {
            perror("recvfrom failed");
            continue;
        }
        
        message_handler(buffer, packet_size);
    }

    return 0;
}

int find_client(uint32_t ip, uint16_t port) {
    for (int i = 0; i < client_count; i++) {
        if (clients[i].ip == ip && clients[i].port == port) return i;
    }
    return -1;
}

void add_client(uint32_t ip, uint16_t port) {
    client_count++;
    if (clients == NULL) {
        clients = (ClientInfo*)malloc(sizeof(ClientInfo) * client_count);
        clients[0].ip = ip;
        clients[0].port = port;
        clients[0].counter = 0;
    }
    else {
        ClientInfo *new_list = (ClientInfo*)malloc(sizeof(ClientInfo) * client_count);
        for (int i = 0; i < client_count - 1; i++) {
            new_list[i] = clients[i];
        }
        new_list[client_count - 1].ip = ip;
        new_list[client_count - 1].port = port;
        new_list[client_count - 1].counter = 0;
        free(clients);
        clients = new_list;
    }
}

void remove_client(uint32_t ip, uint16_t port) {
    int index = find_client(ip, port);
    if (index == -1) return;
    client_count--;
    ClientInfo *new_list = (ClientInfo*)malloc(sizeof(ClientInfo) * client_count);
    for (int i = 0, j = 0; i < client_count + 1; i++) {
        if (i == index) continue;
        new_list[j] = clients[i];
        j++;
    }
    free(clients);
    clients = new_list;
}

void signal_handler(int sig) {
    printf("\nClosing server...\n");
    if (server_socket != -1) {
        printf("Server socket closed\n");
        close(server_socket);
    }
    if (clients != NULL) {
        printf("Client list cleared\n");
        free(clients);
    }
    printf("Server closed\n");
    exit(EXIT_SUCCESS);
}

void message_handler(unsigned char *buffer, int size) {
    struct iphdr *ip_header = (struct iphdr *)buffer;
    unsigned int ip_header_len = ip_header->ihl * 4;
    
    struct udphdr *udp_header = (struct udphdr *)(buffer + ip_header_len);
    
    if (ntohs(udp_header->dest) != SERVER_PORT) {
        return;
    }
    
    char *data = (char *)(buffer + ip_header_len + sizeof(struct udphdr));
    int data_len = ntohs(udp_header->len) - sizeof(struct udphdr);
    
    uint32_t src_ip = ip_header->saddr;
    uint16_t src_port = ntohs(udp_header->source);
    
    if (data_len == 8 && strncmp(data, "SHUTDOWN", 8) == 0) {
        printf("Client %s:%d disconnected\n", inet_ntoa(*(struct in_addr *)&src_ip), src_port);
        remove_client(src_ip, src_port);
        return;
    }
    
    int client_index = find_client(src_ip, src_port);
    if (client_index == -1) {
        printf("New client: %s:%d\n", inet_ntoa(*(struct in_addr *)&src_ip), src_port);
        add_client(src_ip, src_port);
        client_index = client_count - 1;
    }
    
    clients[client_index].counter++;
    
    char response[BUFFER_SIZE];
    snprintf(response, sizeof(response), "%.*s %d", data_len, data, clients[client_index].counter);
    int response_len = strlen(response);
    printf("Response for %s:%d %s\n", inet_ntoa(*(struct in_addr *)&src_ip), src_port, response);
    
    unsigned char packet[BUFFER_SIZE];
    memset(packet, 0, BUFFER_SIZE);
    
    struct iphdr *send_ip_header = (struct iphdr *)packet;
    send_ip_header->ihl = 5;
    send_ip_header->version = 4;
    send_ip_header->tos = 0;
    send_ip_header->tot_len = htons(sizeof(struct iphdr) + sizeof(struct udphdr) + response_len);
    send_ip_header->id = htons(54321);
    send_ip_header->frag_off = 0;
    send_ip_header->ttl = 64;
    send_ip_header->protocol = IPPROTO_UDP;
    send_ip_header->check = 0; // Let kernel fill this
    send_ip_header->saddr = inet_addr("127.0.0.1");
    send_ip_header->daddr = ip_header->saddr;
    
    struct udphdr *send_udp_header = (struct udphdr *)(packet + sizeof(struct iphdr));
    send_udp_header->source = htons(SERVER_PORT);
    send_udp_header->dest = udp_header->source;
    send_udp_header->len = htons(sizeof(struct udphdr) + response_len);
    send_udp_header->check = 0; // Optional for IPv4
    
    char *response_data = packet + sizeof(struct iphdr) + sizeof(struct udphdr);
    strcpy(response_data, response);
    
    struct sockaddr_in dest_addr;
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    dest_addr.sin_port = src_port;
    
    if (sendto(server_socket, packet, ntohs(send_ip_header->tot_len), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) < 0) {
        perror("sendto failed");
    }

    printf("Sent: %s\n", response_data);
}
