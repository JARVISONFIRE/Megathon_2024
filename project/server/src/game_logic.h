// gamelogic.h
#ifndef GAMELOGIC_H
#define GAMELOGIC_H

#include <stdbool.h>
#include <stdint.h>

#define MAX_PLAYERS 4
#define MAX_ENEMIES 100
#define MAX_PUMPKINS 10
#define MAX_BULLETS 50
#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080

typedef struct {
    float x;
    float y;
} Vector2;

typedef struct {
    Vector2 position;
    int health;
    int speed;
    bool active;
    uint32_t id;
} Enemy;

typedef struct {
    Vector2 position;
    int health;
    bool active;
    int playerID;
} Tower;

typedef struct {
    Vector2 position;
    bool active;
    int playerID;
} Pumpkin;

typedef struct {
    Vector2 position;
    Vector2 direction;
    float speed;
    bool active;
    int playerID;
} Bullet;

typedef struct {
    int playerID;
    bool connected;
    int points;
    Tower tower;
} Player;

typedef struct {
    Enemy enemies[MAX_ENEMIES];
    Pumpkin pumpkins[MAX_PUMPKINS];
    Tower towers[MAX_PLAYERS];
    Bullet bullets[MAX_BULLETS];
    Player players[MAX_PLAYERS];
    bool isDaytime;
    float gameTimer;
    int enemiesKilled;
    bool gameOver;
    bool gameWon;
    float spawnTimer;
    int daytimeCycleCount;
} GameState;

// Game logic functions
void initializeGameState(GameState* state);
void updateGameState(GameState* state, float deltaTime);
int addPlayer(GameState* state);
void removePlayer(GameState* state, int playerID);
void handlePlayerAction(GameState* state, int playerID, const char* action, Vector2 position, Vector2 direction);
char* serializeGameState(const GameState* state);
void deserializeGameState(GameState* state, const char* json);

#endif // GAMELOGIC_H
