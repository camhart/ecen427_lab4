#include <xparameters.h>

#include "displayControl.h"
#include "globals.h"

int tank[12] = {12288,12288,64512,64512,16777212,16777212,67108863,67108863,67108863,67108863,67108863,67108863};

int tankPosX = 152;	//tank location

static int tankMissile = 0;	//is there a tank missile?
static int tankMissileX = -1;	//location
static int tankMissileY = 438;	//location


void fireTankMissile(){
	if(!tankMissile){	//only shoot if there isn't a bullet out alread
		tankMissile = 1;
		tankMissileX = tankPosX + 18;	//move 18 pixels up
	}
}

void updateTankMissile(int changePosition){
	if(changePosition && tankMissile) {
		tankMissileY -= 10;
	}
	if(tankMissileY < 0){	//if we've hit the end of the screen, remove missile entirely
		eraseTankMissile();
		tankMissile = 0;
		tankMissileY = 438;
	}
}
//clean up to avoid bugs when writing values outside of the framebuffer
void eraseTankMissile(){
	int cy, cx;
	for(cy = tankMissileY-10; cy < tankMissileY + 12; cy++) {
		for(cx = tankMissileX; cx < tankMissileX + 2; cx++) {
			if(cy > tankMissileY) {
				framebuffer[cy * 640 + cx] = 0;
			}
		}
	}
}
//actual drawing of missile
void drawTankMissile(){
	int cx, cy;
	if(tankMissile){	//if there's a missile, step through its location rows/columns
		for(cy = tankMissileY-10; cy < tankMissileY + 12; cy++) {
			for(cx = tankMissileX; cx < tankMissileX + 2; cx++) {
				if(cy > tankMissileY) {
					framebuffer[cy * 640 + cx] = 0;	//erase previous missle
				} else {
					framebuffer[cy * 640 + cx] = 0xFFFFFF;	//paint missile white
				}
			}
		}
	}
}

int getTankPixel(int row, int col) {
	if(col < 0 || col > 32)
		return 0;	//erase before/after tank	}
	else if ((tank[row] & (1<<(31-col))))	//shift on integer to get individual bit
		return 0x00FF00;	//draw tank body
	else return 0;
}

void drawTank(){
	int fb_row;
	int rowDiff;
	int curRow;
	int curCol;
	for(curRow = 448; curRow < 460; curRow++){
		fb_row = curRow*640;
		rowDiff = curRow - 448;
		//iterate through the row/column and get individual pixel values
		for(curCol = tankPosX-2; curCol < tankPosX + 34; curCol++){
			framebuffer[fb_row+curCol] = getTankPixel(rowDiff, curCol - tankPosX);
		}
	}
}
