#include <xparameters.h>

#include "displayControl.h"
#include "globals.h"
#include "scoreboard.h"

#define SCORE_START_POSITION_X1 24
#define SCORE_STOP_POSITION_X1 52
#define SCORE_START_POSITION_X2 53
#define SCORE_STOP_POSITION_X2 81
#define LIVES_START_POSITION_X1 400
#define LIVES_STOP_POSITION_X1 430
#define LIVES_START_POSITION_X2 431
#define LIVES_STOP_POSITION_X2 450
#define NUMBER_START_POS_X 90
#define NUMBER_WIDTH 12
#define TOP_ROW 6
#define BOTTOM_ROW 15
#define TEXT_HEIGHT 10

//int bitmap of score, level, and numbers
int score_part1[TEXT_HEIGHT] = {66863043, 66863043, 201375756, 201375756, 66109452, 66109452, 835596, 835596, 267403203, 267403203};
int score_part2[TEXT_HEIGHT] = {1010811903, 1010811903, 53490432, 53490432, 54510588, 54510588, 53490432, 53490432, 1009791999, 1009791999};
int lives_part1[TEXT_HEIGHT] = {1611030631, 1611030631, 1611030630, 1611030630, 1611030631, 1611030631, 1611012486, 1611012486, 2145781255, 2145781255};
int tankLife[12] = {12288,12288,64512,64512,16777212,16777212,67108863,67108863,67108863,67108863,67108863,67108863};
int lives_part2[TEXT_HEIGHT] = {520447, 520447, 768, 768, 508156, 508156, 3, 3, 521212, 521212};
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
static int isNotFirstDigit = 0;
static int whichDigit = 0;

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

inline int getLives1Pixel(int row, int col){
	//access integer array for each pixel
	if((lives_part1[row] & (1<<(30-col))))	//shift on integer to get individual bit
		return 0xFFFFFF;
	return 0;
}

inline int getLives2Pixel(int row, int col){
	//access integer array for each pixel
	if((lives_part2[row] & (1<<(18-col))))	//shift on integer to get individual bit
		return 0xFFFFFF;
	return 0;
}


int getTankLifePixel(int row, int col) {
	if(col < 0 || col > 32)
		return 0;	//erase before/after tank	}
	else if ((tankLife[row] & (1<<(31-col))))	//shift on integer to get individual bit
		return 0x00FF00;	//draw tank body
	else return 0;
}

void drawScore(){
	int curRow, curCol;
	for(curRow = TOP_ROW; curRow <= BOTTOM_ROW; curRow++) {	//step through row
		int fb_row = curRow*640;	//get position in framebuffer
		int rowDiff = curRow - TOP_ROW;
		//score part 1 paint
		for(curCol = SCORE_START_POSITION_X1; curCol <= SCORE_STOP_POSITION_X1; curCol++) {
			//get the specific pixel value for the score block and assign framebuffer
			int now = getScore1Pixel(rowDiff, curCol - SCORE_START_POSITION_X1);
			framebuffer[fb_row + curCol] = now;
			framebuffer[fb_row + (++curCol)] = now;
		}
		//score part 2 paint
		for(curCol = SCORE_START_POSITION_X2; curCol <= SCORE_STOP_POSITION_X2; curCol++) {
			//get the specific pixel value for the score block and assign framebuffer
			int now = getScore2Pixel(rowDiff, curCol - SCORE_START_POSITION_X2);
			framebuffer[fb_row + curCol] = now;
			framebuffer[fb_row + (++curCol)] = now;
		}
	}
}
void drawLives(){
	int curRow, curCol;
	for(curRow = TOP_ROW; curRow <= BOTTOM_ROW; curRow++) {	//step through row
		int fb_row = curRow*640;	//get position in framebuffer
		int rowDiff = curRow - TOP_ROW;
		//score part 1 paint
		for(curCol = LIVES_START_POSITION_X1; curCol <= LIVES_STOP_POSITION_X1; curCol++) {
			//get the specific pixel value for the score block and assign framebuffer
			int now = getLives1Pixel(rowDiff, curCol - LIVES_START_POSITION_X1);
			framebuffer[fb_row + curCol] = now;
			framebuffer[fb_row + (++curCol)] = now;
		}
		//score part 2 paint
		for(curCol = LIVES_START_POSITION_X2; curCol <= LIVES_STOP_POSITION_X2; curCol++) {
			//get the specific pixel value for the score block and assign framebuffer
			int now = getLives2Pixel(rowDiff, curCol - LIVES_START_POSITION_X2);
			framebuffer[fb_row + curCol] = now;
			framebuffer[fb_row + (++curCol)] = now;
		}
	}
}

void drawTankLife(int pos){
	int fb_row;
	int rowDiff;
	int curRow;
	int curCol;
	int startCol = 456 + pos*32;
	int stopCol = startCol+32;
	for(curRow = TOP_ROW; curRow <= BOTTOM_ROW+2; curRow++){
		fb_row = curRow*640;
		rowDiff = curRow - TOP_ROW;
		//iterate through the row/column and get individual pixel values
		for(curCol = startCol; curCol <= stopCol; curCol++){
			framebuffer[fb_row+curCol] = getTankLifePixel(rowDiff, curCol - startCol);
		}
	}
}

void drawTankLives(int lives){
	int i;
	for(i=lives; i > 0; i--){
		drawTankLife(i);
	}
}

void resetFirstDigit(){
	isNotFirstDigit = 0;
}

void paintScore(int score, int firstTime){
	if(firstTime){
		resetFirstDigit();	//we're starting a new score display
	}
	if(score/10 > 0){
		paintScore(score/10, 0);	//
	}
	else{
		if(isNotFirstDigit){
			paintDigit(++whichDigit, score);
		}
		else{	//we've hit the lowest digit, so start counting up the digits now
			whichDigit = 1;
			paintDigit(whichDigit, score);
			isNotFirstDigit = 1;
		}
	}
	//paintDigit(digitCount, score);
	//xil_printf("%d", score);
	//while(score = score/10){
//		xil_printf("%d", score);
//		digitCount++;
//		paintDigit(digitCount, score);
//		if(digitCount > 9){
//			paintScore(999999999);
//		}
//	}
}

void paintDigit(int position, int value){
	xil_printf("%d",value);
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
	for(curRow = TOP_ROW; curRow <= BOTTOM_ROW; curRow++) {	//step through row
		int fb_row = curRow*640;	//get position in framebuffer
		int rowDiff = curRow - TOP_ROW;
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


void initializeScore(){
	drawScore();
	drawLives();
	drawTankLives(3);
	paintScore(1235679, 1);
}


