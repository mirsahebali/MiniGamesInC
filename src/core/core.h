#ifndef CORE_H
#define CORE_H

#include "raylib.h"
#include "types.h"

#ifndef GAME_NAME
#define GAME_NAME "No Name Provided"
#endif // !GAME_NAME

#ifndef WINDOW_WIDTH
#define WINDOW_WIDTH 800
#endif

#ifndef WINDOW_HEIGHT
#define WINDOW_HEIGHT 450
#endif

static void GameInit(i32 width, i32 height, const char* name);
static void GameEnd(void);
static void GameUpdate(f32 delta);
static void GameDraw(void);
static void GameControls(void);

#ifdef CORE_MAIN

static void gameLoop(void)
{
  while(!WindowShouldClose())
  {
    GameUpdate(GetFrameTime());
    GameControls();
    GameDraw();
  }
}

int main(int argv, char* argc[])
{
  GameInit(WINDOW_WIDTH, WINDOW_HEIGHT, GAME_NAME);
  gameLoop();
  GameEnd();
  return 0;
}
#endif // CORE_MAIN

#endif // !CORE_H
