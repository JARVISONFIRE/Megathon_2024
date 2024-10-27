// shared/game_constants.h
#ifndef GAME_CONSTANTS_H
#define GAME_CONSTANTS_H

#include "raylib.h"
#include <stdbool.h>

// Screen and gameplay constants
#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080
#define MAX_ENEMIES 100
#define MAX_PUMPKINS 10
#define MAX_TOWERS 1
#define MAX_BULLETS 50
#define GAME_DURATION 540

// Play area dimensions
#define PLAY_AREA_X (SCREEN_WIDTH / 2 - 960)
#define PLAY_AREA_Y (SCREEN_HEIGHT / 2 - 540)
#define PLAY_AREA_WIDTH 1920
#define PLAY_AREA_HEIGHT 1080

// Network-specific constants
#define MAX_CLIENTS 4
#define UPDATE_INTERVAL (1.0f / 60.0f)
#define BUFFER_SIZE 1024
#define MAX_CONNECTIONS 10

// Game entity structures
typedef struct Enemy {
    Vector2 position;
    int health;
    int speed;
    bool active;
} Enemy;

typedef struct Tower {
    Vector2 position;
    int health;
    bool active;
} Tower;

typedef struct Pumpkin {
    Vector2 position;
    bool active;
} Pumpkin;

typedef struct Bullet {
    Vector2 position;
    Vector2 direction;
    float speed;
    bool active;
} Bullet;

// Game state structure
typedef struct GameState {
    Enemy enemies[MAX_ENEMIES];
    Pumpkin pumpkins[MAX_PUMPKINS];
    Tower towers[MAX_TOWERS];
    Bullet bullets[MAX_BULLETS];
    int points;
    float spawnTimer;
    float spawnFrequency;
    int enemiesKilled;
    float gameTimer;
    bool gameOver;
    bool gameWon;
    bool isDaytime;
    bool towerPlaced;
    int daytimeCycleCount;
    bool firstDay;
    int towerHealth;
} GameState;

// Network message types
typedef enum MessageType {
    MSG_JOIN,
    MSG_LEAVE,
    MSG_UPDATE,
    MSG_PLACE_PUMPKIN,
    MSG_SHOOT,
    MSG_SYNC,
    MSG_GAME_START,
    MSG_GAME_OVER
} MessageType;

// Network message structure
typedef struct NetworkMessage {
    MessageType type;
    int clientId;
    union {
        struct {
            Vector2 position;
            Vector2 direction;
        } action;
        GameState gameState;
    } data;
    float timestamp;
} NetworkMessage;

// Player structure
typedef struct Player {
    int id;
    bool active;
    Vector2 cursorPosition;
    bool isReady;
} Player;

// Game session structure
typedef struct GameSession {
    GameState state;
    Player players[MAX_CLIENTS];
    int numPlayers;
    bool isActive;
    float sessionTime;
} GameSession;

// Asset paths
#define ASSET_PATH_BACKGROUND_DAY "assets/images/background_day.png"
#define ASSET_PATH_BACKGROUND_NIGHT "assets/images/background_night.png"
#define ASSET_PATH_CASTLE "assets/images/castle.png"
#define ASSET_PATH_PUMPKIN "assets/images/pumpkin.png"
#define ASSET_PATH_ZOMBIE "assets/images/zombie.png"
#define ASSET_PATH_BULLET "assets/images/bullet.png"
#define ASSET_PATH_SOUND_HIT "assets/sounds/hit.wav"
#define ASSET_PATH_SOUND_PUMPKIN "assets/sounds/pumpkin_place.wav"
#define ASSET_PATH_SOUND_SHOOT "assets/sounds/shoot.wav"
#define ASSET_PATH_SOUND_TOWER "assets/sounds/tower_attack.wav"
#define ASSET_PATH_SOUND_BACKGROUND "assets/sounds/background.mp3"

// Game configuration constants
#define INITIAL_TOWER_HEALTH 900
#define BULLET_SPEED 10.0f
#define ENEMY_BASE_HEALTH 5
#define ENEMY_BASE_SPEED 1
#define BULLET_DAMAGE 2
#define DAY_DURATION 60
#define NIGHT_DURATION 120
#define INITIAL_POINTS 500

// Collision detection constants
#define COLLISION_THRESHOLD 15.0f
#define TOWER_COLLISION_RADIUS 16.0f
#define PUMPKIN_COLLISION_RADIUS 16.0f
#define BULLET_COLLISION_RADIUS 4.0f

// Game state flags
typedef struct GameFlags {
    bool isPaused;
    bool isMultiplayer;
    bool isHost;
    bool debugMode;
} GameFlags;

// Function declarations for common utility functions
float GetDistance(Vector2 a, Vector2 b);
bool CheckCollision(Vector2 pos1, Vector2 pos2, float radius);
Vector2 NormalizeVector(Vector2 vector);

#endif // GAME_CONSTANTS_H
