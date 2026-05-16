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
#include "../../core/ui.h"

#define SPRITE_SIZE 16
#define GRAVITY 9.6f * 20
#define FORCE 2.5f
#define GROUND_HEIGHT 70
#define BG_MOVEMENT_SPEED 15
#define MAX_PIPES 3

#define SPACING_BETWEEN_PIPES 100 
#define MIN_GAP_HEIGHT 100
#define MAX_GAP_HEIGHT 400

#define PIPE_SCALING 2.0f
#define PIPE_SPRITE_WIDTH 32
#define PIPE_WIDTH (PIPE_SPRITE_WIDTH * PIPE_SCALING)


// INFO: Here the top is the name for the not inverted pipe at the lower part and bottom is the upper part. Why? Reasons

// based on the spritesheet measurement from src/resources/flappy-bird/Tiles/Style\ 1/SimpleStyle1.png
#define PIPE_SPRITE_TOP_RECT RECT(0, 0, PIPE_SPRITE_WIDTH, 20)
#define PIPE_SPRITE_BOTTOM_RECT RECT(0, 60, PIPE_SPRITE_WIDTH, 20)
#define PIPE_SPRITE_MIDDLE_RECT RECT(0, 20, PIPE_SPRITE_WIDTH, 40)

#define GROUND_SPRITE_TILESETS RECT(0, 80, 64, 32) 
#define MOVEMENT_SPEED 200

#define SCORE_BOX_WIDTH 80
#define SCORE_BOX_HEIGHT 20

typedef enum {
  SCENE_START_MENU,
  SCENE_RUNNING_GAME,
  SCENE_GAME_OVER,
} Scene;

static Camera2D camera = {0};
static i32 score = 0;
static i32 highScore = 0;
static Scene scene = SCENE_START_MENU;
static bool birdWasColliding[MAX_PIPES] = {0};

// Background related data
Texture2D background = {0};
Texture2D tiles = {0};

// Bird related data
Texture2D birdSpriteSheet = {0};
u8 currentBirdAnimationIndex = 0;
u8 currentBirdSelectionIndex = 0;
Rectangle birdRect = {0};
Rectangle birdHitBox = {0};
Vector2 birdPoint = {0};
f32 birdRotation = 0.0f;

typedef struct 
{
  Vector2 pos;
  i32 height;
} Gap;

Gap genRandomGap(i32 i);

Gap gaps[MAX_PIPES] = {0};

typedef struct 
{
  Rectangle top;
  Rectangle bottom;
} PipeInfo;

typedef struct 
{
  Vector2 start;
  Vector2 end;
} Line;

PipeInfo pipes[MAX_PIPES] = {0};
Line passLines[MAX_PIPES] = {0};

PipeInfo genPipesFromGap(Gap gap);
Gap genInitPipes(i32 i);

void DrawSceneRunningGame(void);
void UpdateSceneRunning(f32 delta);
void DrawSceneStartMenu(void);
void UpdateSceneStartMenu(f32 delta);
void DrawSceneGameOver(void);
void UpdateSceneGameOver(f32 delta);

void DrawPipes(PipeInfo);
void DrawScore(i32 score, Vector2 pos, i32 fontSize, Color bgColor, Color fontColor);

void ClearGameState(void);

static void GameInit(i32 width, i32 height, const char* name)
{
  InitWindow(width, height, GAME_NAME);
  InitAudioDevice();

  Image backgroundImage = LoadImage("resources/flappy-bird/Background/Background1.png"); ;
  Image tilesImage = LoadImage("resources/flappy-bird/Tiles/Style 1/TileSheet.png");
  Image birdSpriteSheetImage = LoadImage("resources/flappy-bird/Player/StyleBird1/AllBird1.png");

  background = LoadTextureFromImage(backgroundImage);
  tiles = LoadTextureFromImage(tilesImage);
  birdSpriteSheet = LoadTextureFromImage(birdSpriteSheetImage);
  
  ClearGameState();

  // clear images from RAM
  UnloadImage(backgroundImage);
  UnloadImage(tilesImage);
  UnloadImage(birdSpriteSheetImage);
}

void ClearGameState(void)
{
  score = 0;
  // setup camera
  birdRect.x = WINDOW_WIDTH / 10.0;
  birdRect.y = WINDOW_HEIGHT / 2.5;
  birdRect.width = 32;
  birdRect.height = 32;
  birdHitBox = birdRect;
  birdHitBox.width = 24;
  birdHitBox.height = 24;

  camera.target = VEC2_ZERO;
  camera.offset = VEC2_ZERO;
  camera.rotation = 0.0f;
  camera.zoom = 1.0f;

  for (i32 i = 0; i < MAX_PIPES; i++) 
  {
    gaps[i] = genInitPipes(i);
    passLines[i].start = VEC2(gaps[i].pos.x + PIPE_WIDTH, gaps[i].pos.y);
    passLines[i].end = VEC2(gaps[i].pos.x + PIPE_WIDTH, gaps[i].pos.y + gaps[i].height);
    pipes[i] = genPipesFromGap(gaps[i]);
  }
}

static void GameControls(void)
{
}
static void GameUpdate(f32 delta)
{
  switch(scene)
  {
    case SCENE_START_MENU:   UpdateSceneStartMenu(delta);  break;
    case SCENE_RUNNING_GAME: UpdateSceneRunning(delta);    break;
    case SCENE_GAME_OVER:    UpdateSceneGameOver(delta);   break;
  }
}

static void GameDraw(void)
{
  BeginDrawing();
  {
    ClearBackground(RAYWHITE);
    BeginMode2D(camera);
    switch(scene)
    {
      case SCENE_START_MENU:   DrawSceneStartMenu();   break;
      case SCENE_RUNNING_GAME: DrawSceneRunningGame(); break;
      case SCENE_GAME_OVER:    DrawSceneGameOver();    break;
    }
    EndMode2D();
    DrawFPS(1, 1);
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
f32 tilesDeltaTime = 0.0f;
static f32 bgMovementXPosition = 0.0f;
void UpdateSceneRunning(f32 delta)
{
  if(score > highScore) highScore = score;
  bgMovementXPosition = GetTime() * BG_MOVEMENT_SPEED;
  birdRect.y += delta * GRAVITY;
  if(birdRotation <= 30.0f) birdRotation += (delta * 100);

  if(IsKeyDown(KEY_SPACE))
  {
    currentBirdAnimationIndex = ((i32)(GetTime() * 10)) % 4;
    birdRect.y -= delta * (FORCE * GRAVITY);
    if(birdRotation >= -10.0f) birdRotation -= (delta * 300);
  }

  birdPoint = VEC2(birdRect.x, birdRect.y);

  birdHitBox = birdRect;
  birdHitBox.width = 24;
  birdHitBox.height = 24;


  for (i32 i = 0; i < MAX_PIPES; i++) 
  {
    bool birdIsColliding = CheckCollisionPointLine(birdPoint, passLines[i].start, passLines[i].end, 1);

    if(!birdIsColliding && birdWasColliding[i])
      score++;

    birdWasColliding[i] = birdIsColliding;
  }

  for (i32 i = 0; i < MAX_PIPES; i++) 
  {
    pipes[i].top.x -= delta * MOVEMENT_SPEED;
    pipes[i].bottom.x -= delta * MOVEMENT_SPEED;
    gaps[i].pos.x -= delta * MOVEMENT_SPEED;
    passLines[i].start = VEC2(gaps[i].pos.x + PIPE_WIDTH, gaps[i].pos.y);
    passLines[i].end = VEC2(gaps[i].pos.x + PIPE_WIDTH, gaps[i].pos.y + gaps[i].height);
    if(gaps[i].pos.x <= -PIPE_WIDTH) 
    {
      gaps[i] = genRandomGap(i);
      pipes[i] = genPipesFromGap(gaps[i]);
    }
  }

  for (i32 i = 0; i < MAX_PIPES; i++) {
    if(CheckCollisionRecs(pipes[i].top, birdHitBox) || CheckCollisionRecs(pipes[i].bottom, birdHitBox)){
      scene = SCENE_GAME_OVER;
    }
  }

}

void DrawSceneRunningGame(void)
{
    // render background with it's movement
    DrawTexturePro(background,
                   RECT(bgMovementXPosition, 0, background.width / 2.0, background.height),
                   RECT(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT - GROUND_HEIGHT),
                   VEC2_ZERO,
                   0.0f,
                   WHITE);
    // render bird animation
    DrawTexturePro(birdSpriteSheet,
                   RECT(currentBirdAnimationIndex * SPRITE_SIZE, 0, SPRITE_SIZE, SPRITE_SIZE),
                   birdRect,
                   VEC2_ZERO,
                   birdRotation,
                   WHITE);

    DrawTexturePro(tiles,
                   GROUND_SPRITE_TILESETS,
                   RECT(0, GetScreenHeight() - GROUND_HEIGHT, GetScreenWidth(), GROUND_HEIGHT),
                   VEC2_ZERO,
                   0, WHITE);

    for (i32 i = 0; i < MAX_PIPES; i++) DrawPipes(pipes[i]);

    // DrawScore

    DrawTextBox(TextFormat("Score: %d", score), VEC2(GetScreenWidth() - 10, 10), 15, WHITE, BLACK);
    DrawTextBox(TextFormat("High Score: %d", highScore), VEC2(GetScreenWidth() - 10, 27), 15, WHITE, BLACK);
}

static bool textBlink = false;
static float passedTime =  0.0f;
void UpdateSceneStartMenu(f32 delta)
{
  currentBirdAnimationIndex = ((i32)(GetTime() * 10)) % 4;
  passedTime += delta;
  if(passedTime >= 0.5)
  {
    textBlink = !textBlink;
    passedTime = 0.0f;
  }
  if(IsKeyPressed(KEY_SPACE)) scene = SCENE_RUNNING_GAME;
}

void DrawSceneStartMenu(void)
{
    DrawTexturePro(background,
                   RECT(0, 0, background.width / 2.0, background.height),
                   RECT(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT - GROUND_HEIGHT),
                   VEC2_ZERO,
                   0.0f,
                   WHITE);

    i32 titleTextSize = MeasureText("Flappy Bird", 40);
    Rectangle birdSrcRect = RECT(currentBirdAnimationIndex * SPRITE_SIZE, 0, SPRITE_SIZE, SPRITE_SIZE);
    DrawTexturePro(birdSpriteSheet,
                   birdSrcRect, 
                   birdRect,
                   VEC2_ZERO,
                   birdRotation,
                   WHITE);

    DrawTexturePro(tiles,
                   GROUND_SPRITE_TILESETS,
                   RECT(0, GetScreenHeight() - GROUND_HEIGHT, GetScreenWidth(), GROUND_HEIGHT),
                   VEC2_ZERO,
                   0, WHITE);
    DrawText("Flappy Bird", (GetScreenWidth() - titleTextSize)/2, 180, 40, BLACK);
    
    if(textBlink)
    {
      i32 startGameTextWidth = MeasureText("Press SPACE to start the game", 20);
      DrawText("Press SPACE to start the game", (GetScreenWidth() - startGameTextWidth)/2, GetScreenHeight() * .75, 20, BLACK);
    }
}

static bool gameOverTextBlinking = false;
static i32 gameOverSceneTimePassed = 0.0f;
void UpdateSceneGameOver(f32 delta)
{
  gameOverSceneTimePassed += delta;
  if(gameOverSceneTimePassed >= 0.5)
  {
    gameOverTextBlinking = !gameOverTextBlinking;
    gameOverSceneTimePassed = 0.0f;
  }
  if(IsKeyPressed(KEY_SPACE)){
    if(score > highScore) highScore = score;
    ClearGameState();
    scene = SCENE_RUNNING_GAME;
  }
}

void DrawSceneGameOver(void)
{
    DrawTexturePro(background,
                   RECT(bgMovementXPosition, 0, background.width / 2.0, background.height),
                   RECT(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT - GROUND_HEIGHT),
                   VEC2_ZERO,
                   0.0f,
                   WHITE);

    i32 gameOverTextSize = MeasureText("Game Over", 40);

    DrawTexturePro(birdSpriteSheet,
                   RECT(currentBirdAnimationIndex * SPRITE_SIZE, 0, SPRITE_SIZE, SPRITE_SIZE),
                   birdRect,
                   VEC2_ZERO,
                   birdRotation,
                   WHITE);

    DrawTexturePro(tiles,
                   GROUND_SPRITE_TILESETS,
                   RECT(0, GetScreenHeight() - GROUND_HEIGHT, GetScreenWidth(), GROUND_HEIGHT),
                   VEC2_ZERO,
                   0, WHITE);
    
    for (i32 i = 0; i < MAX_PIPES; i++) DrawPipes(pipes[i]);

    DrawText("Game Over", (GetScreenWidth() - gameOverTextSize) / 2, 180, 40, BLACK);
    const char* restartText = "Press SPACE to restart the game";
    i32 restartGameTextWidth = MeasureText(restartText, 20);
    DrawText(restartText, (GetScreenWidth() - restartGameTextWidth)/2, GetScreenHeight() * .75, 20, BLACK);

}

void DrawPipes(PipeInfo pipeInfo)
{
  // drawing the lower un-inverted pipe
  DrawTexturePro(tiles,
                 PIPE_SPRITE_TOP_RECT,
                 RECT(pipeInfo.top.x , pipeInfo.top.y, PIPE_WIDTH, PIPE_SPRITE_TOP_RECT.height * PIPE_SCALING),
                 VEC2_ZERO,
                 0,
                 WHITE);
  DrawTexturePro(tiles,
                 PIPE_SPRITE_MIDDLE_RECT,
                 RECT(pipeInfo.top.x, pipeInfo.top.y + (PIPE_SPRITE_TOP_RECT.height * PIPE_SCALING), PIPE_WIDTH, (pipeInfo.top.height) - (PIPE_SPRITE_TOP_RECT.height * PIPE_SCALING)),
                 VEC2_ZERO,
                 0,
                 WHITE);

  // Draw the upper inverted pipe
  Rectangle upperDestRect = RECT(pipeInfo.bottom.x,
                                 pipeInfo.bottom.height - PIPE_SPRITE_BOTTOM_RECT.height - (PIPE_SPRITE_BOTTOM_RECT.height * PIPE_SCALING/2.0),
                                 PIPE_WIDTH,
                                 PIPE_SPRITE_BOTTOM_RECT.height * PIPE_SCALING);
  DrawTexturePro(tiles,
                 PIPE_SPRITE_BOTTOM_RECT,
                 upperDestRect,
                 VEC2_ZERO,
                 0,
                 WHITE);

  DrawTexturePro(tiles,
                 PIPE_SPRITE_MIDDLE_RECT,
                 RECT(pipeInfo.bottom.x, pipeInfo.bottom.y, PIPE_WIDTH, upperDestRect.y),
                 VEC2_ZERO,
                 0,
                 WHITE);
}
PipeInfo genPipesFromGap(Gap gap)
{
  i32 topPipeY = gap.pos.y + gap.height;
  
  return (PipeInfo)
    {
      .top = RECT(gap.pos.x, topPipeY, PIPE_WIDTH, GetScreenHeight() - topPipeY - GROUND_HEIGHT),
      .bottom = RECT(gap.pos.x, 0, PIPE_WIDTH, gap.pos.y),
    };
}

Gap genInitPipes(i32 i)
{
  if(i == 0)
  {
    Vector2 pos = VEC2(GetScreenWidth() - 50, GetRandomValue(100, GetScreenHeight() / 2));
    i32 maxHeight = GetScreenHeight() - pos.y - GROUND_HEIGHT - (PIPE_SPRITE_TOP_RECT.height * 2);
    return (Gap)
      {
        .pos = pos,
        .height = GetRandomValue(SPRITE_SIZE * 4, maxHeight / 2)
      };
  }

  i32 prev = i - 1;
  PipeInfo prevPipeInfo = pipes[prev];
  Vector2 pos = VEC2(prevPipeInfo.top.x + prevPipeInfo.top.width + SPACING_BETWEEN_PIPES, GetRandomValue(100, GetScreenHeight() / 2));
  i32 maxHeight = GetScreenHeight() - pos.y - GROUND_HEIGHT - (PIPE_SPRITE_TOP_RECT.height * 2);

  return (Gap)
    {
      .pos = pos,
      .height = GetRandomValue(SPRITE_SIZE * 4, maxHeight / 2)
    };
}

Gap genRandomGap(i32 i)
{
  i32 prev = i == 0 ? MAX_PIPES - 1 : i - 1;
  PipeInfo prevPipeInfo = pipes[prev];
  Vector2 pos = VEC2(prevPipeInfo.top.x + prevPipeInfo.top.width + SPACING_BETWEEN_PIPES, GetRandomValue(100, GetScreenHeight() / 2));
  i32 maxHeight = GetScreenHeight() - pos.y - GROUND_HEIGHT - (PIPE_SPRITE_TOP_RECT.height * 2);

  return (Gap)
    {
      .pos = pos,
      .height = GetRandomValue(SPRITE_SIZE * 4, maxHeight / 2)
    };
}


void DrawScore(i32 score, Vector2 pos, i32 fontSize, Color bgColor, Color fontColor)
{
  const char* scoreText = TextFormat("Score: %d", score);
  i32 textLength = MeasureText(scoreText, fontSize);
  i32 rectPosX = pos.x - textLength;
  i32 rectWidth = textLength + 5;
  i32 rectHeight = fontSize + 5;
  i32 textPosX = rectPosX + ((rectWidth - textLength) / 2);
  i32 textPosY = pos.y + ((rectHeight - fontSize) / 2);
  DrawRectangle(rectPosX, pos.y, rectWidth, rectHeight, bgColor);
  DrawText(scoreText, textPosX, textPosY, fontSize, fontColor);
}
