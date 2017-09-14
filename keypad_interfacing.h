/*
#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
*/

#define	KEYPAD_ROW_0			GPIO_PIN_7
#define KEYPAD_ROW_1			GPIO_PIN_0
#define KEYPAD_ROWS				(KEYPAD_ROW_0 | KEYPAD_ROW_1)

#define KEYPAD_COL_1			GPIO_PIN_1
#define KEYPAD_COL_2			GPIO_PIN_2
#define KEYPAD_COL_3			(GPIO_PIN_3)
#define KEYPAD_COLS				(KEYPAD_COL_1 | KEYPAD_COL_2 | KEYPAD_COL_3)

#define KEYPAD_ROW_COUNT 2
#define KEYPAD_COL_COUNT 3

void key_function(uint8_t key);
int check_keypad(void);
