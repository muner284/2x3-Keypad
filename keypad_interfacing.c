#include <stdbool.h>
#include <stdint.h>

#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"

#include "keypad_interfacing.h"
#include "Lcd_prototypes.h"

//*******************************************************************
//
// 	Main Function for Keypad Interfacing
//
//*******************************************************************
int main()
{
	uint8_t key;
	
	//*****************************************************************
	//Configure the device to run from the PLL using 8 Mhz crystal 
	// as an input	
	//*****************************************************************
	SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_8MHZ | SYSCTL_OSC_MAIN);
	TimerBegin();
	
	//*******************************************************************
	// Enable The GPIO peripherals which you want to use 
	//*******************************************************************
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOG);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOH);
	
	//*******************************************************************
	// Wait for the GPIO module to be ready
	//*******************************************************************
	while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOG));
	while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOH));
	while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF));
	while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOE));
	
	//*******************************************************************
	// Initialize the GPIO pin configuration
	// Initialize the pins for LCD
	// Configure the Port Pins as Output in my case
	//*******************************************************************
	GPIOPinTypeGPIOOutput(GPIO_PORTG_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5);
	GPIOPinTypeGPIOOutput(GPIO_PORTH_BASE, GPIO_PIN_7);
	
	GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_7);
	GPIOPadConfigSet(GPIO_PORTE_BASE, GPIO_PIN_7, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPD);
	
	//*******************************************************************
	// The Pin 0 of PORTF is NMI locked, unlock it before using
	//*******************************************************************
	HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
	HWREG(GPIO_PORTF_BASE + GPIO_O_CR) |= 0x01;
	//*******************************************************************
	// Configure the port pins, both row and column as PUll-DOWN 
	// in my case
	//*******************************************************************
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_0);
	GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_0, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPD);
	
	GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
	GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPD);
	
	//*******************************************************************
	// Initialize the LCD
	//*******************************************************************
	LCD_Initialization();
	
	while (1)
	{
			key = check_keypad();
		if (key != 0)
		{
			key_function(key);
		}
		else 
		{
			LCD_Write_Command(0x80);
			LCD_Send_Data("Key not detected    ");
		}	
		Wait(7);
	}
}
