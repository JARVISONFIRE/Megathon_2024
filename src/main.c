#include <raylib.h>  // Include the Raylib library
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h> // Include pthread for multi-threading
#include "../include/network.h"  // Include the custom networking library

#define MAX_CLIENTS 3

void *client_handler(void *socket_desc) {
    int sock = *(int*)socket_desc;
    // Handle client communication here (e.g., receive/send data)
    close_socket(sock); // Close the socket after handling
    return NULL;
}

int main() {
    // Initialization
    const int screenWidth = 800;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "Networked Raylib Game");
    SetTargetFPS(60);  

    printf("Starting the game...\n");

    // Create a server socket on port 8080
    int server_socket = create_server_socket(8080);
    if (server_socket < 0) {
        fprintf(stderr, "Error creating server socket.\n");
        CloseWindow();  
        return 1;
    }

    printf("Server is running. Waiting for clients...\n");

    pthread_t client_threads[MAX_CLIENTS];
    int client_sockets[MAX_CLIENTS];
    int client_count = 0;

    while (client_count < MAX_CLIENTS) {
        int client_socket = accept_client(server_socket);
        if (client_socket < 0) {
            fprintf(stderr, "Error accepting client.\n");
            break;
        }

        printf("Client connected!\n");
        client_sockets[client_count] = client_socket;

        // Create a thread for each client
        if (pthread_create(&client_threads[client_count], NULL, client_handler, (void *)&client_sockets[client_count]) < 0) {
            perror("Could not create thread");
            return 1;
        }

        client_count++;
    }

    // Wait for all threads to finish
    for (int i = 0; i < client_count; i++) {
        pthread_join(client_threads[i], NULL);
    }

    // Close sockets
    close_socket(server_socket);
    
    // De-Initialization
    CloseWindow();  

    return 0;
}
