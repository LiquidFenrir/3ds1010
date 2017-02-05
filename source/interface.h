#include "common.h"
#include "draw.h"

void setupScreens();
void drawHover(piece currentPiece, u16 xPos, u16 yPos);
void drawInterface(u8 selected_tile, u8 inventory[3], u8 selected_piece, u32 score, u32 highscore);