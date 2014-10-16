#include <xuartlite_l.h>
#include <xparameters.h>

#include "displaycontrol.h"
#include "globals.h"
#include "aliens.h"
#include "tank.h"
#include "bunkers.h"
#include "boss.h"
#include "stateControl.h"
#include "run.h"
#include "boss.h"
#include "scoreboard.h"

int bossHit = 0; 	//just a place holder to wait after boss is hit for one 200ms period
int tankHit = 0; 	//just a place holder to wait after tank is hit for one 200ms period
int tankLives = 3;
static int alternate = 1;
int gameOver = 0;

int upButtonPressed() {
	return (currentButtonState & 16);
}

int downButtonPressed() {
	return (currentButtonState & 4);
}

int leftButtonPressed() {
	return (currentButtonState & 8);
}

int rightButtonPressed() {
	return (currentButtonState & 2);
}

int centerButtonPressed() {
	return (currentButtonState & 1);
}

int lastTankShotAt = 0;

void handleButtons(int counter) {
	if(centerButtonPressed() && tankAlive) {
		lastTankShotAt = counter;
		fireTankMissile();
	}
	if(leftButtonPressed() && tankAlive) {
		if(tankPosX > 2)
			tankPosX -= 2;
	}
	if(rightButtonPressed() && tankAlive) {
		if(tankPosX < 605)
			tankPosX += 2;
	}
}

void resetGame() {
	int c;
	for(c = 0; c < 55; c++)
		alive[c] = 1;
	bossHit = 0; 	//just a place holder to wait after boss is hit for one 200ms period
	tankHit = 0; 	//just a place holder to wait after tank is hit for one 200ms period
	tankLives = 3;
	alternate = 1;
	gameOver = 0;
	bossPosX = 2; //boss starting point
	bossTopRow = 20;	//boss top Y position
	bossBottomRow = 34;	//boss bottom Y position
	bossDir = 1;	 //right (direction boss is moving)
	bossExists = 0;
	int d;
	for(c = 0; c < 4; c++) {
		for(d = 0; d < 16; d++) {
			bunkerState[c][d] = 0;
			if(d == 14 || d == 13) {
				bunkerState[c][d] = 5;
			}
		}
	}
	state = 0;	//state of bullets
	alienState = 0;
	alienBlockMoved = 1;	//track movement
	score = 0;

	tankPosX = TANK_START_X;	//tank location
	tankPosY = 448;

	tankMissile = 0;	//is there a tank missile?
	tankMissileX = -1;	//location
	tankMissileY = 438;	//location

	tankAlive = 1;

	initializeBoss();
	initializeBunkers();
	initializeScore();

}

void makeChange(int counter) {
	if(upButtonPressed()) {
		return;
	}

//	if(downButtonPressed()) {
//		resetGame();
//	}

	if(gameOver){
		paintScreenRed();
		return;
	}

	if(counter % BULLET_UPDATE == 0) {
		updateAlienMissiles(1);
		int c;
		for(c = 0; c < 4; c++) {
			if(missileType[c] != -1) {
				int hit = detectBunkerHit(missileX[c], missileX[c] + 10, missileY[c]-10, missileY[c]);
				if(hit) {
					eraseAlienMissile(c);
				}
				hit = detectTankHit(missileX[c], missileX[c] + 10, missileY[c]-10, missileY[c]);
				if(hit) {
					tankAlive = 0;
					eraseAlienMissile(c);
				}
			}
		}
		//detect hit, if it, moved = 1;
	}

	if(counter % TANK_BULLET_UPDATE == 0) {
		if(tankMissile) {
			updateTankMissile(1);
			int alien = detectAlienHit(tankMissileX, tankMissileX+2, tankMissileY);
			if(alien) {
				tankMissile = 0;
				eraseTankMissile();
				tankMissileY = 438;
				alienBlockMoved = 1; //remove artifacts
//				killBoss();

			}
			int hit = detectBunkerHit(tankMissileX, tankMissileX+2, tankMissileY - 10, tankMissileY);
			if(hit) {
				eraseTankMissile();
				tankMissile = 0;
				tankMissileY = 438;
			}
			if(bossExists) {
				hit = detectBossHit(tankMissileX, tankMissileX+2, tankMissileY - 10, tankMissileY);
				if(hit) {
					tankMissile = 0;
					eraseTankMissile();
					tankMissileY = 438;
					killBoss();
				}
			}
		}
	}

	if(counter % BOSS_UPDATE == 0) {
		if(bossExists)
			moveBoss();
		else if(rand() % BOSS_APPEAR == 0) {
			initializeBoss();
		}
	}
	if(counter % EXPLOSION_UPDATE == 0) {
		if(!tankAlive){
			if(alternate = !alternate){
				drawTankExplosionB();
			}
			else{
				drawTankExplosionA();
			}
		}
	}

	if(rand() % ALIEN_BULLET_FIRE_PROBABILITY == 0) {
		fireAlienMissile();
	}

	handleButtons(counter);

	if(counter % TANK_DEATH_UPDATE == 0) {
		if(!tankAlive){
			if(tankHit){
				if(tankLives > 0){
					eraseTankExplosion();
					tankAlive = 1;
					tankPosX = TANK_START_X;
				}
			}
			tankHit = 1;
		}
		else{
			tankHit = 0;
		}
	}

	if(counter % BOSS_SCORE_UPDATE == 0) {
		if(!bossExists){
			if(bossHit){
				clearNumbers();
			}
			bossHit = 1;
		}
		else{
			bossHit = 0;
		}
	}

	if(counter % ALIEN_BLOCK_UPDATE == 0) {
//		eraseAlienBlock();
//		int q = 0;
//		for(q = 0; q < 10000; q++)
//			xil_printf(".");
		moveAliens();
		alienBlockMoved = 1;
		if(alienPosY + ALIEN_BLOCK_HEIGHT >= (tankPosY - 5)) {
			gameOver = 1;
		}
	}
	

	render();
}
