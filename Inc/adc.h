#ifndef ADC_H_
#define ADC_H_

#include <stdint.h>

#define GPIOAEN        (1U<<0)
#define ADC1_EN        (1U<<8)
#define ADC_CH4        (4U<<0)
#define ADC_SEQ_LEN_1   0x00
#define CR2_ADON       (1U<<0)
#define CR2_SWSTART    (1U<<30)
#define SR_EOC         (1U<<1)
#define CR2_CONT       (1U<<1)
#define CR1_EOCIE      (1U<<5)

void pa4_adc_interrupt_init(void);
void pa4_adc_init(void);
void start_conversion(void);
uint32_t adc_read(void);

#endif /* ADC_H_ */

