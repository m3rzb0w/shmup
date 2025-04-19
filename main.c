#include "raylib.h"
#include <stdio.h>

// -----------------------------------------------------------------------------
// Constants
// -----------------------------------------------------------------------------
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define SHIP_MAX_BULLETS 50

// -----------------------------------------------------------------------------
// Types
// -----------------------------------------------------------------------------
typedef struct Bullet {
    Vector2 position;
    Vector2 velocity;
    bool active;
} Bullet;

typedef struct Ship {
    Vector2 position;
    Texture2D texture;
    float speed;
    float scale;
    Vector2 velocity;
} Ship;

// -----------------------------------------------------------------------------
// Globals
// -----------------------------------------------------------------------------
Bullet bullets[SHIP_MAX_BULLETS];

float shootCooldown = 0.15f;       // Time between shots
float timeSinceLastShot = 0.0f;

// -----------------------------------------------------------------------------
// Function Declarations
// -----------------------------------------------------------------------------
void InitBullets(void);
void ShootBullet(Vector2 shipPos);
void UpdateBullets(float deltaTime);
void DrawBullets(void);

// -----------------------------------------------------------------------------
// Bullet Functions
// -----------------------------------------------------------------------------
void InitBullets(void) {
    for (int i = 0; i < SHIP_MAX_BULLETS; i++) {
        bullets[i].active = false;
    }
}

void ShootBullet(Vector2 shipPos) {
    for (int i = 0; i < SHIP_MAX_BULLETS; i++) {
        if (!bullets[i].active) {
            bullets[i].position = shipPos;
            bullets[i].velocity = (Vector2){ 0, -500 }; // Shoot upward
            bullets[i].active = true;
            break;
        }
    }
}

void UpdateBullets(float deltaTime) {
    for (int i = 0; i < SHIP_MAX_BULLETS; i++) {
        if (bullets[i].active) {
            bullets[i].position.x += bullets[i].velocity.x * deltaTime;
            bullets[i].position.y += bullets[i].velocity.y * deltaTime;

            if (bullets[i].position.y < 0) {
                bullets[i].active = false;
            }
        }
    }
}

void DrawBullets(void) {
    for (int i = 0; i < SHIP_MAX_BULLETS; i++) {
        if (bullets[i].active) {
            DrawCircleV(bullets[i].position, 5, RED);
        }
    }
}

int CountActiveBullets(void) {
    int count = 0;
    for (int i = 0; i < SHIP_MAX_BULLETS; i++) {
        if (bullets[i].active) {
            count++;
        }
    }
    return count;
}

// -----------------------------------------------------------------------------
// Main
// -----------------------------------------------------------------------------
int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "raylib shmup test");
    SetTargetFPS(60);

    Texture2D ship_sprite = LoadTexture("assets/raw/ship.png");

    Ship player = {
        .position = { SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f },
        .texture = ship_sprite,
        .speed = 300.0f,
        .velocity = { 0 },
        .scale = 3.0f
    };

    InitBullets();

    while (!WindowShouldClose())
    {
        float deltaTime = GetFrameTime();

        // Player Movement
        if (IsKeyDown(KEY_RIGHT)) player.position.x += player.speed * deltaTime;
        if (IsKeyDown(KEY_LEFT))  player.position.x -= player.speed * deltaTime;
        if (IsKeyDown(KEY_DOWN))  player.position.y += player.speed * deltaTime;
        if (IsKeyDown(KEY_UP))    player.position.y -= player.speed * deltaTime;


        timeSinceLastShot += deltaTime;
        // Shooting
        if (IsKeyDown(KEY_SPACE) && timeSinceLastShot >= shootCooldown) {
            // Center horizontally based on ship texture and scale
            Vector2 bulletSpawnPos = {
                player.position.x + (player.texture.width * player.scale / 2.0f),
                player.position.y
            };
            // Move the bullet slightly above the ship
            bulletSpawnPos.y -= 10.0f;

            ShootBullet(bulletSpawnPos); // Fire the bullet
            timeSinceLastShot = 0.0f; // Reset cooldown timer
        }

        // printf("Active Bullets: %d\n", CountActiveBullets());


        // Update
        UpdateBullets(deltaTime);

        // Draw
        BeginDrawing();
        ClearBackground(BLACK);

        DrawTextureEx(player.texture, player.position, 0.0f, player.scale, WHITE);
        DrawBullets();

        EndDrawing();
    }

    UnloadTexture(ship_sprite);
    CloseWindow();

    return 0;
}
