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

void handleButtons() {
	if(centerButtonPressed()) {
		fireTankMissile();
	}
	if(leftButtonPressed()) {
		tankPosX -= 2;
	}
	if(rightButtonPressed()) {
		tankPosX += 2;
	}
}

void makeChange(int counter) {
	if(upButtonPressed()) {
		return;
	}
	if(counter % BULLET_UPDATE == 0) {
		updateAlienMissiles(1);

		if(tankMissile) {
			updateTankMissile(1);
			int alien = detectAlienHit(tankMissileX, tankMissileX+2, tankMissileY);
			if(alien) {
				tankMissile = 0;
				eraseTankMissile();
				tankMissile = 0;
				tankMissileY = 438;

			}
		}
		//detect hit, if it, moved = 1;
	}
	if(counter % BOSS_UPDATE == 0) {
		if(bossExists)
			moveBoss();
		else if(rand() % BOSS_APPEAR == 0) {
			initializeBoss();
		}
	}

	if(rand() % ALIEN_BULLET_FIRE_PROBABILITY == 0) {
		fireAlienMissile();
	}

	handleButtons();

	if(counter % ALIEN_BLOCK_UPDATE == 0) {
//		eraseAlienBlock();
//		int q = 0;
//		for(q = 0; q < 10000; q++)
//			xil_printf(".");
		moveAliens();
		alienBlockMoved = 1;
	}
	

	render();
}
