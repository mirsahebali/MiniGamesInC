#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define CORE_MAIN
#define GAME_NAME "Indy 500"
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
