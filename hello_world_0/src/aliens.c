#include <xparameters.h>

#include "displayControl.h"
#include "globals.h"
#include "aliens.h"

//an array to keep track of alive/dead aliens
char alive[55] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

static int alienMoveCount = 18;	//how far has the alien moved so far?
int alienPosX = 152; //144 + 8 pixel offset
int alienPosY = 50;	//alien position global
static int alienDir = 1;	 //right (direction alien is moving)

//variables to trace missiles
static int missileX[4] = {-1, -1, -1, -1};	//x location
static int missileY[4] = {-1, -1, -1, -1};	//y location
static char missileType[4] = {-1, -1, -1, -1};	//type
static char missileNew[4] = {0, 0, 0, 0};	//is it new?
static int missileIndex = 0;	//how many are out there?

//int bitmaps for each alien
int topInAlien[ALIEN_HEIGHT] = {0x3C0000, 0x3C0000, 0xFF0000, 0xFF0000, 0x3FFC000, 0x3FFC000, 0xF3CF000, 0xF3CF000,	0xFFFF000, 0xFFFF000, 0x33CC000, 0x33CC000, 0xC003000, 0xC003000, 0x300C000, 0x300C000};
int topOutAlien[ALIEN_HEIGHT] = {3932160,3932160,16711680,16711680,67092480,67092480,255651840,255651840,268431360,268431360,12779520,12779520,54312960,54312960,214118400,214118400};
int middleInAlien[ALIEN_HEIGHT] = {50343936,50343936,12632064,12632064,67104768,67104768,255802368,255802368,1073741568,1073741568,872411904,872411904,855651072,855651072,15974400,15974400};
int middleOutAlien[ALIEN_HEIGHT] = {50343936,50343936,817939200,817939200,872411904,872411904,1061109504,1061109504,1073741568,1073741568,268434432,268434432,50343936,50343936,201329664,201329664};
int bottomOutAlien[ALIEN_HEIGHT] = {16744448,16744448,1073740800,1073740800,4294967040,4294967040,4231806720,4231806720,4294967040,4294967040,63160320,63160320,251719680,251719680,4026535680,4026535680};
int bottomInAlien[ALIEN_HEIGHT] = {16744448,16744448,1073740800,1073740800,4294967040,4294967040,4231806720,4231806720,4294967040,4294967040,264499200,264499200,1010580480,1010580480,251719680,251719680};


//int bitmaps for each missile type/orientation
int missiles[4][2][10] = {
	{	//cross top
		{96,96,1020,1020,96,96,96,96,96,96},
		{96,96,96,96,1020,1020,96,96,96,96},
	},
	{	//cross bottom
		{96,96,96,96,96,96,1020,1020,96,96},
		{96,96,96,96,1020,1020,96,96,96,96},
	},
	{	//zig zag
		{24, 24, 96, 96, 384, 384, 96, 96, 24, 24},
		{96, 96, 384, 384, 96, 96, 24, 24, 96, 96}
	},
	{	//corner
		{768, 768, 192, 192, 48, 48, 192, 192, 768, 768},
		{96, 96, 24, 24, 96, 96, 384, 384, 96, 96}
	}
};


//returns 1 if a given alien is alive
char isAlienAlive(int p_row, int p_col) {
	if(alive[p_col / 32 + (p_row / 24) * 11]) {
		return 1;
	}
	return 0;
}

//lets us know if we're between aliens to speed up processing time
char isSpaceRow(int row) {
	if(row % 24 > 15)
		return 1;
	return 0;
}
//returns the color that belongs in the given location
inline int getPixel(int alienType, int row, int col){
	if(isAlienAlive(row, col)) {	//is the alien alive?
		int alienCol = col % 32;
		int alienRow = row % 24;
		if(row < 24) {	//small alien
			if(alienType) {	//are the alien legs open or closed?
				if ((topOutAlien[alienRow] & (1<<(31-alienCol))))	//shift on integer to get individual bit
					return 0xFFFFFF;
				else return 0;
			} else {
				if ((topInAlien[alienRow] & (1<<(31-alienCol))))	//shift on integer to get individual bit
					return 0xFFFFFF;
				else return 0;
			}
		} else if(row < 72) {	//medium alien
			if(alienType) {
				if ((middleOutAlien[alienRow] & (1<<(31-alienCol))))	//shift on integer to get individual bit
					return 0xFFFFFF;
				else return 0;
			} else {
				if ((middleInAlien[alienRow] & (1<<(31-alienCol))))	//shift on integer to get individual bit
					return 0xFFFFFF;
				else return 0;
			}
		} else {	//big alien
			if(alienType) {
				if ((bottomOutAlien[alienRow] & (1<<(31-alienCol))))	//shift on integer to get individual bit
					return 0xFFFFFF;
				else return 0;
			} else {
				if ((bottomInAlien[alienRow] & (1<<(31-alienCol))))	//shift on integer to get individual bit
					return 0xFFFFFF;
				else return 0;
			}
		}
	}
	return 0;
}

void moveAliens() {
	alienMoveCount++;
	if(alienMoveCount % 36 == 0) {	//if on the edge, drop
		if(alienMoveCount % 72 == 0)	//switch directions
			alienDir = 1;
		else
			alienDir = -1;
		alienPosY+=12;
	} else {	//else, move 8 pixels
		alienPosX = alienPosX + alienDir * 8;
	}
}

void fireAlienMissile() {
	//all missiles shot, don't shoot another
	if(missileType[missileIndex] != -1)
		return;

	int column = rand() % 11;
	int row;
	int found = 0;
	//iterate through rows/columns to find a random available alien on the bottom row
	for(row = 4; row >= 0; row--) {
		if(alive[row * 11 + column]) {
			found = 1;
			missileX[missileIndex] = alienPosX + (column * 32) + 16 - 10;
			missileY[missileIndex] = alienPosY + (row * 24) + 24;
			missileType[missileIndex] = rand() % 4;// rand() % 4;
			missileNew[missileIndex] = 0;
			missileIndex++;
			if(missileIndex >= 4)
				missileIndex = 0;
			updateAlienMissiles(0);
			break;
		}
	}
	//if the random alien we tried to have shoot couldn't and everyone's not dead, try again
	if(!found && (alive[0] || alive[1]||alive[2]||alive[3]||alive[4]||alive[5]||alive[6]||alive[7]||alive[8]||alive[9]||alive[10]))
		fireAlienMissile();
}

void drawAlienMissiles() {
	int c;
	//iterate through the 4 missiles and draw those that exist
	for(c = 0; c < 4; c++) {
		if(missileType[c] != -1) {
			drawAlienMissile(c, state);
		}
	}
}



int getMissilePixel(int* missile, int x, int y) {
	//access integer array for each pixel
	if((missile[y] & (1<<(11-x))))	//shift on integer to get individual bit
		return 0xFFFFFF;
	return 0;
}

void drawAlienMissile(int mIndex, int state) {
	int cx, cy;
	int x = missileX[mIndex];
	int y = missileY[mIndex];
	char type = missileType[mIndex];
	cy = (missileNew[mIndex] < 1) ? y : (y - 10);	//if the missile is new, don't erase behind it
	for(; cy < y + 10; cy++) {
		for(cx = x; cx < x + 12; cx++) {
			if(cy < y) {
				framebuffer[cy * 640 + cx] = 0;	//erase
			} else {
				framebuffer[cy * 640 + cx] = getMissilePixel(missiles[missileType[mIndex]][state], cx - x, cy - y);	//draw
			}
		}
	}
}

void updateAlienMissiles(char changePosition) {
	int c;
	for(c = 0; c < 4; c++) {
		if(missileType[c] != -1) {	//if the missile exists, move it forward
			if(changePosition) {
				missileY[c]+=10;
				missileNew[c]++;
			}
			//if hit something
			//or if off screen
			if(missileY[c] > 490) {	//destroy missile once it exits screen
				missileType[c] = -1;
			}
		}
	}
}

void drawAlienBlock(int startRow, int startCol) {
	int curRow, curCol;
	int rowDiff;

	int bound1 = startRow + ALIEN_BLOCK_HEIGHT;

	int bound2 = startCol + ALIEN_BLOCK_WIDTH + 8;

	int bound3 = startCol + ALIEN_BLOCK_WIDTH - 1;

	int fb_row;
	for(curRow = startRow - 12; curRow < bound1; curRow++) {	//step through row
		rowDiff = curRow - startRow;
		fb_row = curRow*640;	//get position in framebuffer
		if(isSpaceRow(rowDiff)) {	//save time if we know the whole row is blank
			for(curCol = startCol - 8; curCol < bound2; curCol++) {	//step through column
				framebuffer[fb_row + curCol] = 0;
			}
			continue;
		}
		for(curCol = startCol - 8; curCol < bound2; curCol++) {

			if(curCol < startCol) {	//erase left
				framebuffer[fb_row + curCol] = 0;
			} else if(curCol > bound3) {	//erase right
				framebuffer[fb_row + curCol] = 0;
			} else if(curRow < startRow) {	//erase top
				framebuffer[fb_row + curCol] = 0;
			}
			else {	//get the specific pixel value for the alien block and assign framebuffer
				int now = getPixel(state, rowDiff, curCol - startCol);
				framebuffer[fb_row + curCol] = now;
				framebuffer[fb_row + (++curCol)] = now;
			}
		}
	}
}
