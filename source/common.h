#pragma once

#include <3ds.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define GETBIT(var, n) ((var >> n) & 1)

typedef struct {
	u8 height;
	u8 width;
	/*
	pieces are divided into "blocks", the grid is 10x10 blocks, the biggest piece has 25 blocks (the 5x5 square)
	some pieces have blank spaces in them (the corner pieces) so this was the simplest way I found to represent the blank blocks
	*/
	u32 blocks;
} piece;

extern piece piecesType[20];
extern u8 grid[10][10];