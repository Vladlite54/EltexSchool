#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>

#define BUFFER_SIZE 1024

void error(const char *msg) {
    perror(msg);
    exit(0);
}

void clear_stdin() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

void download_file(int sock, const char *filename);
void upload_file(int sock, const char *filename);
void calculate(int sock);

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <hostname> <port>\n", argv[0]);
        exit(0);
    }
    
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buffer[BUFFER_SIZE];
    
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) error("ERROR opening socket\n");
    
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }
    
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    memcpy(&serv_addr.sin_addr.s_addr, server->h_addr_list[0], server->h_length);
    serv_addr.sin_port = htons(portno);
    
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR connecting");
    
    printf("Connected to server. Commands:\n");
    printf("DOWNLOAD <filename> - download file from server\n");
    printf("UPLOAD <filename> - upload file to server\n");
    printf("CALC - perform calculation\n");
    printf("QUIT - exit\n");
    
    while (1) {
        printf("> ");
        fgets(buffer, BUFFER_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = 0;
        
        if (strncmp(buffer, "DOWNLOAD", 8) == 0) {
            download_file(sockfd, buffer + 9);
        }
        else if (strncmp(buffer, "UPLOAD", 6) == 0) {
            upload_file(sockfd, buffer + 7);
        }
        else if (strncmp(buffer, "CALC", 4) == 0) {
            calculate(sockfd);
        }
        else if (strncmp(buffer, "QUIT", 4) == 0) {
            send(sockfd, "QUIT", 4, 0);
            break;
        }
        else {
            printf("Unknown command\n");
        }
    }
    
    close(sockfd);
    return 0;
}

void download_file(int sock, const char *filename) {
    char buffer[BUFFER_SIZE];
    int file_fd;
    ssize_t bytes_received;
    
    sprintf(buffer, "DOWNLOAD %s", filename);
    send(sock, buffer, strlen(buffer), 0);
        
    bytes_received = recv(sock, buffer, BUFFER_SIZE, 0);
    
    if (strncmp(buffer, "FILE_NOT_FOUND", 14) == 0) {
        printf("File not found on server\n");
        close(file_fd);
        return;
    }

    file_fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    write(file_fd, buffer, bytes_received);
    close(file_fd);
    printf("File %s downloaded successfully\n", filename);
}

void upload_file(int sock, const char *filename) {
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;
    int file_fd = open(filename, O_RDONLY);
    
    if (file_fd < 0) {
        printf("File %s not found\n", filename);
        return;
    }
    
    sprintf(buffer, "UPLOAD %s", filename);
    send(sock, buffer, strlen(buffer), 0);
    
    usleep(1000);
    while ((bytes_read = read(file_fd, buffer, BUFFER_SIZE)) > 0) {
        send(sock, buffer, bytes_read, 0);
    }
    
    close(file_fd);
    
    memset(buffer, 0, sizeof(buffer));
    recv(sock, buffer, BUFFER_SIZE, 0);
    printf("%s\n", buffer);
}

void calculate(int sock) {
    char buffer[BUFFER_SIZE];
    double a, b;
    char op;
    
    printf("Enter first number: ");
    scanf("%lf", &a);
    printf("Enter second number: ");
    scanf("%lf", &b);
    printf("Enter operation (+, -, *, /): ");
    scanf(" %c", &op);
    
    sprintf(buffer, "CALC %lf %c %lf", a, op, b);
    send(sock, buffer, strlen(buffer), 0);
    
    memset(buffer, 0, sizeof(buffer));
    recv(sock, buffer, BUFFER_SIZE, 0);
    printf("%s\n", buffer);
    clear_stdin();
}