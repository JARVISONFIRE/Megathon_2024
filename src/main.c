// main.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../include/network.h"  // Include the custom networking library

int main() {
    printf("Starting the game...\n");
    
    // Example: Create a server socket on port 8080
    int server_socket = create_server_socket(8080);
    if (server_socket < 0) {
        fprintf(stderr, "Error creating server socket.\n");
        return 1;
    }

    printf("Server is running. Waiting for client...\n");
    
    // Accept a client connection
    int client_socket = accept_client(server_socket);
    if (client_socket < 0) {
        fprintf(stderr, "Error accepting client.\n");
        close_socket(server_socket);
        return 1;
    }

    printf("Client connected!\n");

    // Close sockets
    close_socket(client_socket);
    close_socket(server_socket);
    
    return 0;
}
