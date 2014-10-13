#include <xuartlite_l.h>
#include <xparameters.h>

#include "displaycontrol.h"
#include "globals.h"
#include "aliens.h"
#include "tank.h"
#include "bunkers.h"
#include "boss.h"

//static const short ALIEN_BLOCK_HEIGHT = 120;	//total alien height
//static const short ALIEN_BLOCK_WIDTH = 352;		//total un-altered alien width

unsigned int * framebuffer;

int state = 0;	//state of bullets

int moved = 1;	//track movement

void render() {
	//draw everything!
	state = !state;
	drawTankMissile();
	drawAlienMissiles();
	drawAlienMissiles();
	if(moved) {
		drawAlienBlock(alienPosY, alienPosX);
		drawBoss();
		moved = 0;
	}
	initializeBunkers();
	drawTank();
}

void lab3run(unsigned int * fb) {
	framebuffer = fb;
//	initializeBunkers();
//	moveAliens();
	initializeBoss();
	render();

	while(1) {
		Xuint8 s = XUartLite_RecvByte(XPAR_RS232_UART_1_BASEADDR);
		switch(s) {
			case 0x31:
				moveBoss();
				moved = 1;
				break;
			case 0x32:	//2 -- kill random alien
				xil_printf("Which would you like to kill? (2 digit value please)\r\n");
				Xuint8 a = XUartLite_RecvByte(XPAR_RS232_UART_1_BASEADDR);
				Xuint8 b = XUartLite_RecvByte(XPAR_RS232_UART_1_BASEADDR);
				alive[(a-0x30)*10+(b-0x30)] = 0;
				moved = 1;
				break;
			case 0x33:	//3 - alien shoot!
				fireAlienMissile();
				break;
			case 0x34:	//4 - go left!
				if(tankPosX > 2){
					tankPosX -= 2;
					drawTank();
				}
				break;
			case 0x35:	//5 - tank shoot!
				fireTankMissile();
				break;
			case 0x36:	//6 - go right1
				if(tankPosX < 600){
					tankPosX += 2;
					drawTank();
				}
				break;
			case 0x37:	//7 - erode random bunker!
				erodeBunker(rand()%4, getErodeValue());
				break;
			case 0x38:	//8 - move aliens!
				moveAliens();
				moved = 1;
				break;
			case 0x39:	//update all the bullets!
				updateAlienMissiles(1);
				updateTankMissile(1);
				break;
			default:
				break;
		}
		render();
	}
}
