#include "interface.h"

static PrintConsole scoreWindow;

void setupScreens()
{
	consoleInit(GFX_TOP, &scoreWindow);
	consoleSelect(&scoreWindow);
}

void drawPiece(piece currentPiece, u16 xPos, u16 yPos)
{
	u8 curbit = 0;
	for (int i = 0; i < currentPiece.width; i++) {
		for (int j = 0; j < currentPiece.height; j++) {
			u8 curval = GETBIT(currentPiece.blocks, curbit);
			if (curval == 1) {
				u16 x = xPos + j*10;
				u16 y = yPos + i*10;
				drawSprite( x, y, 10, 10, SPRITE_SMALL);
			}
			curbit++;
		}
	}
}

void drawInventory(u8 inventory[3], u8 selected_piece)
{
	for (int i = 0; i < 3; i++) {
		drawPiece(piecesType[inventory[i]], 240, 20+i*60);
	}
}

void drawGrid(u8 selected_tile)
{
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 10; j++) {
			drawSprite(20 + j*20, 20 + i*20, 20, 20, grid[i][j]);
		}
	}
}

void drawHover(piece currentPiece, u16 xPos, u16 yPos)
{
	u8 curbit = 0;
	for (int i = 0; i < currentPiece.width; i++) {
		for (int j = 0; j < currentPiece.height; j++) {
			u8 curval = GETBIT(currentPiece.blocks, curbit);
			if (curval == 1) {
				u16 x = xPos + j*20;
				u16 y = yPos + i*20;
				drawSpriteWithZ( x, y, 20, 20, SPRITE_FULL, 0.0f);
			}
			curbit++;
		}
	}
}

void drawInterface(u8 selected_tile, u8 inventory[3], u8 selected_piece, u32 score, u32 highscore)
{
	consoleSelect(&scoreWindow);
	consoleClear();
	printf("\x1b[0;0Hhighscore: %lu", highscore);
	printf("\x1b[1;4Hscore: %lu", score);
	
	drawGrid(selected_tile);
	drawInventory(inventory, selected_piece);
}