#include "draw.h"
#include "pieces.h"
#include "grid.h"

piece piecesType[PIECES_AMOUNT] = {0}; //piece 0 is reserved for blank
u8 grid[10][10] = {0};

int main()
{
	gfxInitDefault();
	
	PrintConsole debugWindow, gridWindow, inventoryWindow;
	
	consoleInit(GFX_BOTTOM, &debugWindow);
	consoleInit(GFX_TOP, &gridWindow);
	consoleInit(GFX_TOP, &inventoryWindow);
	
	consoleSetWindow(&gridWindow, 10, 8, 10, 10);
	consoleSetWindow(&inventoryWindow, 32, 6, 6, 18);
	
	consoleSelect(&debugWindow);
	generatePiecesTypes();
	
	u8 inventory[3] = {0};
	u8 selected_tile = 0;
	u8 selected_piece = 0;
	u32 score = 0;
	
	getPieces(inventory);
	
	while (aptMainLoop())
	{
		if (inventory[0] == 0 && inventory[1] == 0 && inventory[2] == 0) //refill inventory when all pieces are placed
			getPieces(inventory);
		
		consoleSelect(&gridWindow);
		consoleClear();
		printf("score: %lu", score);
		drawGrid(selected_tile);
		
		consoleSelect(&inventoryWindow);
		drawInventory(inventory, selected_piece);
		
		consoleSelect(&debugWindow);
		
		hidScanInput();
		
		//quit
		if (hidKeysDown() & KEY_START) {
			break;
		}
		//place the selected piece on the selected tile (starting from top left corner)
		else if (hidKeysDown() & KEY_A) {
			u8 tempscore = placePiece(selected_tile, piecesType[inventory[selected_piece]]);
			if (tempscore != 0) {
				score += tempscore;
				score += checkGrid();
				inventory[selected_piece] = 0;
			}
		}
		//navigate through the grid
		else if (hidKeysDown() & KEY_UP) {
			//dont allow going up if you're already on the top line
			//because the cursor would go out of the grid
			if (selected_tile/10 != 0)
				selected_tile -= 10;
		}
		else if (hidKeysDown() & KEY_DOWN) {
			//dont allow going left if you're already on the bottom line
			//because the cursor would go out of the grid
			if (selected_tile/10 != 9)
				selected_tile += 10;
		}
		else if (hidKeysDown() & KEY_LEFT) {
			//dont allow going left if you're already on the leftmost tile of the line
			//because the cursor would go to the previous line
			if (selected_tile%10 != 0)
				selected_tile -= 1;
		}
		else if (hidKeysDown() & KEY_RIGHT) {
			//dont allow going right if you're already on the rightmost tile of the line
			//because the cursor would go to the next line
			if (selected_tile%10 != 9)
				selected_tile += 1;
		}
		//select the piece to place
		else if (hidKeysDown() & KEY_L) {
			if (selected_piece != 0)
				selected_piece--;
		}
		else if (hidKeysDown() & KEY_R) {
			if (selected_piece != 2)
				selected_piece++;
		}
		//reset
		else if (hidKeysDown() & KEY_Y) {
			memset(&grid, 0, 100);
			score = 0;
			selected_piece = 0;
			selected_tile = 0;
			getPieces(inventory);
		}
		
		gfxFlushBuffers();
		gfxSwapBuffers();
		
		gspWaitForVBlank();
	}
	
	gfxExit();
	return 0;
}
