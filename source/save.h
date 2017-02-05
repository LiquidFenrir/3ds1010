#include "common.h"
#include "themes.h" //for the currentTheme - accessing the name

void readSave(u8 inventory[3], u32 * score, u32 * highscore);
void saveToFile(u8 inventory[3], u32 score, u32 highscore);