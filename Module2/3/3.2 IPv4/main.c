#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>

// Структура для представления IP-адреса в виде четырех октетов
typedef struct {
    unsigned char octet[4];
} IPv4Address;

// Преобразование строки в IP адрес
int string_to_ipv4(const char *ip_str, IPv4Address *ip) {
    int octet_index = 0;
    int num;
    char *token;
    char ip_str_copy[strlen(ip_str) + 1];
    strcpy(ip_str_copy, ip_str);

    token = strtok(ip_str_copy, ".");

    while (token != NULL && octet_index < 4) {
        num = atoi(token);
        if (num < 0 || num > 255) {
            return -1;
        }
        ip->octet[octet_index] = (unsigned char)num;
        octet_index++;
        token = strtok(NULL, ".");
    }

    if (octet_index != 4) {
        return -1;
    }
    return 0;
}

// Преобразование IP-адреса в строку
char *ipv4_to_string(const IPv4Address *ip, char *str) {
    if (ip == NULL || str == NULL) {
        return NULL;
    }

    sprintf(str, "%d.%d.%d.%d", ip->octet[0], ip->octet[1], ip->octet[2], ip->octet[3]);
    return str;
}

// Преобразование IPv4Address в uint32_t
uint32_t ipv4_to_uint32(const IPv4Address *ip) {
    uint32_t result = 0;
    for (int i = 0; i < 4; i++) {
        result = (result << 8) | ip->octet[i];
    }
    return result;
}


// Преобразование uint32_t в IPv4Address
void uint32_to_ipv4(uint32_t ip_int, IPv4Address *ip) {
    for (int i = 3; i >= 0; i--) {
        ip->octet[i] = (ip_int & 0xFF);
        ip_int >>= 8;
    }
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Нужны аргументы: %s <адрес_шлюза> <маска_подсети> <количество_пакетов>\n", argv[0]);
        return 1;
    }

    char *gateway_ip_str = argv[1];
    char *subnet_mask_str = argv[2];
    int num_packets = atoi(argv[3]);

    IPv4Address gateway_ip;
    IPv4Address subnet_mask;
    if (string_to_ipv4(gateway_ip_str, &gateway_ip) != 0) {
        fprintf(stderr, "Ошибка: Некорректный IP-адрес шлюза: %s\n", gateway_ip_str);
        return 1;
    }
    if (string_to_ipv4(subnet_mask_str, &subnet_mask) != 0) {
        fprintf(stderr, "Ошибка: Некорректная маска подсети: %s\n", subnet_mask_str);
        return 1;
    }

    uint32_t gateway_ip_long = ipv4_to_uint32(&gateway_ip);
    uint32_t subnet_mask_long = ipv4_to_uint32(&subnet_mask);


    uint32_t network_address_long = gateway_ip_long & subnet_mask_long;

    IPv4Address network_address;
    uint32_to_ipv4(network_address_long, &network_address);
    char network_address_str[16];
    ipv4_to_string(&network_address, network_address_str);

    srand(time(NULL));

    int packets_in_subnet = 0;
    int packets_out_subnet = 0;

    printf("Шлюз: %s\n", gateway_ip_str);
    printf("Маска подсети: %s\n", subnet_mask_str);
    printf("Адрес сети: %s\n", network_address_str);
    printf("Количество пакетов: %d\n", num_packets);
    printf("\n");

    for (int i = 0; i < num_packets; i++) {
        uint32_t octet1 = rand() % 256;
        uint32_t octet2 = rand() % 256;
        uint32_t octet3 = rand() % 256;
        uint32_t octet4 = rand() % 256;
        uint32_t destination_ip_long = (octet1 << 24) | (octet2 << 16) | (octet3 << 8) | octet4;

        IPv4Address destination_ip;
        uint32_to_ipv4(destination_ip_long, &destination_ip);

        char destination_ip_str[16];
        ipv4_to_string(&destination_ip, destination_ip_str);

        printf("Пакет %d: IP назначения: %s ", i + 1, destination_ip_str);

        uint32_t destination_ip_long_network = destination_ip_long & subnet_mask_long;

        if (destination_ip_long_network == network_address_long) {
            packets_in_subnet++;
            printf("(Своя подсеть)\n");
        } else {
            packets_out_subnet++;
            printf("(Чужая подсеть)\n");
        }
    }

    double in_subnet_percentage = (double)packets_in_subnet / num_packets * 100.0;
    double out_subnet_percentage = (double)packets_out_subnet / num_packets * 100.0;

    printf("\nСтатистика:\n");
    printf("Пакетов в своей подсети: %d (%.2lf%%)\n", packets_in_subnet, in_subnet_percentage);
    printf("Пакетов в чужой подсети: %d (%.2lf%%)\n", packets_out_subnet, out_subnet_percentage);

    return 0;
}