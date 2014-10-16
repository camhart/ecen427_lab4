#include <xparameters.h>

#include "displayControl.h"
#include "globals.h"
#include "boss.h"
#include "scoreboard.h"

int bossPosX = 2; //boss starting point
int bossTopRow = 20;	//boss top Y position
int bossBottomRow = 34;	//boss bottom Y position
int bossDir = 1;	 //right (direction boss is moving)
int bossExists = 0;

//int bitmap of ship
int boss[BOSS_HEIGHT] = {4193280, 4193280, 67108800, 67108800, 268435440, 268435440, 1022611260, 1022611260, 4294967295L, 4294967295L, 264487920, 264487920, 50331840, 50331840};


void eraseBoss(){
	int curRow, curCol;
	int endPaintColumn = bossPosX+34;	//when to stop painting
	for(curRow = bossTopRow; curRow < bossBottomRow; curRow++) {	//step through row
		int fb_row = curRow*640;	//get position in framebuffer
		for(curCol = bossPosX - 2; curCol < endPaintColumn; curCol++) {
			framebuffer[fb_row + curCol] = 0;
			framebuffer[fb_row + (++curCol)] = 0;
		}
	}
}

void moveBoss(){
	bossPosX += bossDir;
	if(bossPosX > 598){
		bossExists = 0;
		eraseBoss();
	}
	else if(bossPosX < 2){
		bossExists = 0;
		eraseBoss();
	}
	drawBoss();
}

void initializeBoss(){
	bossExists = 1;
	if(rand()%2){
		bossDir = 2;	//boss goes right
		bossPosX = 2;	//starting left
	}
	else{
		bossDir = -2;		//boss goes left
		bossPosX = 640-42;	//starting right
	}
}

//returns the color that belongs in the given location
inline int getBossPixel(int row, int col){
	//access integer array for each pixel
	if((boss[row] & (1<<(31-col))))	//shift on integer to get individual bit
		return 0xFF0000;
	return 0;
}

void drawBoss(){
	if(!bossExists){
		return 0;
	}
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

void drawNumbers(int num[TEXT_HEIGHT], int pos){
	int curRow,curCol;
	int startCol = pos;
	int stopCol = pos+12;
	for(curRow = bossTopRow; curRow <= bossTopRow + 9; curRow++) {	//step through row
		int fb_row = curRow*640;	//get position in framebuffer
		int rowDiff = curRow - bossTopRow;
		for(curCol = startCol; curCol <= stopCol; curCol++) {
			//get the specific pixel value for the score block and assign framebuffer
			int now = (num[rowDiff] & (1<<(12-(curCol-startCol))));	//shift on integer to get individual bit
			if(now){
				framebuffer[fb_row + curCol] = 0x00FF00;
				framebuffer[fb_row + (++curCol)] = 0x00FF00;
			}
			else{
				framebuffer[fb_row + curCol] = now;
				framebuffer[fb_row + (++curCol)] = now;
			}
		}
	}
}

void clearNumbers(){
	int curRow,curCol;
	int startCol = bossPosX;
	int stopCol = startCol+40;
	for(curRow = bossTopRow; curRow <= bossTopRow + 9; curRow++) {	//step through row
		int fb_row = curRow*640;	//get position in framebuffer
		int rowDiff = curRow - bossTopRow;
		for(curCol = startCol; curCol <= stopCol; curCol++) {
			framebuffer[fb_row + curCol] = 0;
			framebuffer[fb_row + (++curCol)] = 0;
		}
	}
}

void killBoss(){
	bossExists = 0;
	eraseBoss();
	int bonus = ((rand()%6)+1)*50;
	updateScore(bonus);
	if(bonus ==50){
		drawNumbers(five, bossPosX);
		drawNumbers(zero, bossPosX + 14);
	}
	else if(bonus == 100){
		drawNumbers(one, bossPosX);
		drawNumbers(zero, bossPosX + 14);
		drawNumbers(zero, bossPosX + 28);
	}
	else if(bonus == 150){
			drawNumbers(one, bossPosX);
			drawNumbers(five, bossPosX + 14);
			drawNumbers(zero, bossPosX + 28);
	}
	else if(bonus == 200){
			drawNumbers(two, bossPosX);
			drawNumbers(zero, bossPosX + 14);
			drawNumbers(zero, bossPosX + 28);
	}
	else if(bonus == 250){
			drawNumbers(two, bossPosX);
			drawNumbers(five, bossPosX + 14);
			drawNumbers(zero, bossPosX + 28);
	}
	else{
		drawNumbers(three, bossPosX);
		drawNumbers(zero, bossPosX + 14);
		drawNumbers(zero, bossPosX + 28);
	}
}

int detectBossHit(int x1, int x2, int y1, int y2) {
	int bossWidth = 32;
	int startBossX = bossPosX;
	int endBossX = bossPosX + bossWidth;
	int startBossY = bossTopRow;
	int endBossY = bossBottomRow;
//	if(x1 <= stopCol && x2 >= startCol && y1 <= stopRow && y2 >= startRow) {
	if(x1 <= endBossX && x2 >= startBossX && y1 <= endBossY && y2 >= startBossY) {
		return 1;
	}
	return 0;
}



