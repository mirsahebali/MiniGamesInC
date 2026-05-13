#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define GAME_NAME "Pacman"
#define CORE_MAIN
#include "../../core/core.h"

static void GameInit(int width, int height, const char* name)
{
  InitWindow(width, height, GAME_NAME);
  InitAudioDevice();
}
static void GameEnd(void)
{
  CloseAudioDevice();
  CloseWindow();
}
static void GameUpdate(float delta)
{
}
static void GameDraw(void)
{
  BeginDrawing();
  ClearBackground(RAYWHITE);
  EndDrawing();
}
static void GameControls(void)
{
}
