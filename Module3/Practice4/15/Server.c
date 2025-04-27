#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <ctype.h>
#include <arpa/inet.h>

double calculate(double a, double b, char op);
void dostuff(int);

void error(const char *msg) {
    perror(msg);
    exit(1);
}

int nclients = 0;

void printusers() { 
    if(nclients) {
        printf("%d user(s) on-line\n", nclients);
    } else {
        printf("No users online\n");
    }
}

int main(int argc, char *argv[]) {
    printf("TCP MATH SERVER\n");
    
    if(argc < 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    int sockfd, newsockfd, portno, pid;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0) error("ERROR opening socket");
     
    memset(&serv_addr, 0, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    
    if(bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        error("ERROR on binding\n");
    }
    
    listen(sockfd, 5);
    clilen = sizeof(cli_addr);
    
    while(1) {
        newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
        if(newsockfd < 0) error("ERROR on accept\n");
        
        nclients++;
        printf("New connection from %s\n", inet_ntoa(cli_addr.sin_addr));
        printusers();
        
        pid = fork();
        if(pid < 0) error("ERROR on fork");
        
        if(pid == 0) {
            close(sockfd);
            dostuff(newsockfd);
            exit(0);
        } else {
            close(newsockfd);
        }
    }
    
    close(sockfd);
    return 0;
}

double calculate(double a, double b, char op) {
    switch(op) {
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '/': 
            if(b == 0) {
                return 0; // Обработка деления на 0
            }
            return a / b;
        default: return 0;
    }
}

void dostuff(int sock) {
    char buff[1024];
    char op;
    double a, b, result;
    int bytes_recv;
    
    const char *prompts[] = {
        "Enter first number: ",
        "Enter second number: ",
        "Enter operation (+, -, *, /): "
    };
    
    // Получаем первое число
    write(sock, prompts[0], strlen(prompts[0]));
    bytes_recv = read(sock, buff, sizeof(buff));
    if(bytes_recv < 0) error("ERROR reading from socket\n");
    a = atof(buff);
    
    // Получаем второе число
    write(sock, prompts[1], strlen(prompts[1]));
    bytes_recv = read(sock, buff, sizeof(buff));
    if(bytes_recv < 0) error("ERROR reading from socket\n");
    b = atof(buff);
    
    // Получаем операцию
    write(sock, prompts[2], strlen(prompts[2]));
    bytes_recv = read(sock, buff, sizeof(buff));
    if(bytes_recv < 0) error("ERROR reading from socket\n");
    op = buff[0];
    
    // Вычисляем результат
    result = calculate(a, b, op);
    
    // Форматируем и отправляем ответ
    if (result == 0 && op == '/') snprintf(buff, sizeof(buff), "Can't devide by 0\n");
    else snprintf(buff, sizeof(buff), "Result: %.2f\n", result);
    write(sock, buff, strlen(buff));
    
    nclients--;
    printf("Client disconnected\n");
    printusers();
}