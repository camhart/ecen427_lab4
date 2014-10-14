#include <xparameters.h>

#include "displayControl.h"
#include "globals.h"
#include "scoreboard.h"

#define SCORE_START_POSITION_X1 24
#define SCORE_STOP_POSITION_X1 52
#define SCORE_START_POSITION_X2 53
#define SCORE_STOP_POSITION_X2 81
#define TOP_ROW 6
#define BOTTOM_ROW 15
#define TEXT_HEIGHT 10

//int bitmap of score, level, and numbers
int score_part1[TEXT_HEIGHT] = {133726087, 133726087, 402751512, 402751512, 132218904, 132218904, 1671192, 1671192, 534806407, 534806407};
int score_part2[TEXT_HEIGHT] = {133726087, 133726087, 402751512, 402751512, 132218904, 132218904, 1671192, 1671192, 534806407, 534806407};

void initializeScore(){
	paintScoreText();
	paintScore(0);
}

//returns the color that belongs in the given location
inline int getScore1Pixel(int row, int col){
	//access integer array for each pixel
	if((score_part1[row] & (1<<(31-col))))	//shift on integer to get individual bit
		return 0xFF0000;
	return 0;
}

inline int getScore2Pixel(int row, int col){
	//access integer array for each pixel
	if((score_part2[row] & (1<<(31-col))))	//shift on integer to get individual bit
		return 0xFF0000;
	return 0;
}

void drawBoss(){
	int curRow, curCol;
	//score part 1 paint
	for(curRow = TOP_ROW; curRow < BOTTOM_ROW; curRow++) {	//step through row
		int fb_row = curRow*640;	//get position in framebuffer
		int rowDiff = curRow - BOTTOM_ROW;
		for(curCol = SCORE_START_POSITION_X1; curCol < SCORE_STOP_POSITION_X1; curCol++) {
			//get the specific pixel value for the score block and assign framebuffer
			int now = getScore1Pixel(rowDiff, curCol - SCORE_START_POSITION_X1);
			framebuffer[fb_row + curCol] = now;
			framebuffer[fb_row + (++curCol)] = now;
		}
	}
	//score part 2 paint
	for(curRow = TOP_ROW; curRow < BOTTOM_ROW; curRow++) {	//step through row
		int fb_row = curRow*640;	//get position in framebuffer
		int rowDiff = curRow - BOTTOM_ROW;
		for(curCol = SCORE_START_POSITION_X2; curCol < SCORE_STOP_POSITION_X2; curCol++) {
			//get the specific pixel value for the score block and assign framebuffer
			int now = getScore2Pixel(rowDiff, curCol - SCORE_START_POSITION_X2);
			framebuffer[fb_row + curCol] = now;
			framebuffer[fb_row + (++curCol)] = now;
		}
	}
}

void paintScore(int score){

}


