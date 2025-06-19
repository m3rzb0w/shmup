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
    Vector2 velocity; // This seems unused, consider removing if not needed.
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

    Texture2D ship_sprite = LoadTexture("assets/raw/ship_sheet.png");

    Ship player = {
        .position = { SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f },
        .texture = ship_sprite,
        .speed = 300.0f,
        .velocity = { 0 }, // This field seems unused in your current code.
        .scale = 3.0f // <--- INCREASE THIS VALUE TO MAKE THE SHIP BIGGER
    };

    // Define the source rectangles for each frame on the sprite sheet
    // Your sprite sheet is 120x24 px, with 5 frames of 24x24 px
    // Frame 1: x=0, y=0, width=24, height=24
    // Frame 2: x=24, y=0, width=24, height=24
    // Frame 3: x=48, y=0, width=24, height=24
    // Frame 4: x=72, y=0, width=24, height=24
    // Frame 5: x=96, y=0, width=24, height=24

    Rectangle frame1 = { 0.0f, 0.0f, 24.0f, 24.0f };
    Rectangle frame2 = { 24.0f, 0.0f, 24.0f, 24.0f };
    Rectangle frame3 = { 48.0f, 0.0f, 24.0f, 24.0f }; // Default frame
    Rectangle frame4 = { 72.0f, 0.0f, 24.0f, 24.0f };
    Rectangle frame5 = { 96.0f, 0.0f, 24.0f, 24.0f };

    Rectangle currentFrame = frame3; // Start with the third frame

    InitBullets();
    InitStars();

    while (!WindowShouldClose())
    {
        float deltaTime = GetFrameTime();

        // Player Movement and Frame Selection
        bool movingLeft = false;
        bool movingRight = false;

        if (IsKeyDown(KEY_RIGHT)) {
            player.position.x += player.speed * deltaTime;
            movingRight = true;
        }
        if (IsKeyDown(KEY_LEFT)) {
            player.position.x -= player.speed * deltaTime;
            movingLeft = true;
        }
        if (IsKeyDown(KEY_DOWN))  player.position.y += player.speed * deltaTime;
        if (IsKeyDown(KEY_UP))    player.position.y -= player.speed * deltaTime;

        // Determine current animation frame based on maintained key press
        if (movingRight) {
            currentFrame = frame5; // Remain on frame 5 when moving right
        } else if (movingLeft) {
            currentFrame = frame1; // Remain on frame 1 when moving left
        } else {
            currentFrame = frame3; // Default (idle) frame
        }

        timeSinceLastShot += deltaTime;
        // Shooting
        if (IsKeyDown(KEY_SPACE) && timeSinceLastShot >= shootCooldown) {
            // Adjust bullet spawn position based on the scaled ship size
            Vector2 bulletSpawnPos = {
                player.position.x + (currentFrame.width * player.scale / 2.0f), // Center horizontally
                player.position.y                                               // At the ship's Y position
            };
            // Move the bullet slightly above the ship (relative to scaled height)
            bulletSpawnPos.y -= (currentFrame.height * player.scale / 5.0f); // Adjust as needed for bullet to appear at ship's nose

            ShootBullet(bulletSpawnPos); // Fire the bullet
            timeSinceLastShot = 0.0f; // Reset cooldown timer
        }

        // Update
        UpdateBullets(deltaTime);
        UpdateStars(deltaTime); 

        // Draw
        BeginDrawing();
        ClearBackground(BLACK);

        DrawStars();
        
        // Use DrawTexturePro to draw with scaling
        // sourceRect: The part of the texture to draw (your currentFrame)
        // destRect: Where and how big to draw it on the screen
        //            x, y are player.position
        //            width, height are currentFrame's dimensions * player.scale
        // origin: The point in destRect that corresponds to player.position
        //         (0,0) means top-left of the scaled image is at player.position
        // rotation: 0.0f for no rotation
        // tint: WHITE for no tint
        DrawTexturePro(player.texture,
                       currentFrame,
                       (Rectangle){ player.position.x, player.position.y,
                                    currentFrame.width * player.scale, currentFrame.height * player.scale },
                       (Vector2){ 0, 0 }, // Origin for rotation/scaling, set to (0,0) for top-left
                       0.0f,
                       WHITE);
        DrawBullets();

        DrawFPS(10, 10);

        EndDrawing();
    }

    UnloadTexture(ship_sprite);
    CloseWindow();

    return 0;
}