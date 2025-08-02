#include <stdio.h>
#include "tasks.h"
#include "adc.h"
#include "mq9.h"
#include "lcd.h"
#include "stm32f4xx_hal.h"

QueueHandle_t adcQueue;
QueueHandle_t mq9Queue;

void Task_ADC_Read(void *pvParameters) {
	extern volatile sensorADC_debug;
	uint32_t value;
	    while (1) {
	        value = adc_read();
	        xQueueSend(adcQueue, &value, portMAX_DELAY);
	        sensorADC_debug = value;
	        vTaskDelay(pdMS_TO_TICKS(200));
	    }

}

void Task_Calculate(void *pvParameters) {
    uint32_t adcVal;
    MQ9Data data;

    float Ro = mq9_sensor_calibration();

    while (1) {
        if (xQueueReceive(adcQueue, &adcVal, portMAX_DELAY)) {
            float voltage = adcVal * (3.3f / 4095.0f);
            float Rs = (3.3f - voltage) * 10.0f / voltage;
            float ratio = Rs / Ro;
            float co = mq9_get_gas_value(ratio, "CO");
            float ch4 = mq9_get_gas_value(ratio, "CH4");

            data.adc_value = adcVal;
            data.rs = Rs;
            data.ratio = ratio;
            data.co_ppm = co;
            data.ch4_ppm = ch4;

            xQueueSend(mq9Queue, &data, portMAX_DELAY);
        }
    }
}

void Task_Display(void *pvParameters) {
    MQ9Data data;
    char buffer[32];
    while (1) {
        if (xQueuePeek(mq9Queue, &data, portMAX_DELAY)) {
            LCD_command(0x80);
            snprintf(buffer, sizeof(buffer), "CO: %.1fppm", data.co_ppm);
            LCD_print(buffer);

            LCD_command(0xC0);
            snprintf(buffer, sizeof(buffer), "CH4: %.1fppm", data.ch4_ppm);
            LCD_print(buffer);

            vTaskDelay(pdMS_TO_TICKS(1000));
        }
    }
}

void Task_Alarm(void *pvParameters) {
    MQ9Data data;
    while (1) {
        if (xQueuePeek(mq9Queue, &data, portMAX_DELAY)) {
            if (data.co_ppm > 200 || data.ch4_ppm > 300) {
                HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_1);
                vTaskDelay(pdMS_TO_TICKS(250));
            } else {
                HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
                vTaskDelay(pdMS_TO_TICKS(100));
            }
        }
    }
}

