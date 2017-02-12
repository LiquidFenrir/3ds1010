#include "grid.h"

u8 checkGrid()
{
	u8 completeAmount = 0;
	u8 completeLines[10] = {[0 ... 9] = 3};
	u8 completeColumns[10] = {[0 ... 9] = 3};
	
	for (int i = 0; i < 10; i++) { //checking if the line/column is complete
		for (int j = 0; j < 10; j++) {
			if (grid[i][j] == 0) {
				completeLines[i] = 0;
			}
			if (grid[j][i] == 0) {
				completeColumns[i] = 0;
			}
		}
		completeAmount += completeColumns[i] + completeLines[i];
	}
	
	if (completeAmount == 0) //if no lines/columns have been completed, skip resetting
		return 0;
		
	for (int i = 0; i < 10; i++) { //resetting the lines/columns that are complete
		for (int j = 0; j < 10; j++) { 
			grid[i][j] = (grid[i][j] & ~(completeColumns[j] | completeLines[i]));
		}
	}
	
	return 10 * (completeAmount*(completeAmount + 1))/2; //worth (10 * the triangular number of lines/columns removed) points
}

static u8 testSpot(u8 selected_tile, piece selectedPiece, u8 place)
{
	if (selectedPiece.blocks == 0) //if the passed piece is blank, skip everything
		return 0;
	
	u8 newGrid[10][10] = {0};
	memcpy(&newGrid, &grid, 100);
	
	u8 startX = selected_tile/10;
	u8 startY = selected_tile%10;
	u8 endX = startX + selectedPiece.width;
	u8 endY = startY + selectedPiece.height;
	
	if (endX > 10 || endY > 10) //blocks would be out of the grid
		return 0;
	
	u8 curbit = 0;
	for (int i = startX; i < endX; i++) {
		for (int j = startY; j < endY; j++) {
			u8 curval = GETBIT(selectedPiece.blocks, curbit);
			curbit++;
			newGrid[i][j] |= curval*selectedPiece.type; //some pieces have blank spaces in them, don't want to unset blocks under the blanks
			if (grid[i][j] && curval) //if both are set at the same time, there's already a block where another block would end up = the piece can't be placed
				return 0;
		}
	}
	
	if (place != 0)
		memcpy(&grid, &newGrid, 100);
	
	return __builtin_popcount(selectedPiece.blocks); //each piece is worth the number of blocks it contains
}

u8 placePiece(u8 selected_tile, piece selectedPiece)
{
	return testSpot(selected_tile, selectedPiece, 1);
}

static u8 checkPiece(piece piece)
{
	for (int i = 0; i < 10; i++) {
		for (int j = 0;j < 10; j++) {
			if (testSpot(j+ i*10, piece, 0) != 0) {
				return 1;
			}
		}
	}
	return 0;
}

u8 checkInventory(u8 inventory[3])
{
	u8 possible = 0;
	for (int i = 0; i < 3; i++) {
		possible |= checkPiece(piecesType[inventory[i]]);
	}
	return possible;
}