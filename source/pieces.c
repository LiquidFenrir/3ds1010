#include "pieces.h"

void getPieces(u8 inventory[3]) //need better RNG
{
	srand (time(NULL));
	for (u8 i = 0; i <3; i++) {
		inventory[i] = random() % 5 + 1;
	}
}