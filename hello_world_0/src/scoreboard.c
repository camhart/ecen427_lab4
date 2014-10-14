#include <xparameters.h>

#include "displayControl.h"
#include "globals.h"
#include "scoreboard.h"

#define SCORE_START_POSITION_X1 24
#define SCORE_STOP_POSITION_X1 52
#define SCORE_START_POSITION_X2 53
#define SCORE_STOP_POSITION_X2 81
#define NUMBER_START_POS_X 90
#define NUMBER_WIDTH 12
#define TOP_ROW 6
#define BOTTOM_ROW 15
#define TEXT_HEIGHT 10

//int bitmap of score, level, and numbers
int score_part1[TEXT_HEIGHT] = {66863043, 66863043, 201375756, 201375756, 66109452, 66109452, 835596, 835596, 267403203, 267403203};
int score_part2[TEXT_HEIGHT] = {1010811903, 1010811903, 53490432, 53490432, 54510588, 54510588, 53490432, 53490432, 1009791999, 1009791999};
int one[TEXT_HEIGHT] = {15, 15, 3, 3, 3, 3, 3, 3, 3, 3};
int two[TEXT_HEIGHT] = {1020, 1020, 3, 3, 255, 255, 768, 768, 1023, 1023};
int three[TEXT_HEIGHT] = {1020, 1020, 3, 3, 255, 255, 3, 3, 1020, 1020};
int four[TEXT_HEIGHT] = {771, 771, 771, 771, 1023, 1023, 3, 3, 3, 3};
int five[TEXT_HEIGHT] = {1023, 1023, 768, 768, 1020, 1020, 3, 3, 1020, 1020};
int six[TEXT_HEIGHT] = {252, 252, 768, 768, 1020, 1020, 771, 771, 252, 252};
int seven[TEXT_HEIGHT] = {1023, 1023, 3, 3, 3, 3, 12, 12, 12, 12};
int eight[TEXT_HEIGHT] = {252, 252, 771, 771, 252, 252, 771, 771, 252, 252};
int nine[TEXT_HEIGHT] = {252, 252, 771, 771, 255, 255, 3, 3, 252, 252};
int zero[TEXT_HEIGHT] = {252, 252, 771, 771, 771, 771, 771, 771, 252, 252};

//returns the color that belongs in the given location
inline int getScore1Pixel(int row, int col){
	//access integer array for each pixel
	if((score_part1[row] & (1<<(29-col))))	//shift on integer to get individual bit
		return 0xFFFFFF;
	return 0;
}

inline int getScore2Pixel(int row, int col){
	//access integer array for each pixel
	if((score_part2[row] & (1<<(29-col))))	//shift on integer to get individual bit
		return 0xFFFFFF;
	return 0;
}

inline int get1Pixel(int row, int col){
	//access integer array for each pixel
	if((one[row] & (1<<(12-col))))	//shift on integer to get individual bit
		return 0xFF0000;
	return 0;
}
inline int get2Pixel(int row, int col){
	//access integer array for each pixel
	if((two[row] & (1<<(12-col))))	//shift on integer to get individual bit
		return 0xFF0000;
	return 0;
}
inline int get3Pixel(int row, int col){
	//access integer array for each pixel
	if((three[row] & (1<<(12-col))))	//shift on integer to get individual bit
		return 0xFF0000;
	return 0;
}
inline int get4Pixel(int row, int col){
	//access integer array for each pixel
	if((four[row] & (1<<(12-col))))	//shift on integer to get individual bit
		return 0xFF0000;
	return 0;
}
inline int get5Pixel(int row, int col){
	//access integer array for each pixel
	if((five[row] & (1<<(12-col))))	//shift on integer to get individual bit
		return 0xFF0000;
	return 0;
}
inline int get6Pixel(int row, int col){
	//access integer array for each pixel
	if((six[row] & (1<<(12-col))))	//shift on integer to get individual bit
		return 0xFF0000;
	return 0;
}
inline int get7Pixel(int row, int col){
	//access integer array for each pixel
	if((seven[row] & (1<<(12-col))))	//shift on integer to get individual bit
		return 0xFF0000;
	return 0;
}
inline int get8Pixel(int row, int col){
	//access integer array for each pixel
	if((eight[row] & (1<<(12-col))))	//shift on integer to get individual bit
		return 0xFF0000;
	return 0;
}
inline int get9Pixel(int row, int col){
	//access integer array for each pixel
	if((nine[row] & (1<<(12-col))))	//shift on integer to get individual bit
		return 0xFF0000;
	return 0;
}
inline int get0Pixel(int row, int col){
	//access integer array for each pixel
	if((zero[row] & (1<<(12-col))))	//shift on integer to get individual bit
		return 0xFF0000;
	return 0;
}


void drawScore(){
	int curRow, curCol;
	for(curRow = TOP_ROW; curRow < BOTTOM_ROW; curRow++) {	//step through row
		int fb_row = curRow*640;	//get position in framebuffer
		int rowDiff = curRow - TOP_ROW;
		//score part 1 paint
		for(curCol = SCORE_START_POSITION_X1; curCol < SCORE_STOP_POSITION_X1; curCol++) {
			//get the specific pixel value for the score block and assign framebuffer
			int now = getScore1Pixel(rowDiff, curCol - SCORE_START_POSITION_X1);
			framebuffer[fb_row + curCol] = now;
			framebuffer[fb_row + (++curCol)] = now;
		}
		//score part 2 paint
		for(curCol = SCORE_START_POSITION_X2; curCol < SCORE_STOP_POSITION_X2; curCol++) {
			//get the specific pixel value for the score block and assign framebuffer
			int now = getScore2Pixel(rowDiff, curCol - SCORE_START_POSITION_X2);
			framebuffer[fb_row + curCol] = now;
			framebuffer[fb_row + (++curCol)] = now;
		}
	}
}

void paintScore(int score){
	int digitCount = 1;
	paintDigit(digitCount, score);
	while(score = score/10){
		digitCount++;
		paintDigit(digitCount, score);
		if(digitCount > 9){
			paintDigit(9, 999999999);
		}
	}
}

void paintDigit(int position, int value){
	switch(value){
	case(0):
		drawDigit(zero, position);
	case(1):
		drawDigit(one, position);
	case(2):
		drawDigit(two, position);
	case(3):
		drawDigit(three, position);
	case(4):
		drawDigit(four, position);
	case(5):
		drawDigit(five, position);
	case(6):
		drawDigit(six, position);
	case(7):
		drawDigit(seven, position);
	case(8):
		drawDigit(eight, position);
	default:
		drawDigit(nine, position);
	}
}

void drawDigit(int num[TEXT_HEIGHT], int pos){
	int curRow,curCol;
	int stopCol = pos*NUMBER_WIDTH + NUMBER_START_POS_X;
	int startCol = (pos-1)*NUMBER_WIDTH + NUMBER_START_POS_X;
	for(curRow = TOP_ROW; curRow < BOTTOM_ROW; curRow++) {	//step through row
		int fb_row = curRow*640;	//get position in framebuffer
		int rowDiff = curRow - TOP_ROW;
		for(curCol = startCol; curCol < stopCol; curCol++) {
			//get the specific pixel value for the score block and assign framebuffer
			int now = (num[rowDiff] & (1<<(12-curCol-startCol)));	//shift on integer to get individual bit
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


void initializeScore(){
	drawScore();
	paintScore(0);
}


