/**
 * @file audio_dsp.c
 * @brief Implementation of IIR biquad bandpass filter and dynamic AGC.
 */

#include "audio_dsp.h"
#include <math.h>

static inline float process_biquad(BiquadSection_t *s, float in) {
    float out = (s->b0 * in) + s->d1;
    s->d1 = (s->b1 * in) - (s->a1 * out) + s->d2;
    s->d2 = (s->b2 * in) - (s->a2 * out);
    return out;
}

void audio_dsp_init(AudioDSPState_t *state, float target_rms) {
    if (state == NULL) return;

    /*
     * 2nd-Order Butterworth High-Pass at fc = 300 Hz, fs = 16 kHz
     * Bilinear transformation with pre-warping.
     */
    state->hp_section.b0 = 0.920066f;
    state->hp_section.b1 = -1.840133f;
    state->hp_section.b2 = 0.920066f;
    state->hp_section.a1 = -1.833732f;
    state->hp_section.a2 = 0.846533f;
    state->hp_section.d1 = 0.0f;
    state->hp_section.d2 = 0.0f;

    /*
     * 2nd-Order Butterworth Low-Pass at fc = 3400 Hz, fs = 16 kHz
     */
    state->lp_section.b0 = 0.245237f;
    state->lp_section.b1 = 0.490474f;
    state->lp_section.b2 = 0.245237f;
    state->lp_section.a1 = -0.197825f;
    state->lp_section.a2 = 0.178774f;
    state->lp_section.d1 = 0.0f;
    state->lp_section.d2 = 0.0f;

    state->agc_gain = 1.0f;
    state->target_rms = (target_rms > 0.001f) ? target_rms : 0.125f;
    state->current_energy_sum = 0.0f;
    state->sample_counter = 0;
}

float audio_dsp_process_sample(AudioDSPState_t *state, float input_sample) {
    /* 1. Bandpass filtering: HP cascaded with LP */
    float hp_out = process_biquad(&state->hp_section, input_sample);
    float bp_out = process_biquad(&state->lp_section, hp_out);

    /* 2. Track short-term energy */
    state->current_energy_sum += (bp_out * bp_out);
    state->sample_counter++;

    /* 3. Update AGC gain periodically (every 10 ms = 160 samples) */
    if (state->sample_counter >= AGC_WINDOW_SAMPLES) {
        float window_rms = sqrtf(state->current_energy_sum / (float)AGC_WINDOW_SAMPLES);
        if (window_rms > 0.002f) {
            float desired_gain = state->target_rms / window_rms;
            /* Exponential smoothing on gain: alpha = 0.1 */
            state->agc_gain = (0.9f * state->agc_gain) + (0.1f * desired_gain);

            /* Limit gain range between 0.1x and 10.0x */
            if (state->agc_gain > 10.0f) state->agc_gain = 10.0f;
            if (state->agc_gain < 0.1f)  state->agc_gain = 0.1f;
        }
        state->current_energy_sum = 0.0f;
        state->sample_counter = 0;
    }

    /* 4. Apply gain and soft clipping */
    float out = bp_out * state->agc_gain;
    if (out > 1.0f)  out = 1.0f;
    if (out < -1.0f) out = -1.0f;

    return out;
}
