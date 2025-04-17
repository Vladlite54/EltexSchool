#include <stdio.h>
#include "./menu/Menu.h"

int main() {
    menu();
    // int fd = open("./test.dat", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    // if (fd == -1) {
    //     perror("Open error\n");
    // }
    // size_t sz = 5;
    // if (write(fd, &sz, sizeof(size_t)) != sizeof(size_t)) {
    //     perror("Size write error\n");
    //     close(fd);
    // }
    // close(fd);

    // int fdd = open("./test.dat", O_RDONLY);
    // if (fdd == -1) {
    //     perror("Open error\n");
    // }
    // size_t size;
    // if (read(fdd, &size, sizeof(size_t)) != sizeof(size_t)) {
    //     perror("Size read error");
    //     close(fdd);
    // }
    // printf("%d\n", size);
    return 0;
}