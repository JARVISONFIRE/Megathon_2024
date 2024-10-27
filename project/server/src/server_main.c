// server_main.c
#include "networking.h"
#include "gamelogic.h"
#include <stdio.h>
#include <pthread.h>
#include <signal.h>
#include <time.h>

#define TICK_RATE 60
#define TICK_TIME (1.0f / TICK_RATE)

static GameState gameState;
static int serverSocket;
static pthread_t gameThread;
static bool running = true;
static pthread_mutex_t stateMutex = PTHREAD_MUTEX_INITIALIZER;

void* gameLoop(void* arg) {
    struct timespec last_time, current_time;
    clock_gettime(CLOCK_MONOTONIC, &last_time);

    while (running) {
        // Calculate delta time
        clock_gettime(CLOCK_MONOTONIC, &current_time);
        float deltaTime = (current_time.tv_sec - last_time.tv_sec) +
                         (current_time.tv_nsec - last_time.tv_nsec) / 1e9;
        last_time = current_time;

        // Update game state
        pthread_mutex_lock(&stateMutex);
        updateGameState(&gameState, deltaTime);
        
        // Serialize and broadcast game state to all clients
        char* serializedState = serializeGameState(&gameState);
        // TODO: Broadcast to all clients
        free(serializedState);
        
        pthread_mutex_unlock(&stateMutex);

        // Sleep to maintain tick rate
        struct timespec sleep_time;
        sleep_time.tv_sec = 0;
        sleep_time.tv_nsec = (long)(TICK_TIME * 1e9);
        nanosleep(&sleep_time, NULL);
    }
    return NULL;
}

void handleClientMessage(int clientSocket, const char* message) {
    // TODO: Parse message and update game state
    pthread_mutex_lock(&stateMutex);
    // Update game state based on message
    pthread_mutex_unlock(&stateMutex);
}

void sigintHandler(int sig) {
    running = false;
    pthread_join(gameThread, NULL);
    close(serverSocket);
    exit(0);
}

int main(int argc, char* argv[]) {
    signal(SIGINT, sigintHandler);
    srand(time(NULL));

    // Initialize game state
    initializeGameState(&gameState);

    // Create server socket
    serverSocket = create_server(8080);
    printf("Server started on port 8080\n");

    // Start game loop thread
    pthread_create(&gameThread, NULL, gameLoop, NULL);

    // Main loop to accept clients
    while (running) {
        int clientSocket = accept_client(serverSocket);
        if (clientSocket < 0) continue;

        // Handle new client connection
        pthread_mutex_lock(&stateMutex);
        int playerID = addPlayer(&gameState);
        pthread_mutex_unlock(&stateMutex);

        if (playerID >= 0) {
            printf("Player %d connected\n", playerID);
            // TODO: Send initial game state to client
        } else {
            printf("Server full, rejecting client\n");
            close_connection(clientSocket);
        }
    }

    return 0;
}

// Makefile
CC = gcc
CFLAGS = -Wall -Wextra -g -I/usr/include/json-c
LDFLAGS = -lpthread -ljson-c

SERVER_SRCS = server_main.c networking.c gamelogic.c
SERVER_OBJS = $(SERVER_SRCS:.c=.o)
SERVER_TARGET = game_server

.PHONY: all clean

all: $(SERVER_TARGET)

$(SERVER_TARGET): $(SERVER_OBJS)
	$(CC) $(SERVER_OBJS) -o $(SERVER_TARGET) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(SERVER_OBJS) $(SERVER_TARGET)

// game_render.c
#include "game_render.h"
#include <string.h>

// Asset handles
static Texture2D backgroundDay;
static Texture2D backgroundNight;
static Texture2D castleTexture;
static Texture2D pumpkinTexture;
static Texture2D zombieTexture;
static Texture2D bulletTexture;
static Sound hitSound;
static Sound pumpkinPlaceSound;
static Sound shootSound;
static Sound towerAttackSound;
static Sound backgroundMusic;

void init_renderer(void) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Tower Defense Multiplayer");
    SetTargetFPS(60);
    load_game_assets();
}

void load_game_assets(void) {
    backgroundDay = LoadTexture("assets/images/background_day.png");
    backgroundNight = LoadTexture("assets/images/background_night.png");
    castleTexture = LoadTexture("assets/images/castle.png");
    pumpkinTexture = LoadTexture("assets/images/pumpkin.png");
    zombieTexture = LoadTexture("assets/images/zombie.png");
    bulletTexture = LoadTexture("assets/images/bullet.png");
    
    hitSound = LoadSound("assets/sounds/hit.wav");
    pumpkinPlaceSound = LoadSound("assets/sounds/pumpkin_place.wav");
    shootSound = LoadSound("assets/sounds/shoot.wav");
    towerAttackSound = LoadSound("assets/sounds/tower_attack.wav");
    backgroundMusic = LoadSound("assets/sounds/background.mp3");
}

void render_game_state(GameState *state) {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    // Draw background based on time of day
    DrawTexture(state->is_daytime ? backgroundDay : backgroundNight, 0, 0, WHITE);

    // Draw towers
    for (int i = 0; i < MAX_TOWERS; i++) {
        if (state->tower_active[i]) {
            DrawTexture(castleTexture, 
                       state->tower_positions[i].x - 16, 
                       state->tower_positions[i].y - 16, 
                       WHITE);
        }
    }

    // Draw pumpkins
    for (int i = 0; i < MAX_PUMPKINS; i++) {
        if (state->pumpkin_active[i]) {
            DrawTexture(pumpkinTexture, 
                       state->pumpkin_positions[i].x - 16, 
                       state->pumpkin_positions[i].y - 16, 
                       WHITE);
        }
    }

    // Draw enemies
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (state->enemy_active[i]) {
            DrawTexture(zombieTexture, 
                       state->enemy_positions[i].x - 16, 
                       state->enemy_positions[i].y - 16, 
                       WHITE);
        }
    }

    // Draw bullets
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (state->bullet_active[i]) {
            DrawTexture(bulletTexture, 
                       state->bullet_positions[i].x - 4, 
                       state->bullet_positions[i].y - 4, 
                       WHITE);
        }
    }

    draw_ui(state);
    EndDrawing();
}

void draw_ui(GameState *state) {
    DrawText(TextFormat("Enemies Killed: %d", state->enemies_killed), 10, 10, 20, BLACK);
    DrawText(TextFormat("Points: %d", state->points), 10, 40, 20, BLACK);
    DrawText(TextFormat("Game Time: %.1f", state->game_timer), 10, 70, 20, BLACK);
    DrawText(state->is_daytime ? "Daytime" : "Nighttime", 10, 100, 20, 
             state->is_daytime ? YELLOW : BLUE);

    if (state->game_over) {
        const char* message = state->game_won ? 
            "Congratulations! You survived!" : 
            "Game Over! The tower was destroyed!";
        DrawText(message, SCREEN_WIDTH/2 - 200, SCREEN_HEIGHT/2, 30, 
                state->game_won ? GREEN : RED);
    }
}

void handle_input(GameState *state) {
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        Vector2 mousePos = GetMousePosition();
        // Send place pumpkin message to server
    }

    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
        Vector2 mousePos = GetMousePosition();
        // Send shoot bullet message to server
    }
}

void serialize_game_state(GameState *state, char *buffer) {
    memcpy(buffer, state, sizeof(GameState));
}

void deserialize_game_state(const char *buffer, GameState *state) {
    memcpy(state, buffer, sizeof(GameState));
}

void cleanup_renderer(void) {
    unload_game_assets();
    CloseWindow();
}

void unload_game_assets(void) {
    UnloadTexture(backgroundDay);
    UnloadTexture(backgroundNight);
    UnloadTexture(castleTexture);
    UnloadTexture(pumpkinTexture);
    UnloadTexture(zombieTexture);
    UnloadTexture(bulletTexture);
    UnloadSound(hitSound);
    UnloadSound(pumpkinPlaceSound);
    UnloadSound(shootSound);
    UnloadSound(towerAttackSound);
    UnloadSound(backgroundMusic);
}
