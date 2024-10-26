#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "network.h"

#define MAX_CLIENTS 3

void *client_handler(void *socket_desc) {
    int sock = *(int *)socket_desc;
    free(socket_desc); // Free the dynamically allocated memory

    char buffer[1024];
    int bytes_received;

    while ((bytes_received = receive_data(sock, buffer, sizeof(buffer))) > 0) {
        // Echo the received data back to the client
        send_data(sock, buffer, bytes_received);
    }

    close_socket(sock);
    return NULL;
}

int main() {
    const int screenWidth = 800;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "Networked Raylib Game");
    SetTargetFPS(60);

    printf("Starting the game...\n");

    int server_socket = create_server_socket(8080);
    if (server_socket < 0) {
        fprintf(stderr, "Error creating server socket.\n");
        CloseWindow();
        return 1;
    }

    printf("Server is running. Waiting for clients...\n");

    pthread_t client_threads[MAX_CLIENTS];
    int client_count = 0;

    while (client_count < MAX_CLIENTS) {
        int *client_socket = malloc(sizeof(int));
        *client_socket = accept_client(server_socket);
        if (*client_socket < 0) {
            fprintf(stderr, "Error accepting client.\n");
            free(client_socket);
            break;
        }

        printf("Client connected!\n");

        if (pthread_create(&client_threads[client_count], NULL, client_handler, (void *)client_socket) < 0) {
            perror("Could not create thread");
            free(client_socket);
            return 1;
        }

        client_count++;
    }

    for (int i = 0; i < client_count; i++) {
        pthread_join(client_threads[i], NULL);
    }

    close_socket(server_socket);
    CloseWindow();

    return 0;
}
