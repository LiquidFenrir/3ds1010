#include "pieces.h"

void generatePiecesCorners(u8 south, u8 east) //8 kinds of corners - 1 (2x2) + 1 (3x3), x4 for each corner
{
	u8 base = 13; //number of preexisting pieces - bars and squares
	u8 offset = (south*2 + east)*2;
	
	piecesType[base+offset].type = 3;
	piecesType[base+offset].height = 2;
	piecesType[base+offset].width = 2;
	piecesType[base+offset].blocks = 0xF; //2x2 square
	piecesType[base+offset].blocks &= ~(1 << (offset/2)); //remove the corner block corresponding to parameters
	
	piecesType[base+offset+1].type = 3;
	piecesType[base+offset+1].height = 3;
	piecesType[base+offset+1].width = 3;
	piecesType[base+offset+1].blocks = 0x1FF; //3x3 square
	piecesType[base+offset+1].blocks &= ~(0x1B << (offset/2));  //remove the corner 2x2 square corresponding to parameters
	if (offset == 4) piecesType[base+offset+1].blocks = 0x4F; //special case, could probably be removed if ^ was done better
}

void generatePiecesBars(u8 vertical) //8 kinds of bars - from 2 to 5 blocks of length (4), and vertical/horizontal (x2)
{
	u8 offset = 4+(4*vertical); //to not overwrite the pieces generated before
	
	for (int i = 2; i <= 5; i++) {
		piecesType[i+offset].type = 2;
		if (vertical == 0) {
			piecesType[i+offset].height = 1;
			piecesType[i+offset].width = i;
		}
		else {
			piecesType[i+offset].height = i;
			piecesType[i+offset].width = 1;
		}
		for (int j = 0; j < i; j++) {
			piecesType[i+offset].blocks |= (u32 )pow(2, j);
		}
	}
}

void generatePiecesSquares() //5 kinds of squares - from 1 to 5 blocks of side
{
	for (int i = 1; i <= 5; i++) {
		piecesType[i].type = 1;
		piecesType[i].height = i;
		piecesType[i].width = i;
		for (int j = 0; j < pow(i, 2); j++) {
			piecesType[i].blocks |= (u32 )pow(2, j);
		}
	}
}

void generatePiecesTypes()
{
	generatePiecesSquares();
	generatePiecesBars(0); //horizontal bars
	generatePiecesBars(1); //vertical bars
	generatePiecesCorners(0, 0); //points to north-west
	generatePiecesCorners(0, 1); //points to north-east
	generatePiecesCorners(1, 0); //points to south-west
	generatePiecesCorners(1, 1); //points to south-east
}

void getPieces(u8 inventory[3]) //need better RNG
{
	srand(time(NULL));
	for (int i = 0; i < 3; i++) {
		inventory[i] = (random() % (PIECES_AMOUNT - 1)) + 1;
	}
}