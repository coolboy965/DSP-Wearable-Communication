/**
 * @file main.c
 * @brief Top-level firmware for DSP-Based Wearable Communication System.
 * @details Integrates the Gesture-to-Speech forward path and
 *          Speech-to-Bone-Conduction reverse path on an ARM Cortex-M4 microcontroller.
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "fir_filter.h"
#include "gesture_classifier.h"
#include "audio_dsp.h"

/* State structures */
static FIRFilterFloat_t  sensor_filters[TOTAL_CHANNELS];
static AudioDSPState_t   audio_dsp_state;
static float             gesture_window[TOTAL_CHANNELS][WINDOW_LENGTH_SAMPLES];
static uint16_t          window_write_ptr = 0;

/* Simulated Hardware Peripheral Handlers */
void hardware_init(void) {
    /* Initialize sensor filters */
    for (int i = 0; i < TOTAL_CHANNELS; i++) {
        fir_filter_float_init(&sensor_filters[i]);
    }

    /* Initialize classifier prototypes */
    gesture_classifier_init();

    /* Initialize audio DSP chain (-18 dBFS target) */
    audio_dsp_init(&audio_dsp_state, 0.125f);

    printf("[SYSTEM] Peripheral initialization complete.\n");
    printf("[SYSTEM] 11-Channel ADC initialized at 200 Sa/s.\n");
    printf("[SYSTEM] I2S Audio interface initialized at 16 kHz.\n");
}

/**
 * @brief Called every 5 ms by ADC DMA interrupt (200 Hz).
 * @param raw_adc_data Raw 12-bit ADC / IMU readings [0..TOTAL_CHANNELS-1].
 */
void on_sensor_sample_interrupt(const float raw_adc_data[TOTAL_CHANNELS]) {
    for (int ch = 0; ch < TOTAL_CHANNELS; ch++) {
        /* Apply 41-tap FIR low-pass filter (50 Hz cutoff) */
        float filtered = fir_filter_float_update(&sensor_filters[ch], raw_adc_data[ch]);
        gesture_window[ch][window_write_ptr] = filtered;
    }

    window_write_ptr++;
    if (window_write_ptr >= WINDOW_LENGTH_SAMPLES) {
        window_write_ptr = 0;

        /* Extract 33 features */
        float features[FEATURE_VECTOR_DIM];
        gesture_extract_features(gesture_window, features);

        /* Classify gesture */
        GestureID_t detected = gesture_classify(features, 2.5f);
        if (detected != GESTURE_REJECTED) {
            const char* label = gesture_get_label(detected);
            printf("[GESTURE] Recognized: %s -> Triggering Speech Synthesizer\n", label);
        } else {
            /* Input rejected or ambiguous gesture */
            printf("[GESTURE] Undefined posture (rejected)\n");
        }
    }
}

/**
 * @brief Called by I2S DMA interrupt at 16 kHz for incoming speech.
 * @param mic_sample Input sample from Knowles MEMS microphone.
 * @return Conditioned sample written to DAC / PAM8403 / Bone-Conduction Transducer.
 */
float on_audio_sample_interrupt(float mic_sample) {
    return audio_dsp_process_sample(&audio_dsp_state, mic_sample);
}

int main(void) {
    hardware_init();

    printf("[SYSTEM] Running DSP Wearable Communication System...\n");

    /* Simulated execution test */
    float mock_raw_sensors[TOTAL_CHANNELS];
    for (int ch = 0; ch < TOTAL_CHANNELS; ch++) {
        mock_raw_sensors[ch] = 1.65f; /* Mid-scale baseline */
    }

    for (int step = 0; step < 205; step++) {
        on_sensor_sample_interrupt(mock_raw_sensors);
    }

    float test_mic_sample = 0.05f;
    float transducer_output = on_audio_sample_interrupt(test_mic_sample);
    printf("[AUDIO] Transducer output sample: %f\n", transducer_output);

    return 0;
}
