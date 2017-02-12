#pragma once

#include <3ds.h>
#include <citro3d.h>

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "themes.h"

#define GETBIT(var, n) ((var >> n) & 1)

#define PIECES_AMOUNT 21
#define WORKING_DIR "/3ds/3ds1010/"
#define DEFAULT_THEME "Default"

typedef struct {
	u8 height;
	u8 width;
	/*
	pieces are divided into "blocks", the grid is 10x10 blocks, the biggest piece has 25 blocks (the 5x5 square)
	some pieces have blank spaces in them (the corner pieces) so this was the simplest way I found to represent the blank blocks
	*/
	u32 blocks;
} piece;

extern piece piecesType[PIECES_AMOUNT];
extern u8 grid[10][10];