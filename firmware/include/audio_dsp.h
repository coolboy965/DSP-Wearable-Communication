/**
 * @file audio_dsp.h
 * @brief Audio digital signal processing for the reverse speech-to-bone-conduction path.
 * @details Includes 2nd-order IIR biquad bandpass filter (300 Hz - 3400 Hz) and
 *          short-term RMS automatic gain control (AGC).
 */

#ifndef AUDIO_DSP_H
#define AUDIO_DSP_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define AUDIO_SAMPLE_RATE_HZ (16000)
#define AGC_WINDOW_SAMPLES   (160)    /* 10 ms at 16 kHz */

/**
 * @brief Biquad Direct Form II Transposed filter section.
 */
typedef struct {
    float b0, b1, b2;
    float a1, a2;
    float d1, d2;
} BiquadSection_t;

/**
 * @brief Audio processing state containing cascaded biquads and AGC parameters.
 */
typedef struct {
    BiquadSection_t hp_section; /* High-pass at 300 Hz */
    BiquadSection_t lp_section; /* Low-pass at 3400 Hz */
    float agc_gain;
    float target_rms;
    float current_energy_sum;
    uint16_t sample_counter;
} AudioDSPState_t;

/**
 * @brief Initializes audio bandpass filter and AGC controller.
 * @param state Pointer to AudioDSPState_t.
 * @param target_rms Target output RMS amplitude (e.g., 0.125f for -18 dBFS).
 */
void audio_dsp_init(AudioDSPState_t *state, float target_rms);

/**
 * @brief Processes a single audio sample through bandpass filter and AGC.
 * @param state Pointer to AudioDSPState_t.
 * @param input_sample Floating point sample in [-1.0f, 1.0f].
 * @return Conditioned output sample destined for DAC / transducer.
 */
float audio_dsp_process_sample(AudioDSPState_t *state, float input_sample);

#ifdef __cplusplus
}
#endif

#endif /* AUDIO_DSP_H */
