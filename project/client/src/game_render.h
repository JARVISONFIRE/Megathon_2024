// game_render.h
#ifndef GAME_RENDER_H
#define GAME_RENDER_H

#include "raylib.h"
#include <stdbool.h>

// Forward declaration of GameState
typedef struct GameState GameState;

void InitGameRenderer(void);
void RenderGame(GameState* state);
void CloseGameRenderer(void);

#endif // GAME_RENDER_H
