#include "draw.h"

void drawInventory(u8 pieces[3], u8 selected_piece)
{
	for (u8 i = 0; i < 3; i++) {
		printf("\x1b[%u;1H", i);
		
		if (i == selected_piece)
			printf("\x1b[47;30m"); //selected piece has white background
		
		printf("piece%u: %u", i, pieces[i]);
		printf("\x1b[0m"); //reset color
	}
}

void drawGrid(u8 grid[][10], u8 selected_tile)
{
	for (u8 i = 0; i < 10; i++) {
		for (u8 j = 0; j < 10; j++) {
			u8 curnum = grid[i][j];
			printf("\x1b[%u;%uH", i, j); //set cursor position for each tile
			
			if ((i*10 +j) == selected_tile)
				printf("\x1b[47;30m"); //selected tile has white background
			
			printf("%d", curnum);
			printf("\x1b[0m"); //reset color
		}
	}
}