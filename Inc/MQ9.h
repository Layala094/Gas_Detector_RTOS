#ifndef INC_MQ9_H_
#define INC_MQ9_H_

#define RL_VALUE 10.0 // Load resistance on kilo ohms
#define RO_CLEAN_AIR_FACTOR 9.6 // Based on the datasheet

#define ADC_RESOLUTION 4095.0f // 12-bit ADC (4095)
#define VREF 3.3f // the V<sub>REF</sub> is 3.3V

float mq9_sensor_calibration(void);
float mq9_read_sensor_resistance(uint32_t adc_value);
float mq9_get_gas_value(float ratio, const char gasType[]);

#endif /* INC_MQ9_H_ */
