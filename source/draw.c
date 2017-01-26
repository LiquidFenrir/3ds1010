#include "draw.h"

static PrintConsole debugWindow, gridWindow, inventoryWindow;

void setupScreens()
{
	consoleInit(GFX_BOTTOM, &debugWindow);
	consoleInit(GFX_TOP, &gridWindow);
	consoleInit(GFX_TOP, &inventoryWindow);
	consoleSetWindow(&gridWindow, 10, 8, 15, 15);
	consoleSetWindow(&inventoryWindow, 32, 6, 6, 18);	
}

void drawPiece(piece currentPiece)
{
	u8 curbit = 0;
	for (int i = 0; i < currentPiece.width; i++) {
		for (int j = 0; j < currentPiece.height; j++) {
			u8 curval = GETBIT(currentPiece.blocks, curbit);
			printf("%s", (curval == 0) ? " " : "1");
			curbit++;
		}
		printf("\n");
	}
}

void drawInventory(u8 inventory[3], u8 selected_piece)
{
	for (int i = 0; i < 3; i++) {
		printf("\x1b[%u;0H", i*6);
		
		if (i == selected_piece)
			printf("\x1b[47;30m"); //selected piece has white background
		
		drawPiece(piecesType[inventory[i]]);
		printf("\x1b[0m"); //reset color
	}
}

void drawGrid(u8 selected_tile)
{
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 10; j++) {
			u8 curnum = grid[i][j];
			printf("\x1b[%u;%uH", i+2, j); //set cursor position for each tile
			
			if ((i*10 +j) == selected_tile)
				printf("\x1b[47;30m"); //selected tile has white background
			
			printf("%d", curnum);
			printf("\x1b[0m"); //reset color
		}
	}
}

void drawInterface(u8 selected_tile, u8 inventory[3], u8 selected_piece, u32 score, u8 change)
{
	consoleSelect(&gridWindow);
	if (change != 0)
		consoleClear();
	printf("\x1b[0;0Hscore: %lu", score);
	drawGrid(selected_tile);
	
	consoleSelect(&inventoryWindow);
	if (change != 0)
		consoleClear();
	drawInventory(inventory, selected_piece);
	
	consoleSelect(&debugWindow);
}