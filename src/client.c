#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 5000
#define BUFFER_SIZE 256

int main(int argc, char *argv[]) {
    int sock = 0;
    struct sockaddr_in server_address;
    char buffer[BUFFER_SIZE];

    if (argc < 2) {
        printf("Usage: %s \"order description\"\n", argv[0]);
        printf("Example: %s \"Pizza pepperoni\"\n", argv[0]);
        return 1;
    }

    sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock < 0) {
        perror("socket creation error");
        return 1;
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr) <= 0) {
        perror("invalid address");
        close(sock);
        return 1;
    }

    if (connect(sock, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("connection failed");
        close(sock);
        return 1;
    }

    write(sock, argv[1], strlen(argv[1]));

    memset(buffer, 0, BUFFER_SIZE);
    read(sock, buffer, BUFFER_SIZE - 1);

    printf("%s", buffer);

    close(sock);
    return 0;
}
