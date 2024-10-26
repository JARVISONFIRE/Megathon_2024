// main.c
#include <raylib.h>  // Include the Raylib library
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../include/network.h"  // Include the custom networking library

int main() {
    // Initialization
    const int screenWidth = 800;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "Networked Raylib Game");  // Initialize the game window
    SetTargetFPS(60);  // Set the game to run at 60 frames per second

    printf("Starting the game...\n");

    // Example: Create a server socket on port 8080
    int server_socket = create_server_socket(8080);
    if (server_socket < 0) {
        fprintf(stderr, "Error creating server socket.\n");
        CloseWindow();  // Clean up Raylib resources before exiting
        return 1;
    }

    printf("Server is running. Waiting for client...\n");

    // Accept a client connection
    int client_socket = accept_client(server_socket);
    if (client_socket < 0) {
        fprintf(stderr, "Error accepting client.\n");
        close_socket(server_socket);
        CloseWindow();  // Clean up Raylib resources before exiting
        return 1;
    }

    printf("Client connected!\n");

    // Main game loop
    while (!WindowShouldClose()) {  // Detect window close button or ESC key
        // Update
        // (You can add networking-related updates here if needed)

        // Draw
        BeginDrawing();
        ClearBackground(RAYWHITE);  // Clear the screen with a white background

        DrawText("Welcome to the networked Raylib game!", 150, 200, 20, LIGHTGRAY);  // Display some text on the screen
        DrawText("Client connected to the server!", 170, 240, 20, GREEN);

        EndDrawing();
    }

    // Close sockets
    close_socket(client_socket);
    close_socket(server_socket);

    // De-Initialization
    CloseWindow();  // Close the Raylib window and unload resources

    return 0;
}
