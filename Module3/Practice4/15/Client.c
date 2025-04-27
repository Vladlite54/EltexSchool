#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

void error(const char *msg) {
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[]) {
    if(argc < 3) {
        fprintf(stderr, "Usage: %s <hostname> <port>\n", argv[0]);
        exit(0);
    }

    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buffer[256];
    
    printf("TCP MATH CLIENT\n");
    
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0) error("ERROR opening socket\n");
    
    server = gethostbyname(argv[1]);
    if(server == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }
    
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    memcpy(&serv_addr.sin_addr.s_addr, server->h_addr_list[0], server->h_length);
    serv_addr.sin_port = htons(portno);
    
    if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        error("ERROR connecting\n");
    }
    
    while(1) {
        memset(buffer, 0, sizeof(buffer));
        n = read(sockfd, buffer, sizeof(buffer)-1);
        if(n < 0) error("ERROR reading from socket\n");
        
        printf("%s", buffer);
        
        memset(buffer, 0, sizeof(buffer));
        fgets(buffer, sizeof(buffer)-1, stdin);
        
        if(strncmp(buffer, "quit", 4) == 0) {
            printf("Exiting...\n");
            break;
        }
        
        n = write(sockfd, buffer, strlen(buffer));
        if(n < 0) error("ERROR writing to socket\n");
    }
    
    close(sockfd);
    return 0;
}