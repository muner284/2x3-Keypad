#include <stdbool.h>
#include <stdint.h>

#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"
#include "driverlib/debug.h"
#include "driverlib/fpu.h"

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "inc/hw_ints.h"

#include "keypad_interfacing.h"
#include "Lcd_prototypes.h"

volatile uint32_t value;
volatile uint32_t time;
volatile uint32_t Data;
volatile uint32_t millis = 0;

// Key Detect sequence
static unsigned int keys[KEYPAD_ROW_COUNT][KEYPAD_COL_COUNT]	= {
																												{ 1, 2, 3},
																												{ 4, 5, 6},
																											};


//*******************************************************************
// Functions for Key scan
//*******************************************************************
void key_function(uint8_t key)
{
	switch(key)
	{
		case 1:
						LCD_Write_Command(0x80);
						LCD_Send_Data("Row = 1  & Col = 1");
						break;
		case 2:
						LCD_Write_Command(0x80);
						LCD_Send_Data("Row = 1  & Col = 2");
						break;
		case 3:
						LCD_Write_Command(0x80);
						LCD_Send_Data("Row = 1  & Col = 3");
						break;
		case 4:
						LCD_Write_Command(0x80);
						LCD_Send_Data("Row = 2  & Col = 1");
						break;
		case 5:
						LCD_Write_Command(0x80);
						LCD_Send_Data("Row = 2  & Col = 2");
						break;
		case 6:
						LCD_Write_Command(0x80);
						LCD_Send_Data("Row = 2  & Col = 3");
						break;
		default:
						LCD_Write_Command(0x80);
						LCD_Send_Data(" no key pressed     ");
						break;
	}
}

int check_keypad_Col(unsigned char check_col)
{
	uint8_t col_pins;
	if (check_col == KEYPAD_ROW_0)
	{
	GPIOPinWrite(GPIO_PORTE_BASE, KEYPAD_ROW_0, check_col);						// Enable Row 1
	GPIOPinWrite(GPIO_PORTF_BASE, KEYPAD_ROW_1, 0);										// Disable Row 2
	
	col_pins = GPIOPinRead(GPIO_PORTF_BASE, KEYPAD_COLS);							// Read in which Column key is pressed
	Wait(1);																													// Delay to avoid debouncing
	if (col_pins & KEYPAD_COL_1)	
		return 0;
	else if (col_pins & KEYPAD_COL_2)
		return 1;
	else if (col_pins & KEYPAD_COL_3)
		return 2;
	}
	else if (check_col == KEYPAD_ROW_1)
	{
		GPIOPinWrite(GPIO_PORTF_BASE, KEYPAD_ROW_1, check_col);					// Enable Row 2
		GPIOPinWrite(GPIO_PORTE_BASE, KEYPAD_ROW_0, 0);									// Disable Row 1

		col_pins = GPIOPinRead(GPIO_PORTF_BASE, KEYPAD_COLS);						// Read in which Column key is pressed
		Wait(1);																												// Delay to avoid debouncing
		if (col_pins & KEYPAD_COL_1)
			return 0;
		else if (col_pins & KEYPAD_COL_2)
			return 1;
		else if (col_pins & KEYPAD_COL_3)
			return 2;
	}
		GPIOPinWrite(GPIO_PORTE_BASE, KEYPAD_ROW_0, 0);									// Disable Row 1
		GPIOPinWrite(GPIO_PORTF_BASE, KEYPAD_ROW_1, 0);									// Disable Row 2
	
	return 0xFF;																											// Return No key pressed
}

int check_keypad(void)
{
	uint8_t row, col;
	
	col = check_keypad_Col(KEYPAD_ROW_0);
	if (col != 0xFF)
	{
		row = 0;
		return keys[row][col];
	}
	col = check_keypad_Col(KEYPAD_ROW_1);
	if (col != 0xFF)
	{
		row = 1;
		return keys[row][col];
	}
	return 0;
}
//*******************************************************
//
// Function for Lcd Initialization
//
//*******************************************************
void LCD_Initialization()
{
	//
	// Clear Screen  
	//
	LCD_Write_Command(0x01);
	Wait(1);
	//
	// to reset  
	//
	LCD_Write_Command(0x02);
	Wait(1);
	//
	// 2 Lines and 5x7 matrix
	//
	LCD_Write_Command(0x28);
	Wait(1);
	//
	// Display ON, Cursor OFF
	//
	LCD_Write_Command(0x0C);
	Wait(1);
}

//*******************************************************
//
// Function for writing command 
//
//*******************************************************
void LCD_Write_Command(unsigned char value)
{
	volatile unsigned char temp;

	Data = value & 0xf0;
	Data >>= 4;
	// Make Pin_0 i.e RS = 0, for Command register
	RS_LW;
	Wait(1);
		
	// Write value to higher 4-bits of Data Lines
		Data_Line(Data);
	// Make Pin_1 i.e EN = 1
	EN_HI;
		
	// Make Pin_1 i.e EN = 0, for recieving next character
	EN_LW;
	Wait(1);

	
	// Write value to lower 4-bits of Data Lines
	Data = value & 0x0f;
		Data_Line(Data);
	Wait(1);
	// Make Pin_1 i.e EN = 1
	EN_HI;
	// Make Pin_1 i.e EN = 0, for recieving next character
	EN_LW;
	Wait(1);
}

//******************************************************
//
// Function for Writing Data on to the LCD
//
//******************************************************
void LCD_Write_Data(unsigned char Dat)
{
	volatile unsigned char temp;
	Data = Dat & 0xf0;
	Data >>= 4;
	// Make Pin_0 i.e RS = 1, for Data register
	RS_HI;
	Wait(1);
	
	// Write value to higher 4-bits of the data 
	Data_Line(Data);
	Wait(1);
	// Make Pin_1 i.e EN = 1
	EN_HI;
	// Make Pin_1 i.e EN = 0, for recieving next character
	EN_LW;
	Wait(1);


	// Write value to lower 4-bits of Data Lines
	Data = Dat & 0x0f;
	Data_Line(Data);
	Wait(1);
	// Make Pin_1 i.e EN = 1
	EN_HI;
	// Make Pin_1 i.e EN = 0, for recieving next character
	EN_LW;
	Wait(1);
}

//********************************************************
//
// Function for Sending data on to LCD
//
//********************************************************
void LCD_Send_Data(unsigned char *s)
{
	unsigned int i = 0;
	while (s[i] != '\0')
	{
		// Send Characters to be displayed on LCD
		LCD_Write_Data(s[i]);
		// Decrement the length of the Character
		i++;
	}
}
//**********************************************************************
// Interrupt handler for the timer
//**********************************************************************
void Timer0A_Handler(void)
{
  uint32_t status = 0;
	
	// Check the Status of the Timer
	status = TimerIntStatus(TIMER0_BASE,true);
	// Clear the Timer Interrupt
  TimerIntClear(TIMER0_BASE, status);
  
	// Increment the count millis
	millis++;
	// Enable System Interrupts
	IntMasterEnable();
}

//**********************************************************************
// Timer setup
//**********************************************************************
void TimerBegin(){
  
  //********************************************************************
	// Enable Timer 0 Peripheral and wait for the module to be ready
	//********************************************************************
  SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
  SysCtlDelay(3);
	
  //********************************************************************
	// Configure the timer as periodic, by omission it's in count down mode.
	// It counts from the load value to 0 and then resets back to the load value.
	// REMEMBER: You need to configure the timer before setting the load and match
	//********************************************************************
	IntMasterEnable();

	// Configure the Timer as Periodic
  TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
	
  //We set the load value so the timer interrupts each .2ms
  TimerLoadSet(TIMER0_BASE, TIMER_A, SysCtlClockGet() / 500);
	
	// Enable Timer interrupt
  IntEnable(INT_TIMER0A);
	
	// Register the function Handler in timer Register
	TimerIntRegister(TIMER0_BASE, TIMER_A, &Timer0A_Handler);
	  
  //*********************************************************************
	// Enable the timeout interrupt. In count down mode it's when the timer 
	// reaches 0 and resets back to load. In count up mode it's when the 
	// timer reaches load and resets back to 0.
	//*********************************************************************
   TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
		
	 TimerEnable(TIMER0_BASE, TIMER_A);
}

//***********************************************************************
// This is the delay function.
//***********************************************************************
void Wait (unsigned int tempo) {
  volatile uint32_t temp = millis;
  while ( (millis-temp) < tempo);
}
