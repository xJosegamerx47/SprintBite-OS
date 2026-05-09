#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 5000
#define BUFFER_SIZE 256
#define MAX_PENDING 10

pthread_mutex_t stats_mutex = PTHREAD_MUTEX_INITIALIZER;

int total_orders_received = 0;
int active_clients = 0;
int server_running = 1;

void *handle_client(void *arg) {
    int client_socket = *(int *)arg;
    free(arg);

    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);

    pthread_mutex_lock(&stats_mutex);
    active_clients++;
    int current_active = active_clients;
    pthread_mutex_unlock(&stats_mutex);

    int bytes_read = read(client_socket, buffer, BUFFER_SIZE - 1);

    if (bytes_read > 0) {
        buffer[bytes_read] = '\0';

        printf("[SERVER][CONNECT] New client connected | active clients=%d\n", current_active);
        printf("[SERVER][MESSAGE] Client sent: \"%s\"\n", buffer);
        fflush(stdout);

        if (strncmp(buffer, "exit", 4) == 0) {
            const char *response = "[SERVER] Shutdown signal received.\n";
            ssize_t sent = write(client_socket, response, strlen(response));
            (void)sent;

            pthread_mutex_lock(&stats_mutex);
            server_running = 0;
            pthread_mutex_unlock(&stats_mutex);

            printf("[SERVER][SHUTDOWN] Shutdown requested by client.\n");
            fflush(stdout);
        } else {
            pthread_mutex_lock(&stats_mutex);
            total_orders_received++;
            int order_number = total_orders_received;
            pthread_mutex_unlock(&stats_mutex);

            printf("[SERVER][ORDER] Order #%d registered: %s\n", order_number, buffer);
            fflush(stdout);

            char response[BUFFER_SIZE];
            snprintf(response, BUFFER_SIZE,
                     "[SERVER] Order #%d accepted: %s\n",
                     order_number, buffer);

            ssize_t sent = write(client_socket, response, strlen(response));
            (void)sent;

            printf("[SERVER][RESPONSE] Confirmation sent for order #%d\n", order_number);
            fflush(stdout);
        }

        pthread_mutex_lock(&stats_mutex);
        active_clients--;
        current_active = active_clients;
        pthread_mutex_unlock(&stats_mutex);

        printf("[SERVER][DISCONNECT] Client disconnected | active clients=%d\n", current_active);
        fflush(stdout);
    } else {
        /*
         * Silent health-check connection.
         * The PHP GUI opens a TCP connection to verify if the server is alive,
         * but it does not send an order. This is normal and should not appear
         * as a warning in the server output.
         */
        pthread_mutex_lock(&stats_mutex);
        active_clients--;
        pthread_mutex_unlock(&stats_mutex);
    }

    close(client_socket);
    return NULL;
}

int main() {
    int server_fd;
    struct sockaddr_in address;
    int opt = 1;
    socklen_t addrlen = sizeof(address);

    signal(SIGPIPE, SIG_IGN);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_fd < 0) {
        perror("[SERVER][ERROR] socket failed");
        exit(EXIT_FAILURE);
    }

    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("[SERVER][ERROR] bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, MAX_PENDING) < 0) {
        perror("[SERVER][ERROR] listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("============================================================\n");
    printf(" SprintBite OS - INTERACTIVE CLIENT/SERVER MODULE\n");
    printf(" TCP server listening on port %d\n", PORT);
    printf(" Waiting for clients...\n");
    printf(" Send orders using: ./scripts/run_client.sh \"Pizza\"\n");
    printf(" Stop server using: ./scripts/run_client.sh \"exit\"\n");
    printf("============================================================\n");
    fflush(stdout);

    while (server_running) {
        int *client_socket = malloc(sizeof(int));

        if (!client_socket) {
            perror("[SERVER][ERROR] malloc failed");
            continue;
        }

        *client_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen);

        if (*client_socket < 0) {
            free(client_socket);
            if (server_running) {
                perror("[SERVER][ERROR] accept failed");
            }
            continue;
        }

        pthread_t client_thread;
        pthread_create(&client_thread, NULL, handle_client, client_socket);
        pthread_detach(client_thread);
    }

    sleep(1);

    printf("----------------------------- SUMMARY -----------------------\n");
    printf("Total orders received:       %d\n", total_orders_received);
    printf("Active clients at shutdown:  %d\n", active_clients);
    printf("Communication model:         CLIENT-SERVER TCP SOCKETS\n");
    printf("Concurrency model:           ONE THREAD PER CLIENT\n");
    printf("Synchronization status:      SERVER STATS PROTECTED WITH MUTEX\n");
    printf("-------------------------------------------------------------\n");
    fflush(stdout);

    close(server_fd);
    return 0;
}
