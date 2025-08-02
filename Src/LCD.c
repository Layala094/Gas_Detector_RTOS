// Using PC4 - PC7 as data lines
// Using PB5 = R/S
// Using GND = R/W (read/write)
// Using PB7 = EN (enabler)
#include <stm32f446xx.h>
#include "LCD.h"

#define RS 0x20
#define EN 0x80

// Function to initialize LCD display
void LCD_Init(void) {
    GPIO_init();
	// Initialize Sequence
	delayMs(30);
	LCD_nibble_write(0x30, 0);
	delayMs(5);
	LCD_nibble_write(0x30, 0);
	delayMs(1);
	LCD_nibble_write(0x30, 0);
	delayMs(1);

	LCD_nibble_write(0x20, 0); // Set 4-bit data mode
	delayMs(1);

	LCD_command(0x28); // 4-bit mode, 2 line, 5x7 font
	LCD_command(0x06); // Move cursor right
	LCD_command(0x01); // clear screen
	LCD_command(0x0F); // Turn on display and blink cursor
}

void GPIO_init(void) {
	RCC->AHB1ENR |= 0x06; // Enable clock for PB and PC
	GPIOB->MODER |= (1 << (5 * 2)) | (1 << (7 * 2));  // PB5, PB7 as outputs
	GPIOB->BSRR = 0x00800000; // Clear EN

	GPIOC->MODER |= (1 << (4 * 2)) | (1 << (5 * 2)) | (1 << (6 * 2)) | (1 << (7 * 2));  // PC4-PC7 as outputs
}

void LCD_nibble_write(char data, unsigned char control) {
    // Clear the data bits (PC4-PC7) by setting the corresponding bits in BSRR
    GPIOC->BSRR = (0xF0 << 16);  // Clear PC4-PC7

    // Set the new 4-bit data (only the upper nibble is written)
    GPIOC->BSRR = (data & 0xF0);

    // Check if we are sending a command or data (RS = Register Select)
    if (control & RS) {
        GPIOB->BSRR = RS;  // Set RS to 1 for data mode
    } else {
        GPIOB->BSRR = RS << 16;  // Clear RS (set RS to 0 for command mode)
    }

    // Pulse the Enable (EN) pin to latch the data into the LCD
    GPIOB->BSRR = EN;  // Set EN high
    delayMs(1);        // Short delay for LCD to latch data
    GPIOB->BSRR = EN << 16;  // Set EN low
}

void LCD_command(unsigned char command) {
    // Send the upper 4 bits of the command
    LCD_nibble_write(command & 0xF0, 0);

    // Send the lower 4 bits of the command
    LCD_nibble_write(command << 4, 0);

    // Delay for execution time (longer for certain commands)
    if (command < 4) {
        delayMs(2);  // Commands like "clear display" need more time
    } else {
        delayMs(1);  // Other commands require a shorter delay
    }
}

void LCD_data(char data) {
    // Send the upper 4 bits of the data
    LCD_nibble_write(data & 0xF0, RS);

    // Send the lower 4 bits of the data
    LCD_nibble_write(data << 4, RS);

    // Short delay for the LCD to process the data
    delayMs(1);
}

void LCD_print(char *str) {
    while (*str) {
        LCD_data(*str++); // Send each character to the LCD
    }
}

void SysTick_init(void) {
	// Load value for 1ms delay (assuming 180MHz system clock)
	SysTick->LOAD = (180000000 / 1000) - 1;
	SysTick->VAL = 0; // Clear current value
	SysTick->CTRL = 0x05; // Enable SysTick with processor clock, no interrupt
}

void delayMs(int delay) {
	for (int i = 0; i < delay; i++) {
		SysTick->VAL = 0; // Clear current value
		while ((SysTick->CTRL & 0x10000) == 0); // wait until COUNTFLAG is set
	}
}
