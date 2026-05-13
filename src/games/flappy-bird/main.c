#include "raylib.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define WINDOW_WIDTH 400
#define WINDOW_HEIGHT 800
#define GAME_NAME "Flappy Bird"
#define CORE_MAIN
#include "../../core/core.h"
#include "../../core/utils.h"

#define SPRITE_SIZE 16
#define GRAVITY 9.6f * 20
#define FORCE 2.5f
#define GROUND_HEIGHT 150
#define BG_MOVEMENT_SPEED 15
#define MAX_PIPES 4


typedef struct {
  Vector2 pos;
  f32 height;
  bool isFlipped;
  bool isInsideScreen;
} PipeInfo;


static Camera2D camera = {0};

// Background related data
Texture2D background = {0};
Texture2D tiles = {0};

// Bird related data
Texture2D birdSpriteSheet = {0};
u8 currentBirdAnimationIndex = 0;
u8 currentBirdSelectionIndex = 0;
Rectangle birdRect = {0};
f32 birdRotation = 0.0f;

// Pipe related data
PipeInfo pipes[MAX_PIPES] = {0};

void DrawPipe(PipeInfo pipes);

static void GameInit(i32 width, i32 height, const char* name)
{
  InitWindow(width, height, GAME_NAME);
  InitAudioDevice();

  Image backgroundImage = LoadImage("resources/flappy-bird/Background/Background1.png"); ;
  Image tilesImage = LoadImage("resources/flappy-bird/Tiles/Style 1/SimpleStyle1.png");
  Image birdSpriteSheetImage = LoadImage("resources/flappy-bird/Player/StyleBird1/AllBird1.png");

  background = LoadTextureFromImage(backgroundImage);
  tiles = LoadTextureFromImage(tilesImage);
  birdSpriteSheet = LoadTextureFromImage(birdSpriteSheetImage);
  
  // setup camera
  birdRect.x = WINDOW_WIDTH / 10.0;
  birdRect.y = WINDOW_HEIGHT / 2.5;
  birdRect.width = 32;
  birdRect.height = 32;

  camera.target = VEC2_ZERO;
  camera.offset = VEC2_ZERO;
  camera.rotation = 0.0f;
  camera.zoom = 1.0f;

  pipes[0] = (PipeInfo){0};
  pipes[1] = (PipeInfo){0};
  pipes[2] = (PipeInfo){0};
  pipes[3] = (PipeInfo){0};

  // clear images from RAM
  UnloadImage(backgroundImage);
  UnloadImage(tilesImage);
  UnloadImage(birdSpriteSheetImage);
}

static void GameControls(void)
{
}
static void GameUpdate(f32 delta)
{
  birdRect.y += delta * GRAVITY;
  if(birdRotation <= 30.0f) birdRotation += (delta * 100);

  if(IsKeyDown(KEY_SPACE))
  {
    currentBirdAnimationIndex = ((i32)(GetTime() * 10)) % 4;
    birdRect.y -= delta * (FORCE * GRAVITY);
    if(birdRotation >= -10.0f) birdRotation -= (delta * 300);
  }
}
static void GameDraw(void)
{
  BeginDrawing();
  {
    BeginMode2D(camera);
    {
      // render background with it's movement
      DrawTexturePro(background,
                     RECT(GetTime() * BG_MOVEMENT_SPEED, 0, background.width / 2.0, background.height),
                     RECT(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT - GROUND_HEIGHT),
                     VEC2_ZERO,
                     0.0f,
                     WHITE);
      // render bird animation
      DrawTexturePro(birdSpriteSheet,
                     RECT(currentBirdAnimationIndex * SPRITE_SIZE, 0, SPRITE_SIZE, SPRITE_SIZE),
                     birdRect,
                     VEC2(SPRITE_SIZE / 2.0, SPRITE_SIZE / 2.0),
                     birdRotation,
                     WHITE);

      for (i32 i = 0; i < MAX_PIPES; i++) DrawPipe(pipes[i]);
      ClearBackground(RAYWHITE);
    }
    EndMode2D();
  }
  EndDrawing();
}
static void GameEnd(void)
{

  UnloadTexture(background);
  UnloadTexture(tiles);
  UnloadTexture(birdSpriteSheet);
  CloseAudioDevice();
  CloseWindow();
}

void DrawPipe(PipeInfo pipe)
{
}
