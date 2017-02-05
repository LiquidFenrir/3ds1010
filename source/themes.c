#include "themes.h"

Theme currentTheme;

void loadTheme()
{
	char themedir[256];
	if (strcmp(currentTheme.name, DEFAULT_THEME) == 0) {
		sprintf(themedir, "romfs:/");
	}
	else {
		sprintf(themedir, "%s%s/%s/", WORKING_DIR, "Themes", currentTheme.name);
	}
	
	char spritespath[256];
	sprintf(spritespath, "%s%s", themedir, "sprites.png");
	FILE * fptr = fopen(spritespath, "rb");
	
	fseek(fptr, 0, SEEK_END);
	currentTheme.spritesize = ftell(fptr);
	fseek(fptr, 0, SEEK_SET);
	
	u8 * buf = malloc(currentTheme.spritesize);
	fread(buf, currentTheme.spritesize, 1, fptr);
	
	currentTheme.sprite = buf;
	
	fclose(fptr);
	
	char filepath[256];
	sprintf(filepath, "%s%s", themedir, "bgcolor.bin");
	FILE * colorfptr = fopen(filepath, "rb");
	fread(&currentTheme.bgColor, 3, 1, colorfptr);
	currentTheme.bgColor =  __builtin_bswap32(currentTheme.bgColor) | 0xFF; //0xFF alpha
	fclose(colorfptr);
}

void freeTheme()
{
	free(currentTheme.sprite);
}