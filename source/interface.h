#include "common.h"
#include "draw.h"

void drawHover(piece currentPiece, u16 xPos, u16 yPos);
void drawInterface(u8 inventory[3], u8 lost, u32 score, u32 highscore);
void drawThemesMenu(Theme * themes, u8 themesCount, u8 selected_theme);