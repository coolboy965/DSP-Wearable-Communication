/**
 * @file gesture_classifier.h
 * @brief Feature extraction and 1-NN prototype classifier for gesture recognition.
 */

#ifndef GESTURE_CLASSIFIER_H
#define GESTURE_CLASSIFIER_H

#include <stdint.h>
#include <stdbool.h>
#include "fir_filter.h"

#ifdef __cplusplus
extern "C" {
#endif

#define WINDOW_LENGTH_SAMPLES (200)   /* 1.0 second window at 200 Sa/s */
#define FEATURES_PER_CHANNEL  (3)     /* RMS, Mean, Variance */
#define FEATURE_VECTOR_DIM    (TOTAL_CHANNELS * FEATURES_PER_CHANNEL) /* 11 * 3 = 33 */
#define NUM_GESTURE_CLASSES   (10)    /* 10 ASL signs/letters */

typedef enum {
    GESTURE_A = 0,
    GESTURE_B,
    GESTURE_C,
    GESTURE_D,
    GESTURE_E,
    GESTURE_F,
    GESTURE_G,
    GESTURE_H,
    GESTURE_I,
    GESTURE_L,
    GESTURE_REJECTED = 0xFF
} GestureID_t;

/**
 * @brief Initializes the gesture classifier with calibrated prototype vectors.
 */
void gesture_classifier_init(void);

/**
 * @brief Computes RMS, mean, and variance from a window of samples.
 * @param window_data Buffer of dimension [TOTAL_CHANNELS][WINDOW_LENGTH_SAMPLES].
 * @param out_features Output array of length FEATURE_VECTOR_DIM (33 elements).
 */
void gesture_extract_features(const float window_data[TOTAL_CHANNELS][WINDOW_LENGTH_SAMPLES],
                              float out_features[FEATURE_VECTOR_DIM]);

/**
 * @brief Classifies a feature vector using 1-nearest-neighbor with rejection threshold.
 * @param features Input feature vector of length FEATURE_VECTOR_DIM.
 * @param rejection_threshold Maximum allowed Euclidean distance before rejection.
 * @return Detected GestureID_t or GESTURE_REJECTED.
 */
GestureID_t gesture_classify(const float features[FEATURE_VECTOR_DIM], float rejection_threshold);

/**
 * @brief Retrieves the human-readable text corresponding to a gesture ID.
 * @param id Gesture ID.
 * @return Null-terminated string.
 */
const char* gesture_get_label(GestureID_t id);

#ifdef __cplusplus
}
#endif

#endif /* GESTURE_CLASSIFIER_H */
