/**
 * @file gesture_classifier.c
 * @brief Implementation of statistical feature extraction and 1-NN classification.
 */

#include "gesture_classifier.h"
#include <math.h>
#include <float.h>

/* Prototype feature vectors calibrated for 10 gestures (33 features per gesture) */
static float gesture_prototypes[NUM_GESTURE_CLASSES][FEATURE_VECTOR_DIM];

static const char* const GESTURE_LABELS[NUM_GESTURE_CLASSES] = {
    "A", "B", "C", "D", "E", "F", "G", "H", "I", "L"
};

void gesture_classifier_init(void) {
    /*
     * Synthetic baseline prototype values representative of normalized finger
     * flexions and wrist orientations. In a live system, these are populated
     * during the user calibration routine.
     */
    for (int c = 0; c < NUM_GESTURE_CLASSES; c++) {
        for (int f = 0; f < FEATURE_VECTOR_DIM; f++) {
            /* Distinct synthetic baseline offsets */
            gesture_prototypes[c][f] = ((float)(c + 1) * 0.12f) + ((float)(f % 5) * 0.05f);
        }
    }
}

void gesture_extract_features(const float window_data[TOTAL_CHANNELS][WINDOW_LENGTH_SAMPLES],
                              float out_features[FEATURE_VECTOR_DIM]) {
    for (int ch = 0; ch < TOTAL_CHANNELS; ch++) {
        float sum = 0.0f;
        float sum_sq = 0.0f;

        for (int n = 0; n < WINDOW_LENGTH_SAMPLES; n++) {
            float val = window_data[ch][n];
            sum += val;
            sum_sq += (val * val);
        }

        float mean = sum / (float)WINDOW_LENGTH_SAMPLES;
        float variance = (sum_sq / (float)WINDOW_LENGTH_SAMPLES) - (mean * mean);
        if (variance < 0.0f) {
            variance = 0.0f;
        }
        float rms = sqrtf(sum_sq / (float)WINDOW_LENGTH_SAMPLES);

        int base_idx = ch * FEATURES_PER_CHANNEL;
        out_features[base_idx + 0] = rms;
        out_features[base_idx + 1] = mean;
        out_features[base_idx + 2] = variance;
    }
}

GestureID_t gesture_classify(const float features[FEATURE_VECTOR_DIM], float rejection_threshold) {
    float min_dist_sq = FLT_MAX;
    int best_class_idx = -1;

    for (int c = 0; c < NUM_GESTURE_CLASSES; c++) {
        float dist_sq = 0.0f;

        for (int f = 0; f < FEATURE_VECTOR_DIM; f++) {
            float diff = features[f] - gesture_prototypes[c][f];
            dist_sq += (diff * diff);
        }

        if (dist_sq < min_dist_sq) {
            min_dist_sq = dist_sq;
            best_class_idx = c;
        }
    }

    if (min_dist_sq > (rejection_threshold * rejection_threshold)) {
        return GESTURE_REJECTED;
    }

    return (best_class_idx >= 0) ? (GestureID_t)best_class_idx : GESTURE_REJECTED;
}

const char* gesture_get_label(GestureID_t id) {
    if (id < NUM_GESTURE_CLASSES) {
        return GESTURE_LABELS[id];
    }
    return "UNKNOWN";
}
