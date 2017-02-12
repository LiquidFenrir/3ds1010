//convert a type 1 or 2 3ds1010 save to a type 3
//gcc saveconverter.c -o saveconverter && "./saveconverter"

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <malloc.h>

#define GETBIT(var, n) ((var >> n) & 1)

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32; ///< 32-bit unsigned integer

u8 grid[10][10] = {0};
u8 save_inventory[3] = {0};
u32 save_score = 0;
u32 save_highscore = 0;
char * themename;

void readSave(u8 inventory[3], u32 * score, u32 * highscore)
{
	
	FILE * fptr = fopen("./save.bin", "rb");
	if (fptr == NULL) {
		printf("Couldn't read save.\n");
		return;
	}
	
	fseek(fptr, 0, SEEK_END);
	u32 size = ftell(fptr);
	fseek(fptr, 0, SEEK_SET);
	
	u16 line = 0; //each line is 10 * 2bits = 20 bits, so need a u32
	
	for (int i = 0; i < 10; i++) {
		fread(&line, 2, 1, fptr);
		for (int j = 0; j < 10; j++) {
			grid[i][j] = (u8 )(GETBIT(line, j));
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
	
	if (size == 0x31) {
		themename = "Default";
	}
	else {
		u8 * nameBuf = malloc(themeNameLength+1); // +1 for the ending nullbyte
		memset(nameBuf, 0, themeNameLength+1);
		
		fread(nameBuf, 1, themeNameLength, fptr);
		themename = strdup((char *)nameBuf);
		
		free(nameBuf);
	}
	
	fclose(fptr);
}

void saveToFile(u8 inventory[3], u32 score, u32 highscore)
{
	
	FILE * fptr = fopen("./save.bin", "wb");
	if (fptr == NULL) {
		printf("Couldn't save\n");
		return;
	}
	
	for (int i = 0; i < 10; i++) {
		u32 line = 0;
		for (int j = 0; j < 10; j++) {
			line |= grid[i][j] << (j*2);
		}
		fwrite(&line, 4, 1, fptr);
	}
	
	for (int i = 0; i < 3; i++) {
		fwrite(&inventory[i], 1, 1, fptr);
	}
	
	fwrite(&score, 4, 1, fptr);
	fwrite(&highscore, 4, 1, fptr);
	
	u8 len = strlen(themename);
	fwrite(&len, 1, 1, fptr);
	fwrite(themename, len, 1, fptr);
	
	fclose(fptr);
}

int main(int argc, char ** argv)
{
	printf("Saveconverter for 3ds1010\n"
	       "Convert a type 1 or 2 save to a type 3\n"
	       "Just run this in the folder with the save and you're ready to play the latest version of 3ds1010!\n");
	
	readSave(save_inventory, &save_score, &save_highscore);
	
	printf(" Inventory contained:\n %u\n %u\n %u\n", save_inventory[0], save_inventory[1], save_inventory[2]);
	printf(" Score: %lu\n", save_score);
	printf(" Highscore: %lu\n", save_highscore);
	printf(" Theme (ignore if you converted a type 1): %s\n", themename);
	
	saveToFile(save_inventory, save_score, save_highscore);
	
	printf("Done! If you didnt see an error, you should be good to go. If you did see an error, you didnt run this in the same folder as the save.bin\n");
}