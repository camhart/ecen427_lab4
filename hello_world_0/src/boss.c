#include <xparameters.h>

#include "displayControl.h"
#include "globals.h"
#include "boss.h"

int bossPosX = 2; //boss starting point
int bossTopRow = 20;	//boss top Y position
int bossBottomRow = 34;	//boss bottom Y position
static int bossDir = 1;	 //right (direction boss is moving)

//int bitmap of ship
int boss[BOSS_HEIGHT] = {4193280, 4193280, 67108800, 67108800, 268435440, 268435440, 1022611260, 1022611260, 4294967295L, 4294967295L, 264487920, 264487920, 50331840, 50331840};

void moveBoss(){
	bossPosX += bossDir;
	drawBoss();
}

void initializeBoss(){
	if(rand()%2){
		bossDir = 1;	//boss goes right
		bossPosX = 2;	//starting left
	}
	else{
		bossDir = -1;		//boss goes left
		bossPosX = 640-42;	//starting right
	}
	drawBoss();
}

//returns the color that belongs in the given location
inline int getBossPixel(int row, int col){
	//access integer array for each pixel
	if((boss[row] & (1<<(31-col))))	//shift on integer to get individual bit
		return 0xFF0000;
	return 0;
}

void drawBoss(){
	int curRow, curCol;
	int endPaintColumn = bossPosX+34;	//when to stop painting completely
	int endBossColumn = endPaintColumn - 2;	//when the start erasing
	for(curRow = bossTopRow; curRow < bossBottomRow; curRow++) {	//step through row
		int fb_row = curRow*640;	//get position in framebuffer
		int rowDiff = curRow - bossTopRow;
		for(curCol = bossPosX - 2; curCol < endPaintColumn; curCol++) {
			if(curCol < bossPosX) {	//erase left
				framebuffer[fb_row + curCol] = 0;
			} else if(curCol >= endBossColumn) {	//erase right
				framebuffer[fb_row + curCol] = 0;
			}
			else {	//get the specific pixel value for the boss block and assign framebuffer
				int now = getBossPixel(rowDiff, curCol - bossPosX);
				framebuffer[fb_row + curCol] = now;
				framebuffer[fb_row + (++curCol)] = now;
			}
		}
	}
}

void killBoss(){
	int bonus = ((rand()%6)+1)*50;
	addScore(bonus);
	drawNumbers(bonus, bossPosX, bossTopRow);
}
