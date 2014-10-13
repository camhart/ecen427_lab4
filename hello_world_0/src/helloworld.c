#include "xgpio.h"          // Provides access to PB GPIO driver.
#include <stdio.h>          // xil_printf and so forth.
#include "platform.h"       // Enables caching and other system stuff.
#include "mb_interface.h"   // provides the microblaze interrupt enables, etc.
#include "xintc_l.h"        // Provides handy macros for the interrupt controller.

extern void xil_printf(const char *ctrl1, ...);

XGpio gpLED;  // This is a handle for the LED GPIO block.
XGpio gpPB;   // This is a handle for the push-button GPIO block.

int pb_debounce;
int timer;
int half_second_timer;
int seconds;
int minutes;
int hours;

char setting_time = 0;
int currentButtonState;
int takeOne = 4;
int takeTwo = 2;
int takeThree;
int takeFour;
int times_high;

// This is invoked each time there is a change in the button state (result of a push or a bounce).
void pb_interrupt_handler() {
  // Clear the GPIO interrupt.
  XGpio_InterruptGlobalDisable(&gpPB);                // Turn off all PB interrupts for now.
  currentButtonState = XGpio_DiscreteRead(&gpPB, 1);  // Get the current state of the buttons.

  if(times_high == 4) {	//have we debounced?
	  times_high++;

	  if (currentButtonState > 0) { // button being pressed

		  setting_time = 1;	//pause timer

		  int time_change = 0; //variable to represent the change in time when up/down button set
		  	  	  	  	  	   //0 means no up down button set, 1 means up button set, -1 means down button set

		  if(currentButtonState & 16) {	//up button
			  time_change = 1;
		  }
		  else if (currentButtonState & 4) { //down button
			  time_change = -1;
		  } else {
			  half_second_timer = 0;	//reset half second timer
		  }

		  if(currentButtonState & 2) { //seconds button
			  seconds+=time_change;
		  }

		  if(currentButtonState & 1) { //minutes
			  minutes+=time_change;
		  }

		  if(currentButtonState & 8) { //hours
			  hours+=time_change;
		  }

		  //seconds cleanup
		  if(seconds >= 60) {	//if seconds >= 60, set to 0, increment minutes
			  seconds = 0;
			  minutes++;
		  } else if(seconds < 0) {  //if seconds < 0, set to 59, decrement minutes
			  seconds = 59;
			  minutes--;
		  }

		  if(minutes >= 60) {  //if minutes >= 60 set to 0, increment hours
			  minutes = 0;
			  hours++;
		  } else if(minutes < 0) {	//if minutes < 0 set to 59, decrement hours
			  minutes = 59;
			  hours--;
		  }

		  if(hours >= 24) {	//if hours >= 24 set to 0
			  hours = 0;
		  } else if(hours < 0) { //if hours < 0 set to 23
			  hours = 23;
		  }

		  //print the new time values
		  //xil_printf("\r%02d:%02d:%02d", hours, minutes, seconds);
	  } else {
		  setting_time = 0;	//unpause timer
	  }
  }
  XGpio_InterruptClear(&gpPB, 0xFFFFFFFF);            // Ack the PB interrupt.
  XGpio_InterruptGlobalEnable(&gpPB);                 // Re-enable PB interrupts.
}

// This is invoked in response to a timer interrupt.
// It does 2 things: 1) debounce switches, and 2) advances the time.
void timer_interrupt_handler() {
	pb_debounce++;

	//increment history
	takeFour = takeThree;
	takeThree = takeTwo;
	takeTwo = takeOne;
	takeOne = currentButtonState;

	//if we haven't had any changes in value for 4 cycles after a change, set the value
	if(takeOne == takeTwo && takeTwo == takeThree && takeThree == takeFour && times_high < 4)
		times_high = 4;
	else if(takeOne == takeTwo && takeTwo == takeThree && takeThree == takeFour){
		//do nothing, we're stable and nothing has changed
	}
	else{	//something has changed in the last 4 cycles
		times_high = 0;
	}
	if(times_high == 4){
		pb_interrupt_handler();	//call the actual routine
		xil_printf("\r%02d:%02d:%02d", hours, minutes, seconds);	//print new time
	}

	if (setting_time == 0) {	//setting time, pause timer
		timer++;
		half_second_timer = 0;	//reset half_second_timer (phase 2)
	}
	else {
		half_second_timer++;	//setting time so we need to increment half_second_timer
		if(half_second_timer > 100){	//if it's waited 1 second
			if(half_second_timer % 50 == 0){	//if it's been a half second call pb_interrupt_handler to change the time
				times_high = 4;
				pb_interrupt_handler(); // call pb_interrupt_handler to change the time
				xil_printf("\r%02d:%02d:%02d", hours, minutes, seconds);	//print new time
			}
		}
	}
	if(timer == 100){	//1 second has passed, change time
		timer = 0;
		seconds++;
		if(seconds >= 60) {
			seconds = 0;
			minutes++;
		}
		if(minutes >= 60) {
			minutes = 0;
			hours++;
		}
		if(hours >= 24) {
			hours = 0;
		}
		//print new time
		xil_printf("\r%02d:%02d:%02d", hours, minutes, seconds);
	}
}

// Main interrupt handler, queries the interrupt controller to see what peripheral
// fired the interrupt and then dispatches the corresponding interrupt handler.
// This routine acks the interrupt at the controller level but the peripheral
// interrupt must be ack'd by the dispatched interrupt handler.
void interrupt_handler_dispatcher(void* ptr) {
	int intc_status = XIntc_GetIntrStatus(XPAR_INTC_0_BASEADDR);
	// Check the FIT interrupt first.
	if (intc_status & XPAR_FIT_TIMER_0_INTERRUPT_MASK){
		XIntc_AckIntr(XPAR_INTC_0_BASEADDR, XPAR_FIT_TIMER_0_INTERRUPT_MASK);
		timer_interrupt_handler();
	}
	// Check the push buttons.
	if (intc_status & XPAR_PUSH_BUTTONS_5BITS_IP2INTC_IRPT_MASK){
		XIntc_AckIntr(XPAR_INTC_0_BASEADDR, XPAR_PUSH_BUTTONS_5BITS_IP2INTC_IRPT_MASK);
		pb_interrupt_handler();
	}
}

int main (void) {
    init_platform();
    // Initialize the GPIO peripherals.
    int success;
    hours = 0;
    seconds = 0;
    minutes = 0;

    success = XGpio_Initialize(&gpPB, XPAR_PUSH_BUTTONS_5BITS_DEVICE_ID);
    // Set the push button peripheral to be inputs.
    XGpio_SetDataDirection(&gpPB, 1, 0x0000001F);
    // Enable the global GPIO interrupt for push buttons.
    XGpio_InterruptGlobalEnable(&gpPB);
    // Enable all interrupts in the push button peripheral.
    XGpio_InterruptEnable(&gpPB, 0xFFFFFFFF);

    microblaze_register_handler(interrupt_handler_dispatcher, NULL);
    XIntc_EnableIntr(XPAR_INTC_0_BASEADDR,
    		(XPAR_FIT_TIMER_0_INTERRUPT_MASK | XPAR_PUSH_BUTTONS_5BITS_IP2INTC_IRPT_MASK));
    XIntc_MasterEnable(XPAR_INTC_0_BASEADDR);
    microblaze_enable_interrupts();

    while(1);  // Program never ends.

    cleanup_platform();

    return 0;
}
