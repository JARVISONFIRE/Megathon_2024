// game_networking.h
#ifndef GAME_NETWORKING_H
#define GAME_NETWORKING_H

#include "networking.h"
#include "raylib.h"

#define MAX_CLIENTS 4
#define UPDATE_INTERVAL (1.0f / 60.0f)

typedef struct GameState {
    Enemy enemies[MAX_ENEMIES];
    Pumpkin pumpkins[MAX_PUMPKINS];
    Tower towers[MAX_TOWERS];
    Bullet bullets[MAX_BULLETS];
    int points;
    bool isDaytime;
    float gameTimer;
    int enemiesKilled;
} GameState;

typedef struct NetworkMessage {
    enum {
        MSG_JOIN,
        MSG_LEAVE,
        MSG_UPDATE,
        MSG_PLACE_PUMPKIN,
        MSG_SHOOT,
        MSG_SYNC
    } type;
    int clientId;
    Vector2 position;
    Vector2 direction;
    GameState gameState;
} NetworkMessage;

void init_game_networking(void);
void handle_network_messages(void);
void broadcast_game_state(GameState *state);
void send_player_action(int clientId, NetworkMessage *msg);
void receive_game_updates(GameState *state);

#endif
