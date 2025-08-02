#ifndef INC_TASKS_H_
#define INC_TASKS_H_

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "queue.h"

typedef struct {
    uint32_t adc_value;
    float rs;
    float ratio;
    float co_ppm;
    float ch4_ppm;
} MQ9Data;

extern QueueHandle_t adcQueue;
extern QueueHandle_t mq9Queue;

void Task_ADC_Read(void *pvParameters);
void Task_Calculate(void *pvParameters);
void Task_Display(void *pvParameters);
void Task_Alarm(void *pvParameters);

#endif /* INC_TASKS_H_ */
