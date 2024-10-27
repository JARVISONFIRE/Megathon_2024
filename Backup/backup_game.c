#include "raylib.h"
#include <stdlib.h>
#include <time.h>
#include <math.h>

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

Enemy enemies[MAX_ENEMIES];
Pumpkin pumpkins[MAX_PUMPKINS];
Tower towers[MAX_TOWERS];
Bullet bullets[MAX_BULLETS];
int points = 500;
float spawnTimer = 0;
float spawnFrequency = 2.0f;
int enemiesKilled = 0;
float gameTimer = 0;
bool gameOver = false;
bool gameWon = false;
bool isDaytime = true;
bool towerPlaced = false;
int daytimeCycleCount = 0;
bool firstDay = true;
int towerHealth = 900;  // Tower health set based on damage per frame

Texture2D backgroundDay;
Texture2D backgroundNight;
Texture2D castleTexture;
Texture2D pumpkinTexture;
Texture2D zombieTexture;
Texture2D bulletTexture;
Sound hitSound;
Sound pumpkinPlaceSound;
Sound shootSound;
Sound towerAttackSound;
Sound backgroundMusic;

void InitGame();
void UpdateGame();
void DrawGame();
void SpawnEnemy();
void UpdateEnemies();
void UpdatePumpkins();
void UpdateBullets();
bool CheckGameOver();
void UpdateDayNightCycle();
float GetDistance(Vector2 a, Vector2 b);
void MoveEnemyTowardsTarget(Enemy *enemy, Vector2 target);
void PlaceTower(Vector2 position);
void PlacePumpkin(Vector2 position);
void ShootBullet(Vector2 position, Vector2 direction);

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Tower Defense Game with Raylib");
    SetTargetFPS(60);

    // Load assets
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

    InitGame();

    // Play background music
    PlaySound(backgroundMusic);

    while (!WindowShouldClose() && !gameOver) {
        UpdateGame();
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawGame();
        EndDrawing();
    }

    // Game Over Screen
    BeginDrawing();
    ClearBackground(RAYWHITE);
    if (gameWon) {
        DrawText("Congratulations! You survived the 9 minutes!", 550, SCREEN_HEIGHT / 2 - 30, 20, GREEN);
    } else {
        DrawText("Game Over! The tower was destroyed!", 700, SCREEN_HEIGHT / 2 - 30, 20, RED);
    }
    DrawText("Press R to restart or ESC to exit.", 680, SCREEN_HEIGHT / 2 + 10, 20, DARKGRAY);
    EndDrawing();

    while (true) { 
        if (IsKeyPressed(KEY_R)) {
            gameOver = false;
            InitGame();
            break;
        }
        if (IsKeyPressed(KEY_ESCAPE)) {
            break;
        }
    }

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

    CloseWindow();
    return 0;
}

void InitGame() {
    for (int i = 0; i < MAX_ENEMIES; i++) enemies[i].active = false;
    for (int i = 0; i < MAX_PUMPKINS; i++) pumpkins[i].active = false;
    for (int i = 0; i < MAX_TOWERS; i++) towers[i].active = false;
    for (int i = 0; i < MAX_BULLETS; i++) bullets[i].active = false;

    gameTimer = 0;
    towerPlaced = false;
    firstDay = true;
    srand(time(NULL));

    if (!towerPlaced) {
        PlaceTower((Vector2){SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2});
        towerPlaced = true;
    }
}

void SpawnEnemy() {
    if (!isDaytime) {
        for (int i = 0; i < MAX_ENEMIES; i++) {
            if (!enemies[i].active) {
                int spawnSide = GetRandomValue(0, 3);
                switch (spawnSide) {
                    case 0:
                        enemies[i].position = (Vector2){ GetRandomValue(PLAY_AREA_X, PLAY_AREA_X + PLAY_AREA_WIDTH), PLAY_AREA_Y };
                        break;
                    case 1:
                        enemies[i].position = (Vector2){ GetRandomValue(PLAY_AREA_X, PLAY_AREA_X + PLAY_AREA_WIDTH), PLAY_AREA_Y + PLAY_AREA_HEIGHT };
                        break;
                    case 2:
                        enemies[i].position = (Vector2){ PLAY_AREA_X, GetRandomValue(PLAY_AREA_Y, PLAY_AREA_Y + PLAY_AREA_HEIGHT) };
                        break;
                    case 3:
                        enemies[i].position = (Vector2){ PLAY_AREA_X + PLAY_AREA_WIDTH, GetRandomValue(PLAY_AREA_Y, PLAY_AREA_Y + PLAY_AREA_HEIGHT) };
                        break;
                }
                enemies[i].health = 5;
                enemies[i].speed = 1;
                enemies[i].active = true;
                break;
            }
        }
    }
}

void UpdateEnemies() {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i].active) {
            Vector2 targetPosition = { 0, 0 };
            float closestDistance = 10000;
            bool foundTarget = false;

            // Find closest active pumpkin or tower as target
            for (int j = 0; j < MAX_PUMPKINS; j++) {
                if (pumpkins[j].active) {
                    float distance = GetDistance(enemies[i].position, pumpkins[j].position);
                    if (distance < closestDistance) {
                        closestDistance = distance;
                        targetPosition = pumpkins[j].position;
                        foundTarget = true;
                    }
                }
            }
            for (int j = 0; j < MAX_TOWERS; j++) {
                if (towers[j].active) {
                    float distance = GetDistance(enemies[i].position, towers[j].position);
                    if (distance < closestDistance) {
                        closestDistance = distance;
                        targetPosition = towers[j].position;
                        foundTarget = true;
                    }
                }
            }

            // Move enemy towards the closest target found
            if (foundTarget) {
                MoveEnemyTowardsTarget(&enemies[i], targetPosition);
            }

            // Check collision with pumpkins and deactivate pumpkin only
            for (int j = 0; j < MAX_PUMPKINS; j++) {
                if (pumpkins[j].active && GetDistance(enemies[i].position, pumpkins[j].position) < 15) {
                    pumpkins[j].active = false;  // Deactivate the pumpkin
                    PlaySound(hitSound);        // Optional: Play a sound for collision
                    break;
                }
            }

            // Check collision with towers and apply damage to towers as needed
            for (int j = 0; j < MAX_TOWERS; j++) {
                if (towers[j].active && GetDistance(enemies[i].position, towers[j].position) < 15) {
                    towers[j].health -= 1;
                    if (towers[j].health <= 0) {
                        towers[j].active = false;
                    }
                    enemies[i].active = false;
                    enemiesKilled++;
                    PlaySound(hitSound);
                    break;
                }
            }
        }
    }
}

void UpdatePumpkins() {
    for (int i = 0; i < MAX_PUMPKINS; i++) {
        if (!pumpkins[i].active && IsMouseButtonPressed(MOUSE_BUTTON_LEFT) ) {
            Vector2 mousePos = GetMousePosition();
            if (mousePos.x >= PLAY_AREA_X && mousePos.x <= PLAY_AREA_X + PLAY_AREA_WIDTH &&
                mousePos.y >= PLAY_AREA_Y && mousePos.y <= PLAY_AREA_Y +                PLAY_AREA_HEIGHT) {
                pumpkins[i].position = mousePos;
                pumpkins[i].active = true;
                PlaySound(pumpkinPlaceSound);
                break;
            }
        }
    }
}

void UpdateBullets() {
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].active) {
            bullets[i].position.x += bullets[i].direction.x * bullets[i].speed;
            bullets[i].position.y += bullets[i].direction.y * bullets[i].speed;

            if (bullets[i].position.x < PLAY_AREA_X || bullets[i].position.x > PLAY_AREA_X + PLAY_AREA_WIDTH ||
                bullets[i].position.y < PLAY_AREA_Y || bullets[i].position.y > PLAY_AREA_Y + PLAY_AREA_HEIGHT) {
                bullets[i].active = false;
            } else {
                for (int j = 0; j < MAX_ENEMIES; j++) {
                    if (enemies[j].active && GetDistance(bullets[i].position, enemies[j].position) < 15) {
                        bullets[i].active = false;
                        enemies[j].health -= 2;  // Adjust bullet damage here
                        if (enemies[j].health <= 0) {
                            enemies[j].active = false;
                            enemiesKilled++;
                        }
                        PlaySound(hitSound);
                        break;
                    }
                }
            }
        }
    }

    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && !isDaytime) {
        Vector2 towerPos = towers[0].position;
        Vector2 mousePos = GetMousePosition();
        Vector2 direction = { mousePos.x - towerPos.x, mousePos.y - towerPos.y };
        float magnitude = sqrtf(direction.x * direction.x + direction.y * direction.y);
        direction.x /= magnitude;
        direction.y /= magnitude;
        ShootBullet(towerPos, direction);
        PlaySound(shootSound);
    }
}

void UpdateDayNightCycle() {
    gameTimer += GetFrameTime();
    int cycleDuration = isDaytime ? 60 : 120;  // 1 minute day, 2 minutes night

    if ((int)gameTimer % cycleDuration == 0) {
        isDaytime = !isDaytime;
        if (!isDaytime && firstDay) firstDay = false;
        if (isDaytime) daytimeCycleCount++;
    }
    if ((int)gameTimer >= GAME_DURATION) {
        gameWon = true;
        gameOver = true;
    }
}

bool CheckGameOver() {
    for (int i = 0; i < MAX_TOWERS; i++) {
        if (!towers[i].active) return true;
    }
    return false;
}

void UpdateGame() {
    if (!gameOver) {
        UpdateDayNightCycle();
        UpdateEnemies();
        UpdatePumpkins();
        UpdateBullets();

        spawnTimer += GetFrameTime();
        if (spawnTimer >= spawnFrequency) {
            spawnTimer = 0;
            SpawnEnemy();
        }

        if (!firstDay) gameOver = CheckGameOver();
    }
}

void DrawGame() {
    if (isDaytime) DrawTexture(backgroundDay, 0, 0, WHITE);
    else DrawTexture(backgroundNight, 0, 0, WHITE);

    for (int i = 0; i < MAX_TOWERS; i++) {
        if (towers[i].active) DrawTexture(castleTexture, towers[i].position.x - 16, towers[i].position.y - 16, WHITE);
    }

    for (int i = 0; i < MAX_PUMPKINS; i++) {
        if (pumpkins[i].active) DrawTexture(pumpkinTexture, pumpkins[i].position.x - 16, pumpkins[i].position.y - 16, WHITE);
    }

    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i].active) DrawTexture(zombieTexture, enemies[i].position.x - 16, enemies[i].position.y - 16, WHITE);
    }

    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].active) DrawTexture(bulletTexture, bullets[i].position.x - 4, bullets[i].position.y - 4, WHITE);
    }

    DrawText(TextFormat("Enemies Killed: %d", enemiesKilled), 10, 10, 20, BLACK);
    DrawText(TextFormat("Points: %d", points), 10, 40, 20, BLACK);
    DrawText(TextFormat("Game Time: %.1f", gameTimer), 10, 70, 20, BLACK);
    DrawText(isDaytime ? "Daytime" : "Nighttime", 10, 100, 20, isDaytime ? YELLOW : BLUE);
}

float GetDistance(Vector2 a, Vector2 b) {
    return sqrtf((b.x - a.x) * (b.x - a.x) + (b.y - a.y) * (b.y - a.y));
}

void MoveEnemyTowardsTarget(Enemy *enemy, Vector2 target) {
    Vector2 direction = { target.x - enemy->position.x, target.y - enemy->position.y };
    float distance = GetDistance(enemy->position, target);

    if (distance > 0) {
        enemy->position.x += direction.x / distance * enemy->speed;
        enemy->position.y += direction.y / distance * enemy->speed;
    }
}

void PlaceTower(Vector2 position) {
    towers[0].position = position;
    towers[0].health = towerHealth;
    towers[0].active = true;
}

void PlacePumpkin(Vector2 position) {
    for (int i = 0; i < MAX_PUMPKINS; i++) {
        if (!pumpkins[i].active) {
            pumpkins[i].position = position;
            pumpkins[i].active = true;
            break;
        }
    }
}

void ShootBullet(Vector2 position, Vector2 direction) {
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (!bullets[i].active) {
            bullets[i].position = position;
            bullets[i].direction = direction;
            bullets[i].speed = 10.0f;
            bullets[i].active = true;
            break;
        }
    }
}
