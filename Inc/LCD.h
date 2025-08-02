
#ifndef INC_LCD_H_
#define INC_LCD_H_

void GPIO_init(void);
void SysTick_init(void);
void delayMs(int delay);
void LCD_data(char data);
void LCD_nibble_write(char data, unsigned char control);
void LCD_command(unsigned char command);
void LCD_Init(void);
void LCD_print(char *str);
void delay_us(int delay);

#endif /* INC_LCD_H_ */
