#include "stm32f446xx.h"
#include "adc.h"

// Function to initialize ADC interrupt to PA4
void pa4_adc_interrupt_init(void) {
	/* Configure the ADC GPIO pin */
	// Enable clock access to GPIOA
	RCC->AHB1ENR |= GPIOAEN;

	// Set the mode of PA4 to analog
	GPIOA->MODER |= (1U<<8);
	GPIOA->MODER |= (1U<<9);

	/* Configure the ADC module */
	// Enable clock access to ADC
	RCC->APB2ENR |= ADC1_EN;

	// enable clock access to ADC end-of conversion interrupt
	ADC1->CR1 |= CR1_EOCIE;

	// enable ADC interrupt in NVIC
	NVIC_EnableIRQ(ADC_IRQn);

	// Conversion sequence start
	ADC1->SQR3 = ADC_CH4;

	// Conversion sequence length
	ADC1->SQR1 = ADC_SEQ_LEN_1;

	// Enable ADC module
	ADC1->CR2 |= CR2_ADON;
}


// Function to initialize ADC to PA4
void pa4_adc_init(void) {
	/* Configure the ADC GPIO pin */
	// Enable clock access to GPIOA
	RCC->AHB1ENR |= GPIOAEN;

	// Set the mode of PA4 to analog
	GPIOA->MODER |= (1U<<8);
	GPIOA->MODER |= (1U<<9);

	/* Configure the ADC module */
	// Enable clock access to ADC
	RCC->APB2ENR |= ADC1_EN;

	// Conversion sequence start
	ADC1->SQR3 = ADC_CH4;

	// Conversion sequence length
	ADC1->SQR1 = ADC_SEQ_LEN_1;

	// Enable ADC module
	ADC1->CR2 |= CR2_ADON;
}

void start_conversion(void) {
	// Enable continous conversion
	ADC1->CR2 |= CR2_CONT;

	// Start the ADC conversion
	ADC1->CR2 |= CR2_SWSTART;
}

uint32_t adc_read(void) {
	uint32_t timeout = 100000; // Adjust based on clock speed
	while (!(ADC1->SR & SR_EOC) && --timeout);
	return timeout ? ADC1->DR : 0;
}
