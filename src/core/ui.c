#include "ui.h"

void DrawTextBox(const char* text, Vector2 pos, i32 fontSize, Color bgColor, Color fontColor)
{
  i32 textLength = MeasureText(text, fontSize);
  i32 rectPosX = pos.x - textLength;
  i32 rectWidth = textLength + 5;
  i32 rectHeight = fontSize + 5;
  i32 textPosX = rectPosX + ((rectWidth - textLength) / 2);
  i32 textPosY = pos.y + ((rectHeight - fontSize) / 2);
  DrawRectangle(rectPosX, pos.y, rectWidth, rectHeight, bgColor);
  DrawText(text, textPosX, textPosY, fontSize, fontColor);
}
