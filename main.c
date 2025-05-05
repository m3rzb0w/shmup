#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>

// -----------------------------------------------------------------------------
// Constants
// -----------------------------------------------------------------------------
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define SHIP_MAX_BULLETS 50
#define MAX_STARS 100        // Define the maximum number of stars
#define BASE_STAR_SCROLL_SPEED 530 // Base speed for the stars
#define STAR_SPEED_VARIATION 320 // Range of random speed variation (+/-)
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

typedef struct Star {
    Vector2 position;
    float size;
    Color color;
    float speed;
} Star;
// -----------------------------------------------------------------------------
// Globals
// -----------------------------------------------------------------------------
Bullet bullets[SHIP_MAX_BULLETS];
Star stars[MAX_STARS];

float shootCooldown = 0.15f;       // Time between shots
float timeSinceLastShot = 0.0f;

// -----------------------------------------------------------------------------
// Function Declarations
// -----------------------------------------------------------------------------
void InitBullets(void);
void ShootBullet(Vector2 shipPos);
void UpdateBullets(float deltaTime);
void DrawBullets(void);

void InitStars(void);
void UpdateStars(float deltaTime);
void DrawStars(void);


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
// Star Functions
// -----------------------------------------------------------------------------
void InitStars(void) {
    // Initialize each star with a random position, size, color, and speed
    for (int i = 0; i < MAX_STARS; i++) {
        stars[i].position.x = (float)GetRandomValue(0, SCREEN_WIDTH);
        stars[i].position.y = (float)GetRandomValue(0, SCREEN_HEIGHT);
        stars[i].size = (float)GetRandomValue(1, 3);
        stars[i].color = (Color){130, 130, 130, 255};
        // Assign a random speed to each star
        stars[i].speed = BASE_STAR_SCROLL_SPEED + (float)GetRandomValue(-STAR_SPEED_VARIATION, STAR_SPEED_VARIATION);
        // Ensure the speed is not zero or negative (optional, but might look weird)
        if (stars[i].speed <= 0) {
            stars[i].speed = 1;
        }
    }
}

void UpdateStars(float deltaTime) {
    // Update the vertical position of each star based on its individual speed
    for (int i = 0; i < MAX_STARS; i++) {
        stars[i].position.y += stars[i].speed * deltaTime;

        // If a star goes off the bottom of the screen, reset its position and properties
        if (stars[i].position.y > SCREEN_HEIGHT) {
            stars[i].position.y = (float)GetRandomValue(-5, 0);
            stars[i].position.x = (float)GetRandomValue(0, SCREEN_WIDTH);
            stars[i].size = (float)GetRandomValue(1, 3);
            stars[i].color = (Color){130, 130, 130, 255};
            // Assign a new random speed when the star resets
            stars[i].speed = BASE_STAR_SCROLL_SPEED + (float)GetRandomValue(-STAR_SPEED_VARIATION, STAR_SPEED_VARIATION);
            if (stars[i].speed <= 0) {
                stars[i].speed = 1;
            }
        }
    }
}

void DrawStars(void) {
    // Draw each star as a small circle
    for (int i = 0; i < MAX_STARS; i++) {
        DrawCircleV(stars[i].position, stars[i].size, stars[i].color);
    }
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
    InitStars();

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
        UpdateStars(deltaTime); 

        // Draw
        BeginDrawing();
        ClearBackground(BLACK);

        DrawStars();
        DrawTextureEx(player.texture, player.position, 0.0f, player.scale, WHITE);
        DrawBullets();

        DrawFPS(10, 10);

        EndDrawing();
    }

    UnloadTexture(ship_sprite);
    CloseWindow();

    return 0;
}
