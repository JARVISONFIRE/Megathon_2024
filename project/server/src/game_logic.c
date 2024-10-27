

// gamelogic.c
#include "gamelogic.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <json-c/json.h>

#define SPAWN_FREQUENCY 2.0f
#define GAME_DURATION 540.0f
#define DAY_DURATION 60.0f
#define NIGHT_DURATION 120.0f

static float getDistance(Vector2 a, Vector2 b) {
    return sqrtf((b.x - a.x) * (b.x - a.x) + (b.y - a.y) * (b.y - a.y));
}

void initializeGameState(GameState* state) {
    memset(state, 0, sizeof(GameState));
    state->isDaytime = true;
    state->gameTimer = 0;
    state->spawnTimer = 0;
    state->daytimeCycleCount = 0;
    
    // Initialize arrays
    for (int i = 0; i < MAX_ENEMIES; i++) state->enemies[i].active = false;
    for (int i = 0; i < MAX_PUMPKINS; i++) state->pumpkins[i].active = false;
    for (int i = 0; i < MAX_BULLETS; i++) state->bullets[i].active = false;
    for (int i = 0; i < MAX_PLAYERS; i++) {
        state->players[i].connected = false;
        state->players[i].points = 500;
        state->players[i].tower.active = false;
    }
}

static void spawnEnemy(GameState* state) {
    if (state->isDaytime) return;
    
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!state->enemies[i].active) {
            int spawnSide = rand() % 4;
            switch (spawnSide) {
                case 0: // Top
                    state->enemies[i].position.x = rand() % SCREEN_WIDTH;
                    state->enemies[i].position.y = 0;
                    break;
                case 1: // Right
                    state->enemies[i].position.x = SCREEN_WIDTH;
                    state->enemies[i].position.y = rand() % SCREEN_HEIGHT;
                    break;
                case 2: // Bottom
                    state->enemies[i].position.x = rand() % SCREEN_WIDTH;
                    state->enemies[i].position.y = SCREEN_HEIGHT;
                    break;
                case 3: // Left
                    state->enemies[i].position.x = 0;
                    state->enemies[i].position.y = rand() % SCREEN_HEIGHT;
                    break;
            }
            state->enemies[i].health = 5;
            state->enemies[i].speed = 1;
            state->enemies[i].active = true;
            state->enemies[i].id = rand(); // Generate unique ID
            break;
        }
    }
}

void updateGameState(GameState* state, float deltaTime) {
    if (state->gameOver) return;

    // Update game timer and day/night cycle
    state->gameTimer += deltaTime;
    if (state->gameTimer >= GAME_DURATION) {
        state->gameOver = true;
        state->gameWon = true;
        return;
    }

    // Update day/night cycle
    float cycleTime = state->isDaytime ? DAY_DURATION : NIGHT_DURATION;
    if (fmodf(state->gameTimer, cycleTime) < 0.016f) { // Check for cycle change (assuming 60 FPS)
        state->isDaytime = !state->isDaytime;
        if (state->isDaytime) state->daytimeCycleCount++;
    }

    // Spawn enemies during night
    state->spawnTimer += deltaTime;
    if (state->spawnTimer >= SPAWN_FREQUENCY) {
        state->spawnTimer = 0;
        spawnEnemy(state);
    }

    // Update enemies
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!state->enemies[i].active) continue;

        // Find closest target (tower or pumpkin)
        float closestDist = INFINITY;
        Vector2 targetPos = {0, 0};
        bool foundTarget = false;

        // Check towers
        for (int j = 0; j < MAX_PLAYERS; j++) {
            if (state->players[j].tower.active) {
                float dist = getDistance(state->enemies[i].position, state->players[j].tower.position);
                if (dist < closestDist) {
                    closestDist = dist;
                    targetPos = state->players[j].tower.position;
                    foundTarget = true;
                }
            }
        }

        // Check pumpkins
        for (int j = 0; j < MAX_PUMPKINS; j++) {
            if (state->pumpkins[j].active) {
                float dist = getDistance(state->enemies[i].position, state->pumpkins[j].position);
                if (dist < closestDist) {
                    closestDist = dist;
                    targetPos = state->pumpkins[j].position;
                    foundTarget = true;
                }
            }
        }

        // Move enemy towards target
        if (foundTarget) {
            Vector2 dir = {
                targetPos.x - state->enemies[i].position.x,
                targetPos.y - state->enemies[i].position.y
            };
            float dist = sqrtf(dir.x * dir.x + dir.y * dir.y);
            if (dist > 0) {
                state->enemies[i].position.x += (dir.x / dist) * state->enemies[i].speed;
                state->enemies[i].position.y += (dir.y / dist) * state->enemies[i].speed;
            }
        }
    }

    // Update bullets
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (!state->bullets[i].active) continue;

        // Move bullet
        state->bullets[i].position.x += state->bullets[i].direction.x * state->bullets[i].speed;
        state->bullets[i].position.y += state->bullets[i].direction.y * state->bullets[i].speed;

        // Check for bullet collision with enemies
        for (int j = 0; j < MAX_ENEMIES; j++) {
            if (!state->enemies[j].active) continue;
            
            if (getDistance(state->bullets[i].position, state->enemies[j].position) < 15) {
                state->enemies[j].health -= 2;
                state->bullets[i].active = false;
                
                if (state->enemies[j].health <= 0) {
                    state->enemies[j].active = false;
                    state->enemiesKilled++;
                    state->players[state->bullets[i].playerID].points += 10;
                }
                break;
            }
        }

        // Deactivate bullets that are out of bounds
        if (state->bullets[i].position.x < 0 || state->bullets[i].position.x > SCREEN_WIDTH ||
            state->bullets[i].position.y < 0 || state->bullets[i].position.y > SCREEN_HEIGHT) {
            state->bullets[i].active = false;
        }
    }

    // Check for game over conditions
    bool anyTowerActive = false;
    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (state->players[i].tower.active) {
            anyTowerActive = true;
            break;
        }
    }
    if (!anyTowerActive) {
        state->gameOver = true;
        state->gameWon = false;
    }
}
