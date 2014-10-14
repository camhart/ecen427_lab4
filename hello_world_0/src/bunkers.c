#include <xparameters.h>

#include "displayControl.h"
#include "globals.h"
#include "aliens.h"

//This section contains all the compressed bitmap values for each sprite.
//Each number is an integer corresponding to a row of binary pixels.
//Ex. 4095 = 1111111111 (a row of all white)
int bunkers[6][12] = {	//good bunker bitmaps for each 4 position types
		{63, 63, 255, 255, 1023, 1023, 4095, 4095, 4095, 4095, 4095, 4095},			//0
		{4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095},	//1
		{4032, 4032, 4080, 4080, 4092, 4092, 4095, 4095, 4095, 4095, 4095, 4095},	//2
		{4032, 4032, 3840, 3840, 3072, 3072, 0, 0, 0, 0, 0, 0},
		{63, 63, 15, 15, 3, 3, 0, 0, 0, 0, 0, 0},									//4
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};
int damaged[5][12] = {	//damaged bunker bitmaps
		{0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF},	//full
		{3135, 3135, 4092, 4092, 207, 207, 1023, 1023, 3855, 3855, 4083, 4083},	//damaged once
		{51, 51, 828, 828, 192, 192, 1023, 1023, 3084, 3084, 3123, 3123},	//hit twice
		{0, 0, 780, 780, 192, 192, 195, 195, 3084, 3084, 0, 0},	//almost gone!
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}	//toast
};
//bunkerMap is used to keep track of which bitmap fits each of the 16 squares making a bunker
int* bunkerMap[16] = { bunkers[0], bunkers[1], bunkers[1], bunkers[2],
					bunkers[1],bunkers[1],bunkers[1],bunkers[1],
					bunkers[1],bunkers[3],bunkers[4],bunkers[1],
					bunkers[1],bunkers[5],bunkers[5],bunkers[1]
};

int getErodeValue() {
	//get bunker part to erode, but avoid the two already blank
	int val = rand()%16;
	if(val == 14 || val == 13)
		return getErodeValue();
	return val;
}


int getBunkerPixel(int bunkerState, int bunkerPart, int x, int y){
	//bunkerPart is which location in a bunker it is
	if (bunkerMap[bunkerPart][y] & damaged[bunkerState][y] & (1<<(11-x)))	//shift on integer to get individual bit
		return 0x00FF00;
	return 0;
}
//array to keep track of how damaged each bunker is
int bunkerState[4][16] = {
	{0,0,0,0,0,0,0,0,0,0,0,0,0,5,5,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,5,5,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,5,5,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,5,5,0}
};
//setup bunkers inially
void initializeBunkers(){
	int i;
	for(i = 0; i < 16; i++){
		drawBunkerPart(0, i);
		drawBunkerPart(1, i);
		drawBunkerPart(2, i);
		drawBunkerPart(3, i);
	}
}
//increment bunker's erosion state and redraw it
void erodeBunker(int bunker, int bunkerPart){
	bunkerState[bunker][bunkerPart]++;
	drawBunkerPart(bunker, bunkerPart);
}
//draw bunker
void drawBunkerPart(int bunker, int bunkerPart){
	int curRow;
	int curCol;
	int fb_row;
	int rowDiff;
	if(bunkerState[bunker][bunkerPart] < 5){	//only draw it if that bunker piece still exists
		switch(bunker){	//pick which bunker it is, set column value to the corner
		case(0):
			curCol = 90;
			break;
		case(1):
			curCol = 229;
			break;
		case(2):
			 curCol = 367;
			break;
		default:
			curCol = 506;
			break;
		}
		int startCol = curCol + (bunkerPart%4)*12;	//shift to specific piece of bunker
		int startRow = 380 + (bunkerPart/4)*12;	//shift to specific piece of bunker
		int stopRow = startRow + 12;
		int stopCol = startCol + 12;
		for(curRow = startRow; curRow < stopRow; curRow++){	//iterate through and paint
			fb_row = curRow*640;
			rowDiff = curRow - startRow;
			for(curCol = startCol; curCol < stopCol; curCol++){
				framebuffer[fb_row+curCol] = getBunkerPixel(bunkerState[bunker][bunkerPart], bunkerPart, curCol - startCol, rowDiff);
			}
			curCol -= 12;
		}
	}
}

int detectBunkerHit(int x1, int x2, int y1, int y2) {
	int bunker, curCol;
	for(bunker = 0; bunker < 4; bunker++) {
		switch(bunker) {
			case(0):
				curCol = 90;
				break;
			case(1):
				curCol = 229;
				break;
			case(2):
				 curCol = 367;
				break;
			default:
				curCol = 506;
				break;
		}
		int bunkerPart;
		for(bunkerPart = 0; bunkerPart < 16; bunkerPart++) {
			if(bunkerPart == 13 || bunkerPart == 14) {
				continue;
			}
			else if(bunkerState[bunker][bunkerPart] < 5) {
				int startCol = curCol + (bunkerPart%4)*12;	//shift to specific piece of bunker
				int startRow = 380 + (bunkerPart/4)*12;	//shift to specific piece of bunker
				int stopRow = startRow + 12;
				int stopCol = startCol + 12;
				if(x1 <= stopCol && x2 >= startCol && y1 <= stopRow && y2 >= startRow) {
//					xil_printf("erode %d %d, ", bunker, bunkerPart);
					erodeBunker(bunker, bunkerPart);
					return 1;
				}
			}
		}
	}
	return 0;
}


