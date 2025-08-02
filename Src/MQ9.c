// MQ-9 sensor source file for initialization and property functions
// Based on the datasheet: https://www.smart-prototyping.com/image/data/2_components/sensors/100996%20MQ-9/MQ9.pdf
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "stm32f446xx.h"
#include "adc.h"
#include "MQ9.h"

/**
 * @brief Calibrates the sensor in clean air.
 * @note  This function should be called before starting FreeRTOS tasks.
 *        It uses its own ADC reads and assumes no other tasks are accessing ADC.
 * @return Ro: Sensor baseline resistance in clean air.
 */
float mq9_sensor_calibration(void) {
	float rs_sum = 0.0f;
	printf("Calibrating in clean air. Please wait...\n");

	for (int i = 0; i < 100; i++) {
		extern uint32_t adc_read(void);
		extern void start_conversion(void);

		start_conversion();
		uint32_t raw = adc_read();

		float rs = mq9_read_sensor_resistance(raw);
		rs_sum += rs;

		// Optional: delay between readings if needed
		for (volatile int d = 0; d < 100000; ++d); // crude delay
	}

	float rs_avg = rs_sum / 100.0f;
	float ro = rs_avg / RO_CLEAN_AIR_FACTOR;
	return ro;
}

/**
 * @brief Reads sensor resistance (Rs) from a provided ADC value.
 * @param adc_value: Raw ADC value (0-4095)
 * @return Rs resistance in kilo-ohms
 */
float mq9_read_sensor_resistance(uint32_t adc_value) {
    float voltage = adc_value * (VREF / ADC_RESOLUTION);
    if (voltage <= 0.0f) voltage = 0.0001f;  // Prevent division by zero
    float Rs = (VREF - voltage) * RL_VALUE / voltage;
    return Rs;
}

/**
 * @brief Converts Rs/Ro ratio into gas concentration using log-log fit.
 * @param ratio: Rs/Ro ratio
 * @param gasType: "CO" or "CH4"
 * @return ppm value (0–10000), or -1 if invalid gas type
 */
float mq9_get_gas_value(float ratio, const char gasType[]) {
	float a, b;

	if (strcmp(gasType, "CO") == 0) {
		// Approximation from datasheet: log(ppm) = a * log(ratio) + b
		// Datasheet: log(200) = log(0.5), log(10) = log(2.5) -> m ~ -0.77
		a = -0.67;
		b = 1.24;
	} else if (strcmp(gasType, "CH4") == 0) {
		a = -0.49;
		b = 1.56;
	} else {
		return -1;
	}

	if (ratio <= 0.0f) return 0.0f;

	float logRatio = log10(ratio);
	float logPPM = (logRatio - b) / a;
	float ppm = pow(10, logPPM);

	if (isnan(ppm) || ppm < 0.0f || ppm > 10000.0f) return 0.0f;

	return ppm;
}
