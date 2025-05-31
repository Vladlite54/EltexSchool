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

#define BUFFER_SIZE 165536
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 9090

int client_socket = -1;
uint16_t client_port = 0;

void signal_handler(int sig);   // Обработка сигналов

int send_udp_msg(unsigned char *message, int message_len); // Отправка udp пакета с сообщением
int response_handler(unsigned char *buffer, int size);  // Обработка ответов

int main(int argc, char *argv[]) {
    if (argc < 2) {
        perror("Enter port: ./Client <port>\n");
        exit(EXIT_FAILURE);
    }

    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    client_port = atoi(argv[1]);

    client_socket = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    if (client_socket < 0) {
        perror("Socket creation error");
    }

    int one = 1;
    if (setsockopt(client_socket, IPPROTO_IP, IP_HDRINCL, &one, sizeof(one)) < 0) {
        perror("setsockopt failed");
        close(client_socket);
        exit(EXIT_FAILURE);
    }
    
    printf("Client started. Enter Ctrl+C to end\n");

    char message[BUFFER_SIZE];

    while (1) {
        printf("Enter message -> ");
        if (!fgets(message, BUFFER_SIZE, stdin)) {
            break;
        }

        message[strcspn(message, "\n")] = 0;
        int message_len = strlen(message);

        if (send_udp_msg(message, message_len) != 0) continue;

        unsigned char response_buffer[BUFFER_SIZE];
        memset(response_buffer, 0, sizeof(response_buffer));
       
        while(1) {
            int response_size = recvfrom(client_socket, response_buffer, BUFFER_SIZE, 0, NULL, NULL);
            if (response_size < 0) {
                perror("recvfrom failed");
                continue;
            }
            if (response_handler(response_buffer, response_size) == 0) break;
        }

    }

    return 0;
}

void signal_handler(int sig) {
    printf("\nClosing client...\n");
    if (client_socket != -1) {
        send_udp_msg("SHUTDOWN", 8);
        close(client_socket);
        printf("Socket closed\n");
    }
    printf("Client closed\n");
    exit(EXIT_SUCCESS);
}

int send_udp_msg(unsigned char *message, int message_len) {
    char packet[BUFFER_SIZE];
    memset(packet, 0, sizeof(packet));

    struct iphdr *ip = (struct iphdr*)packet;
    ip->version = 4;
    ip->ihl = 5;       
    ip->tos = 0;
    ip->tot_len = htons(sizeof(struct iphdr) + sizeof(struct udphdr) + message_len);
    ip->id = htons(12345);
    ip->frag_off = 0;
    ip->ttl = 64;
    ip->protocol = IPPROTO_UDP;
    ip->saddr = inet_addr("127.0.0.1");
    ip->daddr = inet_addr(SERVER_IP);
    ip->check = 0;

    struct udphdr *udp = (struct udphdr *)(packet + sizeof(struct iphdr));
    udp->source = htons(client_port);
    udp->dest = htons(SERVER_PORT);
    udp->len = htons(sizeof(struct udphdr) + message_len);
    udp->check = 0;

    char *data = packet + sizeof(struct iphdr) + sizeof(struct udphdr);
    strcpy(data, message);

    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_address.sin_port = htons(SERVER_PORT);

    if (sendto(client_socket, packet, ntohs(ip->tot_len), 0, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        perror("send fail");
        return -1;
    }

    return 0;
}

int response_handler(unsigned char *buffer, int size) {
    struct iphdr *ip_header = (struct iphdr *)buffer;
    unsigned int ip_header_len = ip_header->ihl * 4;
    
    struct udphdr *udp_header = (struct udphdr *)(buffer + ip_header_len);
 
    if (ntohs(udp_header->dest) != client_port) {
        return -1;
    }
    
    char *response_data = (char *)(buffer + ip_header_len + sizeof(struct udphdr));
    int response_data_len = ntohs(udp_header->len) - sizeof(struct udphdr);
        
    char response_msg[BUFFER_SIZE];
    snprintf(response_msg, sizeof(response_msg), "%.*s", response_data_len, response_data);
    int response_msg_len = strlen(response_msg);
    printf("Server response: %s\n", response_msg);

    return 0;
}
