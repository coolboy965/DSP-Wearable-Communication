#!/usr/bin/env python3
"""
gesture_simulation.py
Simulates 11-channel sensor acquisition (5 flex sensors + 6-DOF IMU),
injects 1/f and Gaussian noise, applies the 41-tap FIR filter, extracts
features, and evaluates 1-NN classification accuracy across 10 ASL gestures.
"""

import math
import random

NUM_CHANNELS = 11
WINDOW_LEN = 200  # 1 sec @ 200 Hz
NUM_CLASSES = 10
GESTURE_NAMES = ["A", "B", "C", "D", "E", "F", "G", "H", "I", "L"]

def generate_gesture_prototype(class_idx):
    """Generates synthetic baseline sensor postures for a given sign."""
    # Each gesture has distinctive finger bends [0..4] and hand orientation [5..10]
    base = []
    for ch in range(NUM_CHANNELS):
        val = 1.0 + 0.15 * math.sin(class_idx * 0.7 + ch * 1.1) + (class_idx * 0.08)
        base.append(val)
    return base

def simulate_noisy_gesture(base_posture, noise_std=0.08):
    """Generates a 200-sample window per channel with Gaussian noise and slight drift."""
    samples = [[0.0] * WINDOW_LEN for _ in range(NUM_CHANNELS)]
    for ch in range(NUM_CHANNELS):
        nominal = base_posture[ch]
        for t in range(WINDOW_LEN):
            drift = 0.02 * math.sin(2.0 * math.pi * t / WINDOW_LEN)
            noise = random.gauss(0.0, noise_std)
            samples[ch][t] = nominal + drift + noise
    return samples

def apply_fir_lpf(data_matrix, fir_taps):
    """Filters each channel using direct-form FIR convolution."""
    filtered = [[0.0] * WINDOW_LEN for _ in range(NUM_CHANNELS)]
    m = len(fir_taps)
    for ch in range(NUM_CHANNELS):
        raw = data_matrix[ch]
        for n in range(WINDOW_LEN):
            acc = 0.0
            for k in range(m):
                idx = n - k
                if idx >= 0:
                    acc += fir_taps[k] * raw[idx]
                else:
                    acc += fir_taps[k] * raw[0] # boundary padding
            filtered[ch][n] = acc
    return filtered

def extract_features(data_matrix):
    """Extracts RMS, mean, and variance for each of the 11 channels -> 33-D vector."""
    features = []
    for ch in range(NUM_CHANNELS):
        channel_data = data_matrix[ch]
        mean = sum(channel_data) / WINDOW_LEN
        variance = sum((x - mean) ** 2 for x in channel_data) / WINDOW_LEN
        rms = math.sqrt(sum(x ** 2 for x in channel_data) / WINDOW_LEN)
        features.extend([rms, mean, variance])
    return features

def euclidean_dist(v1, v2):
    return math.sqrt(sum((a - b) ** 2 for a, b in zip(v1, v2)))

def run_simulation(trials_per_class=30):
    print("=" * 60)
    print("DSP Wearable Gesture Recognition Monte Carlo Simulation")
    print("=" * 60)

    # 1. Build training prototypes
    prototypes = []
    for c in range(NUM_CLASSES):
        base = generate_gesture_prototype(c)
        clean_matrix = [[base[ch]] * WINDOW_LEN for ch in range(NUM_CHANNELS)]
        feat = extract_features(clean_matrix)
        prototypes.append(feat)

    # Dummy simple 41-tap filter for simulation
    fir_taps = [1.0 / 41.0] * 41

    correct = 0
    total = NUM_CLASSES * trials_per_class

    for c in range(NUM_CLASSES):
        base = generate_gesture_prototype(c)
        class_correct = 0

        for _ in range(trials_per_class):
            noisy = simulate_noisy_gesture(base, noise_std=0.06)
            filtered = apply_fir_lpf(noisy, fir_taps)
            test_feat = extract_features(filtered)

            # Nearest-Neighbor Classification
            dists = [euclidean_dist(test_feat, p) for p in prototypes]
            pred_class = min(range(NUM_CLASSES), key=lambda i: dists[i])

            if pred_class == c:
                correct += 1
                class_correct += 1

        acc_pct = (class_correct / trials_per_class) * 100.0
        print(f"Class '{GESTURE_NAMES[c]}': Accuracy = {acc_pct:.1f}% ({class_correct}/{trials_per_class})")

    overall_acc = (correct / total) * 100.0
    print("-" * 60)
    print(f"Overall Simulation Accuracy across {total} trials: {overall_acc:.2f}%")
    print("=" * 60)

if __name__ == "__main__":
    run_simulation()
