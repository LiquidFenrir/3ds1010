#include "pieces.h"

void generatePiecesSquares() //5 kinds of squares - from 1 to 5 blocks of side
{
	for (int i = 1; i <= 5; i++) {
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
}

void getPieces(u8 inventory[3]) //need better RNG
{
	srand(time(NULL));
	for (int i = 0; i < 3; i++) {
		inventory[i] = random() % 5 + 1;
	}
}