#include "save.h"

void readSave(u8 inventory[3], u32 * score, u32 * highscore)
{
	char savepath[256];
	sprintf(savepath, "%s%s", WORKING_DIR, "save.bin");
	
	FILE * fptr = fopen(savepath, "rb");
	if (fptr == NULL) {
		printf("Couldn't read save.\n");
		return;
	}
	
	u16 line = 0; //each line is 10 number = 10 bytes, so can't use a u8 (only 8 bits)
	
	for (int i = 0; i < 10; i++) {
		fread(&line, 2, 1, fptr);
		for (int j = 0; j < 10; j++) {
			grid[i][j] = (u8 )GETBIT(line, j);
		}
	}
	
	u8 piece = 0;
	for (int i = 0; i < 3; i++) {
		fread(&piece, 1, 1, fptr);
		inventory[i] = piece;
	}
	
	u32 savedScore = 0;
	fread(&savedScore, 4, 1, fptr);
	*score = savedScore;
	
	u32 savedHighScore = 0;
	fread(&savedHighScore, 4, 1, fptr);
	*highscore = savedHighScore;
	
	u8 themeNameLength = 0;
	fread(&themeNameLength, 1, 1, fptr);
	
	u8 * nameBuf = malloc(themeNameLength+1); // +1 for the ending nullbyte
	memset(nameBuf, 0, themeNameLength+1);
	
	fread(nameBuf, 1, themeNameLength, fptr);
	currentTheme.name = strdup((char *)nameBuf);
	
	free(nameBuf);
	fclose(fptr);
}

void saveToFile(u8 inventory[3], u32 score, u32 highscore)
{
	char savepath[256];
	sprintf(savepath, "%s%s", WORKING_DIR, "save.bin");
	
	FILE * fptr = fopen(savepath, "wb");
	if (fptr == NULL) {
		printf("Couldn't save, your SD may have a problem.\n");
		return;
	}
	
	for (int i = 0; i < 10; i++) {
		u16 line = 0;
		for (int j = 0; j < 10; j++) {
			line |= grid[i][j] << j;
		}
		fwrite(&line, 2, 1, fptr);
	}
	
	for (int i = 0; i < 3; i++) {
		fwrite(&inventory[i], 1, 1, fptr);
	}
	
	fwrite(&score, 4, 1, fptr);
	fwrite(&highscore, 4, 1, fptr);
	
	u8 len = strlen(currentTheme.name);
	fwrite(&len, 1, 1, fptr);
	fwrite(currentTheme.name, len, 1, fptr);
	
	fclose(fptr);
}