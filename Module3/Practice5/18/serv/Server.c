#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <dirent.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/select.h>

#define BUFFER_SIZE 1024


void error(const char *msg) {
    perror(msg);
    exit(1);
}

void send_file(int sock, const char *filename);
void receive_file(int sock, const char *filename);
void handle_client(int sock, char* buffer);

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    int listener, newsockfd, portno;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;
    
    listener = socket(AF_INET, SOCK_STREAM, 0);
    if (listener < 0) error("ERROR opening socket");
    
    memset(&serv_addr, 0, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    
    if (bind(listener, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
        error("ERROR on binding");
    
    listen(listener, 5);
    clilen = sizeof(cli_addr);
    
    printf("Server started on port %d\n", portno);

    fd_set master, read_fds;
    FD_ZERO(&master);
    FD_ZERO(&read_fds);

    FD_SET(listener, &master);
    int FD_MAX = listener;
    
    while (1) {
        read_fds = master;
        if (select(FD_MAX + 1, &read_fds, NULL, NULL, NULL) == -1) error("Error in select");    // ждем событие на сокете

        for (int i = 0; i <= FD_MAX; i++) {
            if (FD_ISSET(i, &read_fds)) {
                if (i == listener) {    // фиксируем новое подключение
                    newsockfd = accept(listener, (struct sockaddr*)&cli_addr, &clilen);
                    printf("New connection from %s\n", inet_ntoa(cli_addr.sin_addr));
                    FD_SET(newsockfd, &master);
                    if (newsockfd > FD_MAX) FD_MAX = newsockfd;
                }
                else {  // обрабатываем существующего клиента
                    char buffer[BUFFER_SIZE];
                    memset(buffer, 0, sizeof(buffer));
                    if (recv(i, buffer, BUFFER_SIZE, 0) <= 0) {
                        close(i);
                        FD_CLR(i, &master); // удаляем из множества после отключения
                    }
                    else {
                        handle_client(i, buffer);
                    }
                    
                }
            }
        }
    }
    
    close(listener);
    return 0;
}

void send_file(int sock, const char *filename) {
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;
    int file_fd = open(filename, O_RDONLY);
    
    if (file_fd < 0) {
        strcpy(buffer, "FILE_NOT_FOUND");
        send(sock, buffer, strlen(buffer), 0);
        return;
    }
    
    while ((bytes_read = read(file_fd, buffer, BUFFER_SIZE)) > 0) {
        send(sock, buffer, bytes_read, 0);
    }
    
    close(file_fd);
}

void receive_file(int sock, const char *filename) {
    char buffer[BUFFER_SIZE];
    ssize_t bytes_received;
    int file_fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    
    bytes_received = recv(sock, buffer, BUFFER_SIZE, 0);
    write(file_fd, buffer, bytes_received);

    close(file_fd);
}

void handle_client(int sock, char* buffer) {
    if (strncmp(buffer, "DOWNLOAD", 8) == 0) {
        char *filename = buffer + 9;
        send_file(sock, filename);
    }
    else if (strncmp(buffer, "UPLOAD", 6) == 0) {
        char *filename = buffer + 7;
        receive_file(sock, filename);
        sprintf(buffer, "File %s uploaded successfully", filename);
        send(sock, "File has been uploaded", 22, 0);
    }
    else if (strncmp(buffer, "CALC", 4) == 0) {
        double a, b, result;
        char op;
        sscanf(buffer + 5, "%lf %c %lf", &a, &op, &b);
        
        switch(op) {
            case '+': result = a + b; break;
            case '-': result = a - b; break;
            case '*': result = a * b; break;
            case '/': result = (b != 0) ? a / b : 0; break;
            default: result = 0;
        }
        
        sprintf(buffer, "Result: %.2f", result);
        send(sock, buffer, strlen(buffer), 0);
    }
    else if (strncmp(buffer, "QUIT", 4) == 0) {
       printf("Client leaved\n");
    }
}