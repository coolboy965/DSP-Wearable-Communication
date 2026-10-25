/**
 * @file fir_filter.c
 * @brief Discrete-time FIR filter implementations for gesture sensor channels.
 */

#include "fir_filter.h"
#include <string.h>

/*
 * 41-Tap Symmetric Hamming-Windowed Sinc Low-Pass Filter Coefficients
 * Sampling frequency fs = 200 Hz, Cutoff frequency fc = 50 Hz (fc_norm = 0.25).
 * Stopband attenuation > 40 dB above 60 Hz.
 */
static const float H_FLOAT[FIR_NUM_TAPS] = {
    -0.00064087f, -0.00140381f, -0.00177002f, -0.00125122f,  0.00057983f,
     0.00372314f,  0.00750732f,  0.01055908f,  0.01110840f,  0.00756836f,
    -0.00006104f, -0.01055908f, -0.02151489f, -0.02975464f, -0.03118896f,
    -0.02294922f, -0.00317383f,  0.02655029f,  0.06253052f,  0.09829712f,
     0.12500000f,  0.09829712f,  0.06253052f,  0.02655029f, -0.00317383f,
    -0.02294922f, -0.03118896f, -0.02975464f, -0.02151489f, -0.01055908f,
    -0.00006104f,  0.00756836f,  0.01110840f,  0.01055908f,  0.00750732f,
     0.00372314f,  0.00057983f, -0.00125122f, -0.00177002f, -0.00140381f,
    -0.00064087f
};

/* Q15 fixed-point representation: round(H_FLOAT[k] * 32768) */
static const int16_t H_Q15[FIR_NUM_TAPS] = {
     -21,  -46,  -58,  -41,   19,  122,  246,  346,  364,  248,
      -2, -346, -705, -975,-1022, -752, -104,  870, 2049, 3221,
    4096, 3221, 2049,  870, -104, -752,-1022, -975, -705, -346,
      -2,  248,  364,  346,  246,  122,   19,  -41,  -58,  -46,
     -21
};

void fir_filter_float_init(FIRFilterFloat_t *filter) {
    if (filter != NULL) {
        memset(filter->delay_line, 0, sizeof(filter->delay_line));
        filter->write_index = 0;
    }
}

float fir_filter_float_update(FIRFilterFloat_t *filter, float input_sample) {
    filter->delay_line[filter->write_index] = input_sample;

    float accumulator = 0.0f;
    uint8_t read_idx = filter->write_index;

    for (uint8_t tap = 0; tap < FIR_NUM_TAPS; tap++) {
        accumulator += H_FLOAT[tap] * filter->delay_line[read_idx];
        if (read_idx == 0) {
            read_idx = FIR_NUM_TAPS - 1;
        } else {
            read_idx--;
        }
    }

    filter->write_index = (filter->write_index + 1) % FIR_NUM_TAPS;
    return accumulator;
}

void fir_filter_q15_init(FIRFilterQ15_t *filter) {
    if (filter != NULL) {
        memset(filter->delay_line, 0, sizeof(filter->delay_line));
        filter->write_index = 0;
    }
}

int16_t fir_filter_q15_update(FIRFilterQ15_t *filter, int16_t input_sample) {
    filter->delay_line[filter->write_index] = input_sample;

    int32_t accumulator = 0;
    uint8_t read_idx = filter->write_index;

    for (uint8_t tap = 0; tap < FIR_NUM_TAPS; tap++) {
        accumulator += (int32_t)H_Q15[tap] * (int32_t)filter->delay_line[read_idx];
        if (read_idx == 0) {
            read_idx = FIR_NUM_TAPS - 1;
        } else {
            read_idx--;
        }
    }

    filter->write_index = (filter->write_index + 1) % FIR_NUM_TAPS;
    /* Scale down by 15-bit shift (Q15 format) */
    return (int16_t)(accumulator >> 15);
}
