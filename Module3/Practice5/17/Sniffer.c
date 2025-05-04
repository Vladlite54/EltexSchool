#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <arpa/inet.h>
#include <time.h>

#define BUFFER_SIZE 65536
#define SERVER_PORT 12345

void save_packet_dump(unsigned char *data, int size, struct iphdr *iph);
void process_udp_packet(unsigned char *buffer, int size);

int main() {
    
    int raw_socket;
    struct sockaddr saddr;
    unsigned char *buffer = (unsigned char *)malloc(BUFFER_SIZE);
    
    raw_socket = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    if(raw_socket < 0) {
        perror("RAW socket create error");
        exit(1);
    }
    
    printf("Start sniffing UDP packages for port %d...\n", SERVER_PORT);
    
    while(1) {
        int saddr_size = sizeof(saddr);
        int data_size = recvfrom(raw_socket, buffer, BUFFER_SIZE, 0, &saddr, &saddr_size);
        
        if(data_size < 0) {
            perror("Error of getting package");
            break;
        }
        
        process_udp_packet(buffer, data_size);
    }
    
    close(raw_socket);
    free(buffer);
    return 0;
}

void save_packet_dump(unsigned char *data, int size, struct iphdr *iph) {
    const char filename[] = "dump.dat";
    
    FILE *fp = fopen(filename, "wb");
    if (!fp) {
        perror("File open error");
        return;
    }
    
    fprintf(fp, "=== UDP Packet Dump ===\n");
    fprintf(fp, "Timestamp: %ld\n", time(NULL));
    fprintf(fp, "Source IP: %s\n", inet_ntoa(*(struct in_addr *)&iph->saddr));
    fprintf(fp, "Dest IP: %s\n", inet_ntoa(*(struct in_addr *)&iph->daddr));
    fprintf(fp, "Packet size: %d bytes\n", size);
    fprintf(fp, "=== Raw Data ===\n");
    
    fwrite(data, 1, size, fp);
    fclose(fp);
    
    printf("Package dump saved in: %s\n", filename);
}

void process_udp_packet(unsigned char *buffer, int size) {
    struct iphdr *iph = (struct iphdr *)buffer;
    unsigned short iphdrlen = iph->ihl * 4;
    
    struct udphdr *udph = (struct udphdr*)(buffer + iphdrlen);
    
    if(ntohs(udph->dest) == SERVER_PORT) {
        printf("\n\nCaught UDP package:\n");
        printf("From: %s:%d\n", inet_ntoa(*(struct in_addr *)&iph->saddr), ntohs(udph->source));
        printf("To: %s:%d\n", inet_ntoa(*(struct in_addr *)&iph->daddr), ntohs(udph->dest));
        printf("Size: %d bytes\n", size);
        
        int header_size = iphdrlen + sizeof(struct udphdr);
        int data_size = size - header_size;
        
        if(data_size > 0) {
            printf("Data (%d byte):\n", data_size);
            printf("Text: ");
            for(int i = header_size; i < size; i++) {
                if(buffer[i] >= 32 && buffer[i] <= 126) {
                    printf("%c", buffer[i]);
                } else {
                    printf("\\x%02x", buffer[i]);
                }
            }
            printf("\n");
            
            save_packet_dump(buffer, size, iph);
        }
    }
}
