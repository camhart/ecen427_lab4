#include <xparameters.h>

#include "displayControl.h"
#include "globals.h"

int tank[12] = {12288,12288,64512,64512,16777212,16777212,67108863,67108863,67108863,67108863,67108863,67108863};
int tankExplosionA[16] = {0, 0, 805503024, 805503024, 12582912, 12582912, 201523248, 201523248, 787203, 787203, 809250816, 809250816, 16776240, 16776240, 67108608, 67108608, 1073741808, 1073741808};
int tankExplosionB[16] = {3145728, 3145728, 0, 0, 50381568, 50381568, 3345600, 3345600, 50331696, 50331696, 847872, 847872, 822083328, 822083328, 67108800, 67108800, 1073741808, 1073741808};

int tankPosX = 152;	//tank location

int tankMissile = 0;	//is there a tank missile?
int tankMissileX = -1;	//location
int tankMissileY = 438;	//location


void fireTankMissile(){
	if(!tankMissile){	//only shoot if there isn't a bullet out alread
		tankMissile = 1;
		tankMissileX = tankPosX + 18;	//move 18 pixels up
	}
}

//clean up to avoid bugs when writing values outside of the framebuffer
void eraseTankMissile(){
	int cy, cx;
	for(cy = tankMissileY-10; cy < tankMissileY + 12; cy++) {
		for(cx = tankMissileX; cx < tankMissileX + 2; cx++) {
//			if(cy > tankMissileY && tankMissileY >= 0 && tankMissileY <= 639) {
				if(framebuffer[cy * 640 + cx] == 0xFFFFFF)
					framebuffer[cy * 640 + cx] = 0;
//			}
		}
	}
}

void updateTankMissile(int changePosition){
	if(changePosition && tankMissile) {
		tankMissileY -= 8;
	}
	if(tankMissileY < 30){	//if we've hit the end of the screen, remove missile entirely
		eraseTankMissile();
		tankMissile = 0;
		tankMissileY = 438;
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

void drawTankExplosionA(){
	int fb_row;
	int rowDiff;
	int curRow;
	int curCol;
	for(curRow = 444; curRow < 460; curRow++){
		fb_row = curRow*640;
		rowDiff = curRow - 444;
		//iterate through the row/column and get individual pixel values
		for(curCol = tankPosX-2; curCol < tankPosX + 34; curCol++){
			framebuffer[fb_row+curCol] = getTankExplosionPixelA(rowDiff, curCol - tankPosX);
		}
	}
}

int getTankExplosionPixelA(int row, int col) {
	if(col < 0 || col > 29)
		return 0;	//erase before/after tank	}
	else if ((tank[row] & (1<<(29-col))))	//shift on integer to get individual bit
		return 0x00FF00;	//draw tank body
	else return 0;
}

void drawTankExplosionB(){
	int fb_row;
	int rowDiff;
	int curRow;
	int curCol;
	for(curRow = 444; curRow < 460; curRow++){
		fb_row = curRow*640;
		rowDiff = curRow - 444;
		//iterate through the row/column and get individual pixel values
		for(curCol = tankPosX-2; curCol < tankPosX + 34; curCol++){
			framebuffer[fb_row+curCol] = getTankExplosionPixelB(rowDiff, curCol - tankPosX);
		}
	}
}

int getTankExplosionPixelB(int row, int col) {
	if(col < 0 || col > 29)
		return 0;	//erase before/after tank	}
	else if ((tank[row] & (1<<(29-col))))	//shift on integer to get individual bit
		return 0x00FF00;	//draw tank body
	else return 0;
}

//void eraseTankMissile2() {
//	int cx, cy;
//	for(cy = tankMissileY-10; cy < tankMissileY + 12; cy++) {
//		for(cx = tankMissileX; cx < tankMissileX + 2; cx++) {
//			framebuffer[cy * 640 + cx] = 0;	//erase previous missle
//		}
//	}
//}

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
