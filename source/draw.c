#include "draw.h"

void drawPiece(piece currentPiece)
{
	u8 curbit = 0;
	for (int i = 0; i < currentPiece.height; i++) {
		for (int j = 0; j < currentPiece.width; j++) {
			u8 curval = GETBIT(currentPiece.blocks, curbit);
			printf("%u", curval);
			curbit++;
		}
		printf("\n");
	}
}

void drawInventory(u8 inventory[3], u8 selected_piece)
{
	consoleClear(); //pieces smaller than the ones before would be unreadable
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