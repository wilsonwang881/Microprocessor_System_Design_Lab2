#include <xparameters.h>
#include <xgpio.h>
#include <xstatus.h>
#include <xil_printf.h>

/* Definitions */
/* GPIO device that LEDs are connected to */
#define GPIO_DEVICE_ID_LED XPAR_LED_DEVICE_ID   
/* GPIO device that switches and buttons are connected to */
#define GPIO_DEVICE_ID_SW_BUTTONS XPAR_SW_BUTTONS_DEVICE_ID  
#define WAIT_VAL 0x1000000

// declare the delay function
int delay(void);

// declare the print button function
int printSWStatus(int flag);

// declare the count value limiting function
int limitCount(int count, int op);

int main() {

	// variable that hold the count value
    int count;

    // converted value for writing LED on/off
    int count_masked;

    // led address used by the GPIO write function
    XGpio leds;

    // switch and button addresses used by the GPIO read function
    XGpio sw_buttons;

    // initialization status holder
    int status_led;
    int status_sw_buttons;

    // flag for pressing buttons
    int btn0 = 0;
    int btn1 = 1;

    // variable to hold the read values returned from GPIO read
    u32 GPIORead;

    // store the previous button and switch status
    u32 GPIOReadHistory;

    // return the status of initializing GPIO
    status_led = XGpio_Initialize(&leds, GPIO_DEVICE_ID_LED);
    status_sw_buttons = XGpio_Initialize(&sw_buttons, GPIO_DEVICE_ID_SW_BUTTONS);
    XGpio_SetDataDirection(&leds, 1, 0x00);
    XGpio_SetDataDirection(&sw_buttons, 1, 0x01);

    // check initialization status
    if(status_led != XST_SUCCESS || status_sw_buttons != XST_SUCCESS) {
        xil_printf("Initialization failed");    
    }

    // initialize the counter value holder
    count = 0;

    // do the counting
    // in an infinite loop
    while(1) {

    	// get the GPIO read values
    	GPIORead = XGpio_DiscreteRead(&sw_buttons, 1);

    	// btn0 = 0x10 => increment
    	if ((GPIORead >> 4) & 0x1){
    		xil_printf("btn0 pressed, increment counter \n\r");
    		count = limitCount(count, 1);
    		xil_printf("count %d \n\r", count);
    		// console log the current LED status
			xil_printf("LEDs = 0x%x \n\r", count_masked);
    	}
    	// btn1 = 0x20 => decrement
    	else if ((GPIORead >> 5) & 0x1){
    		xil_printf("btn1 pressed, decrement counter \n\r");
    		count = limitCount(count, 0);
    		xil_printf("count %d \n\r", count);
    		// console log the current LED status
			xil_printf("LEDs = 0x%x \n\r", count_masked);
    	}
    	// btn2 = 0x40 => display switch status
    	else if ((GPIORead >> 6) & 0x1){
    		/**
    		sw0 = 0x1
			sw1 = 0x2
			sw3 = 0x4
			sw4 = 0x8
    		 */
    		xil_printf("btn2 pressed, display switch status \n\r");
    		xil_printf("sw0: ");
    		printSWStatus(GPIORead & 0x1);
    		xil_printf("sw1: ");
			printSWStatus((GPIORead & 0x2) >> 1);
			xil_printf("sw2: ");
			printSWStatus((GPIORead & 0x4) >> 2);
			xil_printf("sw3: ");
			printSWStatus((GPIORead & 0x8) >> 3);
			// console log the current LED status
			xil_printf("LEDs = 0x%x \n\r", count_masked);
    	}
    	// btn3 = 0x80 => display count on LED
    	else if ((GPIORead >> 7) & 0x1){
    		xil_printf("btn3 pressed, display counter on LED \n\r");

    		// get the masked count value using a bitwise AND operation
    		count_masked = count & 0xF;

    		// write to the LDE on/off status
			XGpio_DiscreteWrite(&leds, 1, count_masked);

			// console log the current LED status
			xil_printf("LEDs = 0x%x \n\r", count_masked);
    	}

        // call the delay function so that we can observe the counting process
        delay();
    }
    return (0);
}

int delay(void) {
    volatile int delay_count = 0;

    // increment the counter in a loop
    while(delay_count < WAIT_VAL)
        delay_count++;
    return(0);
}

int printSWStatus(int flag) {
	if (flag == 0x1){
		xil_printf("on \n\r");
	}
	else{
		xil_printf("off \n\r");
	}
	return(0);
}

int limitCount(int count, int op) {
	if (count == 0 && op == 0) {
		return 15;
	}
	else if(count >= 0 && count < 15 && op == 1) {
		count++;
		return count;
	}
	else if(count == 15 && op == 1) {
		return 0;
	}
	else if(op == 0) {
		count--;
		return count;
	}
	else if(op == 1) {
		count++;
		return count;
	}
}

