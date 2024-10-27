



// game_render.c
#include "game_render.h"
#include <raylib.h>

static Texture2D backgroundDay;
static Texture2D backgroundNight;
static Texture2D castleTexture;
static Texture2D pumpkinTexture;
static Texture2D zombieTexture;
static Texture2D bulletTexture;

void InitGameRenderer(void) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Tower Defense Multiplayer");
    SetTargetFPS(60);

    // Load textures
    backgroundDay = LoadTexture("assets/images/background_day.png");
    backgroundNight = LoadTexture("assets/images/background_night.png");
    castleTexture = LoadTexture("assets/images/castle.png");
    pumpkinTexture = LoadTexture("assets/images/pumpkin.png");
    zombieTexture = LoadTexture("assets/images/zombie.png");
    bulletTexture = LoadTexture("assets/images/bullet.png");
}

void RenderGame(GameState* state) {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    // Draw background based on time of day
    DrawTexture(state->isDaytime ? backgroundDay : backgroundNight, 0, 0, WHITE);

    // Draw towers
    for (int i = 0; i < MAX_TOWERS; i++) {
        if (state->towers[i].active) {
            DrawTexture(castleTexture, 
                state->towers[i].position.x - 16, 
                state->towers[i].position.y - 16, 
                WHITE);
        }
    }

    // Draw pumpkins
    for (int i = 0; i < MAX_PUMPKINS; i++) {
        if (state->pumpkins[i].active) {
            DrawTexture(pumpkinTexture, 
                state->pumpkins[i].position.x - 16, 
                state->pumpkins[i].position.y - 16, 
                WHITE);
        }
    }

    // Draw enemies
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (state->enemies[i].active) {
            DrawTexture(zombieTexture, 
                state->enemies[i].position.x - 16, 
                state->enemies[i].position.y - 16, 
                WHITE);
        }
    }

    // Draw bullets
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (state->bullets[i].active) {
            DrawTexture(bulletTexture, 
                state->bullets[i].position.x - 4, 
                state->bullets[i].position.y - 4, 
                WHITE);
        }
    }

    // Draw UI elements
    DrawText(TextFormat("Enemies Killed: %d", state->enemiesKilled), 10, 10, 20, BLACK);
    DrawText(TextFormat("Points: %d", state->points), 10, 40, 20, BLACK);
    DrawText(TextFormat("Game Time: %.1f", state->gameTimer), 10, 70, 20, BLACK);
    DrawText(state->isDaytime ? "Daytime" : "Nighttime", 10, 100, 20, 
        state->isDaytime ? YELLOW : BLUE);

    EndDrawing();
}

void CloseGameRenderer(void) {
    UnloadTexture(backgroundDay);
    UnloadTexture(backgroundNight);
    UnloadTexture(castleTexture);
    UnloadTexture(pumpkinTexture);
    UnloadTexture(zombieTexture);
    UnloadTexture(bulletTexture);
    CloseWindow();
}
