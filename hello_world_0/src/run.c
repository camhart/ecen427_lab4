#include <stdio.h>
//#include "xtmrctr_l.h"
#include "xtmrctr.h"
#include "xtmrctr_l.h"
#include "xil_types.h"
#include "xparameters.h"
//#include "xtmrctr_l.h"		//needed for the timer
#include "xil_macroback.h"	//needed for the timer
#include "xgpio.h"          // Provides access to PB GPIO driver.
#include "mb_interface.h"   // provides the microblaze interrupt enables, etc.
#include "xintc_l.h"        // Provides handy macros for the interrupt controller.
#include "platform.h"
#include "xparameters.h"
#include "xaxivdma.h"
#include "xio.h"
#include "unistd.h"
#include "displayControl.h"
#include "stateControl.h"
#include "run.h"
//
//#define DEBUG
//void print(char *str);

#define FRAME_BUFFER_0_ADDR 0xC1000000  // Starting location in DDR where we will store the images that we display.

XGpio gpLED;  // This is a handle for the LED GPIO block.
XGpio gpPB;   // This is a handle for the push-button GPIO block.

// This is invoked in response to a timer interrupt.
// It does 2 things: 1) debounce switches, and 2) advances the time.
int counter = 0;

XTmrCtr xps_timer_0;
Xuint32 startTime, endTime, calibrationTime, runTime;
XStatus status;

unsigned long long longest = 0;
unsigned long long shortest = 0xFFFFFFFF;
unsigned long long average = 0;
unsigned int acount = 0;
unsigned long long sum = 0;
unsigned int ccount = 0;

void timer_interrupt_handler() {
    status = XTmrCtr_Initialize(&xps_timer_0, XPAR_AXI_TIMER_0_DEVICE_ID);
    if (status != XST_SUCCESS)
    {
		xil_printf("Issue setting up timer");
		return;
    }
	startTime = XTmrCtr_GetValue(&xps_timer_0, XPAR_AXI_TIMER_0_DEVICE_ID);
	XTmrCtr_Start(&xps_timer_0, 0);

	counter++;
	makeChange(counter);
	//XTC_DEVICE_TIMER_COUNT
	XTmrCtr_Stop(&xps_timer_0, 0);
	endTime = XTmrCtr_GetValue(&xps_timer_0, XPAR_AXI_TIMER_0_DEVICE_ID);
	runTime = endTime - startTime - calibrationTime;
	ccount++;
	sum += runTime;
	if(ccount % 1000 == 0) {
//		acount++;
//		average = average * (acount - 1 / acount) + (sum / ccount) * (1 / acount);
		xil_printf("average %x\r\n", sum / ccount);
//		sum = 0;
//		ccount = 0;
	}
	if(runTime > longest) {
		longest = runTime;
		xil_printf("longest %x\r\n", runTime);
	}
	else if(runTime < shortest) {
		shortest = runTime;
		xil_printf("shortest %x\r\n", runTime);
	}
}

int currentButtonState;

// This is invoked each time there is a change in the button state (result of a push or a bounce).
void pb_interrupt_handler() {
  // Clear the GPIO interrupt.
  XGpio_InterruptGlobalDisable(&gpPB);                // Turn off all PB interrupts for now.
  currentButtonState = XGpio_DiscreteRead(&gpPB, 1);  // Get the current state of the buttons.
  // You need to do something here.

  XGpio_InterruptClear(&gpPB, 0xFFFFFFFF);            // Ack the PB interrupt.
  XGpio_InterruptGlobalEnable(&gpPB);                 // Re-enable PB interrupts.
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

int main()
{
	init_platform();                   // Necessary for all programs.

	int Status;                        // Keep track of success/failure of system function calls.
	XAxiVdma videoDMAController;
	// There are 3 steps to initializing the vdma driver and IP.
	// Step 1: lookup the memory structure that is used to access the vdma driver.
    XAxiVdma_Config * VideoDMAConfig = XAxiVdma_LookupConfig(XPAR_AXI_VDMA_0_DEVICE_ID);
    // Step 2: Initialize the memory structure and the hardware.
    if(XST_FAILURE == XAxiVdma_CfgInitialize(&videoDMAController, VideoDMAConfig,	XPAR_AXI_VDMA_0_BASEADDR)) {
    	xil_printf("VideoDMA Did not initialize.\r\n");
    }
    // Step 3: (optional) set the frame store number.
    if(XST_FAILURE ==  XAxiVdma_SetFrmStore(&videoDMAController, 2, XAXIVDMA_READ)) {
    	xil_printf("Set Frame Store Failed.");
    }
    // Initialization is complete at this point.

    // Setup the frame counter. We want two read frames. We don't need any write frames but the
    // function generates an error if you set the write frame count to 0. We set it to 2
    // but ignore it because we don't need a write channel at all.
    XAxiVdma_FrameCounter myFrameConfig;
    myFrameConfig.ReadFrameCount = 2;
    myFrameConfig.ReadDelayTimerCount = 10;
    myFrameConfig.WriteFrameCount =2;
    myFrameConfig.WriteDelayTimerCount = 10;
    Status = XAxiVdma_SetFrameCounter(&videoDMAController, &myFrameConfig);
    if (Status != XST_SUCCESS) {
	   xil_printf("Set frame counter failed %d\r\n", Status);
	   if(Status == XST_VDMA_MISMATCH_ERROR)
		   xil_printf("DMA Mismatch Error\r\n");
    }
    // Now we tell the driver about the geometry of our frame buffer and a few other things.
    // Our image is 480 x 640.
    XAxiVdma_DmaSetup myFrameBuffer;
    myFrameBuffer.VertSizeInput = 480;    // 480 vertical pixels.
    myFrameBuffer.HoriSizeInput = 640*4;  // 640 horizontal (32-bit pixels).
    myFrameBuffer.Stride = 640*4;         // Dont' worry about the rest of the values.
    myFrameBuffer.FrameDelay = 0;
    myFrameBuffer.EnableCircularBuf=1;
    myFrameBuffer.EnableSync = 0;
    myFrameBuffer.PointNum = 0;
    myFrameBuffer.EnableFrameCounter = 0;
    myFrameBuffer.FixedFrameStoreAddr = 0;
    if(XST_FAILURE == XAxiVdma_DmaConfig(&videoDMAController, XAXIVDMA_READ, &myFrameBuffer)) {
    	xil_printf("DMA Config Failed\r\n");
     }
    // We need to give the frame buffer pointers to the memory that it will use. This memory
    // is where you will write your video data. The vdma IP/driver then streams it to the HDMI
    // IP.
     myFrameBuffer.FrameStoreStartAddr[0] = FRAME_BUFFER_0_ADDR;
     myFrameBuffer.FrameStoreStartAddr[1] = FRAME_BUFFER_0_ADDR + 4*640*480;

     if(XST_FAILURE == XAxiVdma_DmaSetBufferAddr(&videoDMAController, XAXIVDMA_READ,
    		               myFrameBuffer.FrameStoreStartAddr)) {
    	 xil_printf("DMA Set Address Failed Failed\r\n");
     }
     // Print a sanity message if you get this far.
     xil_printf("Woohoo! I made it through initialization.\n\r");
     // Now, let's get ready to start displaying some stuff on the screen.
     // The variables framePointer and framePointer1 are just pointers to the base address
     // of frame 0 and frame 1.
     unsigned int * framePointer0 = (unsigned int *) FRAME_BUFFER_0_ADDR;

     int row=0, col=0;
	 for( row=0; row<480; row++) {
	 	for(col=0; col<640; col++) {
			framePointer0[row*640 + col] = 0;
		}
	 }

     // This tells the HDMI controller the resolution of your display (there must be a better way to do this).
     XIo_Out32(XPAR_AXI_HDMI_0_BASEADDR, 640*480);

     // Start the DMA for the read channel only.
     if(XST_FAILURE == XAxiVdma_DmaStart(&videoDMAController, XAXIVDMA_READ)){
    	 xil_printf("DMA START FAILED\r\n");
     }
     int frameIndex = 0;
     // We have two frames, let's park on frame 0. Use frameIndex to index them.
     // Note that you have to start the DMA process before parking on a frame.
     if (XST_FAILURE == XAxiVdma_StartParking(&videoDMAController, frameIndex,  XAXIVDMA_READ)) {
    	 xil_printf("vdma parking failed\n\r");
     }

     // Initialize the GPIO peripherals.
     int success;
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


     lab4init(framePointer0);

     render();
//     XTmrCtr_Enable( , XPAR_AXI_TIMER_0_DEVICE_ID);
//     XTmrCtr_Enable(XPAR_AXI_TIMER_0_BASEADDR, XPAR_AXI_TIMER_0_DEVICE_ID);


     status = XTmrCtr_Initialize(&xps_timer_0, XPAR_AXI_TIMER_0_DEVICE_ID);
     if (Status != XST_SUCCESS)
     {
    	 xil_printf("Issue setting up timer");
    	 return 0;
     }
//
//// 	startTime = XTmrCtr_GetValue(&xps_timer_0, XPAR_AXI_TIMER_0_DEVICE_ID);
//// 	XTmrCtr_Start(&xps_timer_0, 0);
//// 	XTmrCtr_Stop(&xps_timer_0, 0);
//// 	endTime = XTmrCtr_GetValue(&xps_timer_0, XPAR_AXI_TIMER_0_DEVICE_ID);
//// 	calibrationTime = endTime - startTime;

  	startTime = XTmrCtr_GetValue(&xps_timer_0, XPAR_AXI_TIMER_0_DEVICE_ID);
  	XTmrCtr_Start(&xps_timer_0, 0);
  	XTmrCtr_Stop(&xps_timer_0, 0);
  	endTime = XTmrCtr_GetValue(&xps_timer_0, XPAR_AXI_TIMER_0_DEVICE_ID);
  	calibrationTime = endTime - startTime;

     microblaze_enable_interrupts();

     while (1);

     cleanup_platform();

    return 0;
}
