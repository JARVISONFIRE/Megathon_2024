// client_main.c
#include "networking.h"
#include "game_render.h"
#include <pthread.h>

#define SERVER_PORT 8080

typedef struct GameState {
    Enemy enemies[MAX_ENEMIES];
    Pumpkin pumpkins[MAX_PUMPKINS];
    Tower towers[MAX_TOWERS];
    Bullet bullets[MAX_BULLETS];
    int points;
    bool isDaytime;
    bool gameOver;
    float gameTimer;
    int enemiesKilled;
    pthread_mutex_t mutex;
} GameState;

GameState gameState;

void* network_thread(void* arg) {
    int client_socket = (int)arg;
    while (!gameState.gameOver) {
        char* msg = receive_message(client_socket);
        if (msg) {
            pthread_mutex_lock(&gameState.mutex);
            // Parse network message and update game state
            memcpy(&gameState, msg, sizeof(GameState));
            pthread_mutex_unlock(&gameState.mutex);
            free(msg);
        }
    }
    return NULL;
}

int main() {
    int client_socket;
    struct sockaddr_in server_addr;
    pthread_t network_thread_id;

    // Initialize game state
    pthread_mutex_init(&gameState.mutex, NULL);
    InitGameRenderer();

    // Connect to server
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        perror("Failed to create socket");
        return -1;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr); // Replace with your server's IP

    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Failed to connect to server");
        close_connection(client_socket);
        return -1;
    }

    // Start network thread
    pthread_create(&network_thread_id, NULL, network_thread, &client_socket);

    // Main game loop
    while (!WindowShouldClose() && !gameState.gameOver) {
        pthread_mutex_lock(&gameState.mutex);
        
        // Handle local input
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            Vector2 mousePos = GetMousePosition();
            char input_msg[32];
            sprintf(input_msg, "CLICK:%f,%f", mousePos.x, mousePos.y);
            send_message(client_socket, input_msg);
        }

        // Render game state
        RenderGame(&gameState);
        
        pthread_mutex_unlock(&gameState.mutex);
    }

    // Cleanup
    pthread_mutex_destroy(&gameState.mutex);
    close_connection(client_socket);
    CloseGameRenderer();

    return 0;
}
