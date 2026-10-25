/**
 * @file fir_filter.h
 * @brief Discrete-time FIR filter declarations for sensor signal conditioning.
 * @details Implements 41-tap Hamming-windowed low-pass filtering in both 32-bit
 *          single-precision float and Q15 fixed-point formats with circular buffer delay lines.
 */

#ifndef FIR_FILTER_H
#define FIR_FILTER_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define FIR_ORDER        (40)
#define FIR_NUM_TAPS     (FIR_ORDER + 1)
#define NUM_FLEX_CHANNELS (5)
#define NUM_IMU_CHANNELS  (6)
#define TOTAL_CHANNELS    (NUM_FLEX_CHANNELS + NUM_IMU_CHANNELS)

/**
 * @brief State structure for a single-channel floating-point FIR filter.
 */
typedef struct {
    float delay_line[FIR_NUM_TAPS];
    uint8_t write_index;
} FIRFilterFloat_t;

/**
 * @brief State structure for a single-channel Q15 fixed-point FIR filter.
 */
typedef struct {
    int16_t delay_line[FIR_NUM_TAPS];
    uint8_t write_index;
} FIRFilterQ15_t;

/**
 * @brief Initializes a floating-point FIR filter state.
 * @param filter Pointer to FIRFilterFloat_t structure.
 */
void fir_filter_float_init(FIRFilterFloat_t *filter);

/**
 * @brief Updates a floating-point FIR filter with a single new sample.
 * @param filter Pointer to FIRFilterFloat_t structure.
 * @param input_sample Sensed input value.
 * @return Filtered output sample.
 */
float fir_filter_float_update(FIRFilterFloat_t *filter, float input_sample);

/**
 * @brief Initializes a Q15 fixed-point FIR filter state.
 * @param filter Pointer to FIRFilterQ15_t structure.
 */
void fir_filter_q15_init(FIRFilterQ15_t *filter);

/**
 * @brief Updates a Q15 fixed-point FIR filter with a single new sample.
 * @param filter Pointer to FIRFilterQ15_t structure.
 * @param input_sample Raw 12-bit ADC reading scaled or zero-centered.
 * @return Filtered sample in Q15 format.
 */
int16_t fir_filter_q15_update(FIRFilterQ15_t *filter, int16_t input_sample);

#ifdef __cplusplus
}
#endif

#endif /* FIR_FILTER_H */
